/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "CSIR"
// Copyright (c) 2014 CSIR.  All rights reserved.
#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveRtspServer.h>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <liveMedia.hh>
#include <RTSPCommon.hh> // for snprintf
#include <LiveMediaExt/LiveMediaSubsession.h>
#include <LiveMediaExt/LiveMediaSubsessionFactory.h>

namespace lme
{

/// DUP from RTSPServer.cpp
// Generate a "Date:" header for use in a RTSP response:
static char const* _dateHeader() {
  static char buf[200];
#if !defined(_WIN32_WCE)
  time_t tt = time(NULL);
  strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
#else
  // WinCE apparently doesn't have "time()", "strftime()", or "gmtime()",
  // so generate the "Date:" header a different, WinCE-specific way.
  // (Thanks to Pierre l'Hussiez for this code)
  SYSTEMTIME SystemTime;
  GetSystemTime(&SystemTime);
  WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
  WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
  WCHAR inBuf[200];
  DWORD locale = LOCALE_NEUTRAL;

  int ret = GetDateFormat(locale, 0, &SystemTime,
    (LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof inBuf);
  inBuf[ret - 1] = ' ';
  ret = GetTimeFormat(locale, 0, &SystemTime,
    (LPTSTR)timeFormat,
    (LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
  wcstombs(buf, inBuf, wcslen(inBuf));
#endif
  return buf;
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
  char const* streamName) {
  char* url = rtspServer->rtspURL(sms);
  UsageEnvironment& env = rtspServer->envir();
  env << "\n\"" << streamName << "\" stream\n";
  env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
}

static char const* allowedCommandNames
  = "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";

LiveRtspServer* LiveRtspServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase, 
                                          IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl)
{
	int ourSocket = -1;

	do 
  {
		int ourSocket = setUpOurSocket(env, ourPort);
		if (ourSocket == -1) break;

		LiveRtspServer* pRtspServer = new LiveRtspServer(env, ourSocket, ourPort, authDatabase, pFactory, pGlobalRateControl);
    return pRtspServer;

	} while (0);

	if (ourSocket != -1) ::closeSocket(ourSocket);
	return NULL;
}

LiveRtspServer::LiveRtspServer(UsageEnvironment& env, int ourSocket, Port ourPort, UserAuthenticationDatabase* authDatabase,
  IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl)
  : RTSPServer(env, ourSocket, ourPort, authDatabase, 45),
  m_checkClientSessionTask(NULL),
  m_uiMaxConnectedClients(0),
  m_pFactory(pFactory),
  m_pGlobalRateControl(pGlobalRateControl)
{
  checkClientSessions();
}

LiveRtspServer::~LiveRtspServer() 
{
  // In cases where the client is still streaming when
  // the server is shutdown, the super class (RTSPServer)
  // deletes the client session objects resulting in them
  // trying to deregister themselves from LiveRtspServer.
  // We need to orphan them before to avoid crashes!
  for (LiveClientSessionMap_t::iterator it = m_mRtspClientSessions.begin(); it != m_mRtspClientSessions.end(); ++it)
  {
    it->second->orphan();
  }

  // Turn off any liveness checking:
  envir().taskScheduler().unscheduleDelayedTask(m_checkClientSessionTask);
}

// fwd
static ServerMediaSession* createNewSMS(UsageEnvironment& env, LiveRtspServer& rRtspServer, const Channel& channel, IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl); 

void LiveRtspServer::addRtspMediaSession(const Channel& channel)
{
  const std::string sSessionName = channel.ChannelName;
	// Next, check whether we already have an RTSP "ServerMediaSession" for this media stream:
	ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(sSessionName.c_str());
	Boolean bSmsExists = sms != NULL;
	if (!bSmsExists)
	{
    VLOG(2) << "Creating Session " << sSessionName << " on RTSP server";
		// Create a new "ServerMediaSession" object for streaming from the named file.
		sms = createNewSMS(envir(), *this, channel, m_pFactory, m_pGlobalRateControl);
    VLOG(2) << "Adding ServerMediaSession " << sSessionName;
		addServerMediaSession(sms);
    announceStream(this, sms, sSessionName.c_str());
	}
  else
  {
    LOG(WARNING) << "Session " << sSessionName << " already exists on RTSP server";
  }
}

void LiveRtspServer::removeRtspMediaSession(const Channel& channel)
{
  /// code to kick clients before removing session so that there are no outstanding references
  endServerSession(channel.ChannelName);

	// Check whether we already have a "ServerMediaSession" for this media stream:
  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(channel.ChannelName.c_str());
	
	if (sms != NULL)
	{
    VLOG(2) << "Removed session Channel Id: " << channel.ChannelId << " Name: " << channel.ChannelName << " from RTSP ServerMediaSessions";
		// "sms" was created for a media stream that the transcoder is no longer sending. Remove it.
		removeServerMediaSession(sms);
	}
	else
	{
    LOG(WARNING) << "Failed removing session " << channel.ChannelName << " from RTSP ServerMediaSessions";
	}
}


ServerMediaSession* LiveRtspServer::lookupServerMediaSession(char const* streamName) 
{
  VLOG(2) << "Looking up new ServerMediaSession: " << streamName;
  return RTSPServer::lookupServerMediaSession(streamName);
}

#define NEW_SMS(description) do {\
	char* descStr = description\
	", streamed by the LIVE555 Media Server";\
	sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);\
} while(0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env, LiveRtspServer& rRtspServer, const Channel& channel, 
                                        IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl)
{
  VLOG(2) << "createNewSMS: " << channel.ChannelName;
  ServerMediaSession* sms = ServerMediaSession::createNew(env, channel.ChannelName.c_str(), channel.ChannelName.c_str(), "Session streamed by \"MSS\"", False/*SSM*/);

  // at least a video or audio descriptor must be set
  assert(channel.VideoDescriptor || channel.AudioDescriptor);

  // FIXME: hard-coded in application and lib for now!
  uint32_t uiVideoId = 0;
  uint32_t uiAudioId = 0;

  // create the live media RTSP subsessions
  if (channel.VideoDescriptor)
  {
    // if there is a video channel, the audio source id increases!
    uiAudioId = 1;
    LiveMediaSubsession* pLiveMediaSubsession = LiveMediaSubsessionFactory::createVideoSubsession(env, rRtspServer, channel.ChannelName, channel.ChannelId, uiVideoId, *(channel.VideoDescriptor), 
                                                                                                  pFactory, pGlobalRateControl);
    if (pLiveMediaSubsession == NULL)
    {
      LOG(WARNING) << "TODO: Invalid video subsession";
      Medium::close(sms);
      return NULL;
    }
    else
    {
      VLOG(2) << "Added " << channel.ChannelName << " to video ServerMediaSession";
      sms->addSubsession(pLiveMediaSubsession);
      pLiveMediaSubsession->setClientJoinHandler(boost::bind(&LiveRtspServer::onClientJoin, boost::ref(rRtspServer), _1, _2, _3, _4));
      pLiveMediaSubsession->setClientUpdateHandler(boost::bind(&LiveRtspServer::onClientUpdate, boost::ref(rRtspServer), _1, _2, _3, _4));
      pLiveMediaSubsession->setClientLeaveHandler(boost::bind(&LiveRtspServer::onClientLeave, boost::ref(rRtspServer), _1, _2, _3));
    }
  }

  // create the live media RTSP subsessions
  if (channel.AudioDescriptor)
  {
    LiveMediaSubsession* pLiveMediaSubsession = LiveMediaSubsessionFactory::createAudioSubsession(env, rRtspServer, channel.ChannelName, channel.ChannelId, uiAudioId, *(channel.AudioDescriptor), 
                                                                                                  pFactory, pGlobalRateControl);
    if (pLiveMediaSubsession == NULL)
    {
      LOG(WARNING) << "TODO: Invalid audio subsession";
      Medium::close(sms);
      return NULL;
    }
    else
    {
      VLOG(2) << "Added " << channel.ChannelName << " audio to ServerMediaSession";
      sms->addSubsession(pLiveMediaSubsession);
      pLiveMediaSubsession->setClientJoinHandler(boost::bind(&LiveRtspServer::onClientJoin, boost::ref(rRtspServer), _1, _2, _3, _4));
      pLiveMediaSubsession->setClientUpdateHandler(boost::bind(&LiveRtspServer::onClientUpdate, boost::ref(rRtspServer), _1, _2, _3, _4));
      pLiveMediaSubsession->setClientLeaveHandler(boost::bind(&LiveRtspServer::onClientLeave, boost::ref(rRtspServer), _1, _2, _3));
    }
  }

  return sms;
}

void LiveRtspServer::checkClientSessions()
{
  envir().taskScheduler()
      .rescheduleDelayedTask(m_checkClientSessionTask,
			     1000000, // in microseconds
			     (TaskFunc*)clientSessionsTask, this);

}

void LiveRtspServer::clientSessionsTask(LiveRtspServer* pRtspServer)
{
  pRtspServer->doCheckClientSessions();
}

void LiveRtspServer::doCheckClientSessions()
{
  // Process all receiver reports

  // schedule next task
  checkClientSessions();
}

void LiveRtspServer::kickClient(unsigned uiClientId)
{
  // find corresponding client session
  LiveClientSessionMap_t::iterator it = m_mRtspClientSessions.find(uiClientId);
  if (it != m_mRtspClientSessions.end())
  {
    VLOG(15) << "Kicking client - " << uiClientId;
    // acording to live555 mailing list, deletion of a client session cleanly removes the session
    delete it->second;
  }
  else
  {
    // couldn't find client in map
    VLOG(15) << "Unable to kick client - not found " << uiClientId;
  }
}

void LiveRtspServer::endServerSession(const std::string& sSession)
{
  // Next, check whether we already have an RTSP "ServerMediaSession" for this media stream:
  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(sSession.c_str());

  if (sms)
  {
    VLOG(2) << "TODO: Kick all clients from RTSP media session: " << sSession;
    // First kick all clients so that we can remove the subsession and dereg from the scheduler
    // The LiveMediaSubsession objects can only be reclaimed once there are no outstanding references
    ServerMediaSubsessionIterator iter(*sms);
    ServerMediaSubsession* pSub = iter.next();

    // stores set of unique ids
    std::set<unsigned> uniqueClientIds;

    while (pSub != NULL) 
    {
      // Kick all clients
      LiveMediaSubsession* pSubsession = static_cast<LiveMediaSubsession*>(pSub);
      // assuming our server only contains LiveMediaSubsession
      // in the case that it does not, we could use dynamic_cast but this shouldn't be the case
      std::vector<unsigned> vClientIds = pSubsession->getConnectedClientIds();
      // the live555 assigns the same client id to the audio and video subsession
      // this next part could be superfluous since the first subsession SHOULD contain all IDs
      // but playing it on the safe side for now
      std::copy(vClientIds.begin(), vClientIds.end(), inserter(uniqueClientIds, uniqueClientIds.end()));
      pSub = iter.next();
    }

    // now kick all clients
    BOOST_FOREACH(unsigned uiId, uniqueClientIds)
    {
      kickClient(uiId);
    }

    VLOG(2) << "Kicking complete: " << sSession;
  }
  else
  {
    LOG(WARNING) << "Unable to find session " << sSession;
  }
}

unsigned LiveRtspServer::getMaxConnectedClients() const
{
  return m_uiMaxConnectedClients;
}

void LiveRtspServer::setMaxConnectedClients( unsigned val )
{
  m_uiMaxConnectedClients = val;
#pragma chMSG(TODO: if val is less than previous value kick oldest clients to meet requirement)
}

void LiveRtspServer::onRtspClientSessionPlay(unsigned uiClientSessionId)
{
  if (m_onClientSessionPlay) m_onClientSessionPlay(uiClientSessionId);
}

void LiveRtspServer::onClientJoin(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId, std::string& sIpAddress)
{
  VLOG(5) << "Client joined: Channel: " << uiChannelId << ":" << uiSourceId << " client ID: " << uiChannelId << " IP: " << sIpAddress;
}

void LiveRtspServer::onClientUpdate(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId, uint32_t uiChannelIndex)
{
  VLOG(5) << "Client joined: Channel: " << uiChannelId << ":" << uiSourceId << " client ID: " << uiChannelId << " Channel index: " << uiChannelIndex;
}

void LiveRtspServer::onClientLeave(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId)
{
  VLOG(5) << "Client left: Channel: " << uiChannelId << ":" << uiSourceId << " client ID: " << uiChannelId;
}

} // lme


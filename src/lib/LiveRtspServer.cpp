#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveRtspServer.h>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <boost/foreach.hpp>
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

static char const* allowedCommandNames
  = "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER";

LiveRtspServer* LiveRtspServer::createNew(UsageEnvironment& env, Port ourPort, UserAuthenticationDatabase* authDatabase )
{
	int ourSocket = -1;

	do 
  {
		int ourSocket = setUpOurSocket(env, ourPort);
		if (ourSocket == -1) break;

		LiveRtspServer* pRtspServer = new LiveRtspServer(env, ourSocket, ourPort, authDatabase);
    return pRtspServer;

	} while (0);

	if (ourSocket != -1) ::closeSocket(ourSocket);
	return NULL;
}

LiveRtspServer::LiveRtspServer(UsageEnvironment& env, int ourSocket, Port ourPort, UserAuthenticationDatabase* authDatabase)
	: RTSPServer(env, ourSocket, ourPort, authDatabase, 45),
  m_checkClientSessionTask(NULL),
  m_uiMaxConnectedClients(0)
{
  checkClientSessions();
}

LiveRtspServer::~LiveRtspServer() 
{
  // Turn off any liveness checking:
  envir().taskScheduler().unscheduleDelayedTask(m_checkClientSessionTask);
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env, LiveRtspServer& rRtspServer, char const* fileName/*, TranscoderSessionManager::ptr pSessionManager*/); // forward

void LiveRtspServer::addRtspMediaSession(const std::string& sSessionName)
{
	// Check if the transcoder manager has such a session
  //TranscoderSessionManager::ptr pTranscoderSessionManager = m_pTranscoderSessionManager.lock();
  //assert(pTranscoderSessionManager);
	//Boolean bSessionExists = pTranscoderSessionManager->exists(sSessionName);

	// Next, check whether we already have an RTSP "ServerMediaSession" for this media stream:
	//ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(sSessionName.c_str());
	//Boolean bSmsExists = sms != NULL;

 // VLOG(5) << "Session " << sSessionName << " status - Transcoder Session Manager: " << (int) bSessionExists << " RTSPServer: " << (int) bSmsExists;

	//// Handle the four possibilities for "sessionExists" and "smsExists":
	//if (!bSessionExists) 
	//{
	//	if (bSmsExists) 
	//	{
 //     VLOG(2) << "Removing ServerMediaSession " << sSessionName;
	//		// "sms" was created for a media stream that the transcoder is no longer sending. Remove it.
	//		removeServerMediaSession(sms);
	//	}
 //   else
 //   {
 //     VLOG(2) << "Session " << sSessionName << " doesn't exist, no need to remove";
 //   }
	//}
	//else 
	//{
	//	if (!bSmsExists)
	//	{
 //     VLOG(2) << "Creating Session " << sSessionName << " on RTSP server";
	//		// Create a new "ServerMediaSession" object for streaming from the named file.
	//		sms = createNewSMS(envir(), *this, sSessionName.c_str(), pTranscoderSessionManager);
 //     VLOG(2) << "Adding ServerMediaSession " << sSessionName;
	//		addServerMediaSession(sms);
	//	}
 //   else
 //   {
 //     VLOG(2) << "Session " << sSessionName << " already on RTSP server";
 //   }
	//}
}

void LiveRtspServer::removeRtspMediaSession(const std::string& sSessionName)
{
	//// Check whether we already have a "ServerMediaSession" for this media stream:
	//ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(sSessionName.c_str());
	//
	//if (sms != NULL)
	//{
 //   VLOG(2) << "Removed session " << sSessionName << " from RTSP ServerMediaSessions";
	//	// "sms" was created for a media stream that the transcoder is no longer sending. Remove it.
	//	removeServerMediaSession(sms);
	//}
	//else
	//{
 //   LOG(WARNING) << "Failed removing session " << sSessionName << " from RTSP ServerMediaSessions";
	//}
}


ServerMediaSession* LiveRtspServer::lookupServerMediaSession(char const* streamName) 
{
 // VLOG(2) << "Looking up new ServerMediaSession: " << streamName;
	//// Check if the transcoder manager has such a session
 // TranscoderSessionManager::ptr pTranscoderSessionManager = m_pTranscoderSessionManager.lock();
 // assert(pTranscoderSessionManager);

 // Boolean sessionExists = pTranscoderSessionManager->exists(streamName);

	//// Next, check whether we already have a "ServerMediaSession" for this media stream:
	//ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName);
	//Boolean smsExists = sms != NULL;

	//// Handle the four possibilities for "sessionExists" and "smsExists":
	//if (!sessionExists) 
	//{
 //   VLOG(2) << "TranscoderSession " << streamName << " does not exist";
	//	if (smsExists) 
	//	{
 //     // "sms" was created for a media stream that the transcoder is no longer sending. Remove it.
 //     VLOG(2) << "Removing liveMedia server session for " << streamName;
 //     // remove session from RTSP server: this results in the deletion of the LiveLiveMediaSubsessions
 //     // as long as there are no reference counts (due to connected clients)
	//		removeServerMediaSession(sms);
	//	}
	//	// This will result in an RTSP Stream not found response to the client
	//	return NULL;
	//}
	//else 
	//{
	//	if (!smsExists) 
	//	{
	//		// Create a new "ServerMediaSession" object for streaming from the named file.
 //     VLOG(2) << "Creating liveMedia server session for " << streamName;
	//		sms = createNewSMS(envir(), *this, streamName, pTranscoderSessionManager);
	//		addServerMediaSession(sms);
	//	}
 //   else
 //   {
 //     VLOG(2) << "Server media session already exists " << streamName;
 //   }
	//	return sms;
	//}
  return NULL;
}

#define NEW_SMS(description) do {\
	char* descStr = description\
	", streamed by the LIVE555 Media Server";\
	sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);\
} while(0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env, LiveRtspServer& rRtspServer, char const* streamName/*, TranscoderSessionManager::ptr pTranscoderSessionManager*/) 
{
  //// Use the session manager to obtain the information about the media streams
  //TranscoderClientSession* pTranscoderClientSession = pTranscoderSessionManager->find(streamName);

  //if (pTranscoderClientSession)
  //{
  //  VLOG(2) << "Retrieved " << streamName << " from transcoder manager";

  //  ServerMediaSession* sms = ServerMediaSession::createNew(env, streamName, streamName, "Session streamed by \"MSS\"", True/*SSM*/);

  //  MediaSession mediaSession = pTranscoderClientSession->getMediaSession();
  //  const MediaSubsessionPtrList_t& vSubsessions = mediaSession.getMediaSubsessions();

  //  try
  //  {
  //    // For each subsession get the media attributes and create a source and sink
  //    for (MediaSubsessionPtrList_t::const_iterator it = vSubsessions.begin(); it != vSubsessions.end();it++)
  //    {
  //      // Depending on the subsession type, create a live media subsession matching it
  //      MediaSubsession::ptr pSubsession = (*it);

  //      unsigned uiPort(0);

  //      if (mediaSession.multiplexMedia())
  //      {
  //        // use session port
  //        uiPort = mediaSession.getPort();
  //      }
  //      else
  //      {
  //        // use subsession port
  //        uiPort = pSubsession->getPort();
  //      }

  //      assert(uiPort > 0);
  //      // unique ID of channel
  //      unsigned uiChannelId(mediaSession.getChannelId());
  //      // subsession id
  //      unsigned uiSubsessionId(pSubsession->getId());
  //      // generated id used for routing of media packets from ports to correct media subsession
  //      unsigned uiLiveMediaSessionId = generateMediaSubsessionIdHash(uiPort, uiSubsessionId);
  //      LOG(WARNING) << "ChannelId: " << uiChannelId << " Port: " << uiPort << ", Subsession Id: " << uiSubsessionId << " Generated live media session id : " << uiLiveMediaSessionId;

  //      // create the live media RTSP subsession
  //      LiveMediaSubsession* pLiveMediaSubsession = LiveMediaSubsessionFactory::create(env, rRtspServer,  streamName, uiChannelId, uiLiveMediaSessionId, pSubsession);
  //      if (pLiveMediaSubsession == NULL)
  //      {
  //        LOG(WARNING) << "TODO: Invalid subsession";
  //        // RG: live555 update makes destructor protected!
  //        // TODO: revise
  //        // delete sms;
  //        return NULL;
  //      }
  //      else
  //      {
  //        sms->addSubsession(pLiveMediaSubsession);
  //      }
  //    }
  //    // return created SMS
  //    return sms;
  //  }
  //  catch(const Exception& e)
  //  {
  //    LOG(WARNING) << "Error creating server media session: " << e.message();
  //    // RG: live555 update makes destructor protected!
  //    // TODO: revise
  //    //delete sms;
  //    return NULL;
  //  }


  //}
  //else
  //{
  //  LOG(WARNING) << "TODO: Failed to get session from transcoder session manager";
	 // // The live555 code returns a NULL pointer when the session doesn't exist, so this shouldn't break anything...
	 // return NULL;
  //}
  return NULL;
}

void LiveRtspServer::checkClientSessions()
{
  envir().taskScheduler()
      .rescheduleDelayedTask(m_checkClientSessionTask,
			     100000, // in microseconds
			     (TaskFunc*)clientSessionsTask, this);

}

void LiveRtspServer::clientSessionsTask(LiveRtspServer* pRtspServer)
{
  pRtspServer->doCheckClientSessions();
}

void LiveRtspServer::doCheckClientSessions()
{
  // DO something here

  // schedule next task
  checkClientSessions();
}

void LiveRtspServer::endServerSession(const std::string& sSession)
{
  // Next, check whether we already have an RTSP "ServerMediaSession" for this media stream:
  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(sSession.c_str());

  if (sms)
  {
    //VLOG(2) << "Kicking all clients from RTSP media session: " << sSession;
    //// First kick all clients so that we can remove the subsession and dereg from the scheduler
    //// The LiveMediaSubsession objects can only be reclaimed once there are no outstanding references
    //ServerMediaSubsessionIterator iter(*sms);
    //ServerMediaSubsession* pSub = iter.next();

    //// stores set of unique ids
    //std::set<unsigned> uniqueClientIds;

    //while (pSub != NULL) 
    //{
    //  // Kick all clients
    //  LiveMediaSubsession* pSubsession = static_cast<LiveMediaSubsession*>(pSub);
    //  // assuming our server only contains LiveMediaSubsession
    //  // in the case that it does not, we could use dynamic_cast but this shouldn't be the case
    //  std::vector<unsigned> vClientIds = pSubsession->getConnectedClientIds();
    //  // the live555 assigns the same client id to the audio and video subsession
    //  // this next part could be superfluous since the first subsession SHOULD contain all IDs
    //  // but playing it on the safe side for now
    //  std::copy(vClientIds.begin(), vClientIds.end(), inserter(uniqueClientIds, uniqueClientIds.end()));

    //  pSub = iter.next();
    //}

    //// now kick all clients
    //BOOST_FOREACH(unsigned uiId, uniqueClientIds)
    //{
    //  kickClient(uiId);
    //}

    //VLOG(2) << "Kicking complete: " << sSession;
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

}


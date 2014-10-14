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
#pragma once
#ifndef _RTSP_SERVER_HH
#include <RTSPServer.hh>
#endif
#include <map>
#include <vector>
#include <BasicUsageEnvironment.hh>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/mutex.hpp>
#include <Media/AudioChannelDescriptor.h>
#include <Media/VideoChannelDescriptor.h>

namespace lme
{

// fwd
class IRateAdaptationFactory;
class IRateController;

/**
 * @brief Channel descriptor
 */
struct Channel
{
  Channel()
    :ChannelId(0)
  {

  }

  Channel(uint32_t uiChannelId, const std::string& sChannelName, VideoChannelDescriptor video, AudioChannelDescriptor audio)
    :ChannelId(uiChannelId),
    ChannelName(sChannelName),
    VideoDescriptor(video),
    AudioDescriptor(audio)
  {

  }
  Channel(uint32_t uiChannelId, const std::string& sChannelName, VideoChannelDescriptor video)
    :ChannelId(uiChannelId),
    ChannelName(sChannelName),
    VideoDescriptor(video)
    {

    }
    Channel(uint32_t uiChannelId, const std::string& sChannelName, AudioChannelDescriptor audio)
    :ChannelId(uiChannelId),
    ChannelName(sChannelName),
    AudioDescriptor(audio)
    {

    }

    uint32_t ChannelId;
    std::string ChannelName;
    boost::optional<VideoChannelDescriptor> VideoDescriptor;
    boost::optional<AudioChannelDescriptor> AudioDescriptor;
};

/**
 * Our RTSP server class is derived from the liveMedia RTSP server. It extends the live555 RTSP server
 * to stream live media sessions.
 */
class LiveRtspServer: public RTSPServer
{
  friend class LiveRTSPClientSession;
public:
	/**
   * @brief Named constructor
   */
  static LiveRtspServer* createNew(UsageEnvironment& env, Port ourPort = 554, UserAuthenticationDatabase* authDatabase = NULL,
                                   IRateAdaptationFactory* pFactory = NULL, IRateController* pGlobalRateControl = NULL);

  unsigned getMaxConnectedClients() const;
  void setMaxConnectedClients(unsigned val);

  unsigned getNumberOfConnectedClients() const { return m_mRtspClientSessions.size(); }

	/// adds an RTSP session for the transcoder session provided the transcoder session exists in the transcoder manager
	void addRtspMediaSession(const Channel& channel);
	/// removes an RTSP session for the corresponding transcoder session from the RTSP server
  void removeRtspMediaSession(const Channel& channel);

protected:
  void endServerSession(const std::string& sSessionName);

protected:
  class LiveRTSPClientSession;
  /**
   * @brief Subclassing this to make the client address acessible and add handleCmd_notEnoughBandwidth.
   */
  class LiveRTSPClientConnection : public RTSPClientConnection
  {
    friend class LiveRTSPClientSession;
  public:
    /**
     * @brief Constructor
     */
    LiveRTSPClientConnection(RTSPServer& ourServer, int clientSocket, struct sockaddr_in clientAddr)
      :RTSPClientConnection(ourServer, clientSocket, clientAddr)
    {
    }
    /**
     * @brief Destructor
     */
    virtual ~LiveRTSPClientConnection()
    {
    }
  protected:
    /**
     * @brief Getter for client address
     */
    struct sockaddr_in getClientAddr() const { return fClientAddr; }
    /**
     * @brief This method can be called to respond to requests where there is insufficient bandwidth
     * to handle them.
     */
    virtual void handleCmd_notEnoughBandwidth()
    {
      setRTSPResponse("453 Not Enough Bandwidth");
    }
  };

  class LiveRTSPClientSession : public RTSPServer::RTSPClientSession 
  {
  public:
    LiveRTSPClientSession(LiveRtspServer& ourServer, unsigned sessionId)
      : RTSPClientSession(ourServer, sessionId),
      m_pParent(&ourServer),
      m_uiSessionId(sessionId)
    {

    }

    virtual ~LiveRTSPClientSession()
    {
      // We need to check if the parent is still valid
      // in the case where the client session outlives the
      // RTSPServer child class implementation!
      if (m_pParent)
        m_pParent->removeClientSession(m_uiSessionId);
    }

    void orphan()
    {
      m_pParent = NULL;
    }
  protected:
    /**
     * @brief Overriding this to limit the maximum number of clients that can connect to the RTSP server
     */ 
    virtual void handleCmd_SETUP(RTSPServer::RTSPClientConnection* ourClientConnection,
      char const* urlPreSuffix, char const* urlSuffix,
      char const* fullRequestStr)
    {
      // "urlPreSuffix" should be the session (stream) name, and
      // "urlSuffix" should be the subsession (track) name.

      if (m_pParent->getMaxConnectedClients() == 0)
      {
        // no limitiations set: let base class handle it
        RTSPClientSession::handleCmd_SETUP(ourClientConnection, urlPreSuffix, urlSuffix, fullRequestStr);
        return;
      }

      char const* streamName = urlPreSuffix;
      char const* trackId = urlSuffix;

      // Check whether we have existing session state, and, if so, whether it's
      // for the session that's named in "streamName".  (Note that we don't
      // support more than one concurrent session on the same client connection.) #####
      if (fOurServerMediaSession == NULL)
      {
        // this is a new session: reject the user if we have reached the server limit
        if (m_pParent->getNumberOfConnectedClients() <= m_pParent->getMaxConnectedClients())
        {
          RTSPClientSession::handleCmd_SETUP(ourClientConnection, urlPreSuffix, urlSuffix, fullRequestStr);
          return;
        }
        else
        {
          LiveRTSPClientConnection* pRtvcConnection = dynamic_cast<LiveRTSPClientConnection*>(ourClientConnection);
          assert(pRtvcConnection);

          // Convert IpAddress to readable format
          struct in_addr destinationAddr; destinationAddr.s_addr = pRtvcConnection->getClientAddr().sin_addr.s_addr;
          std::string sIpAddress(inet_ntoa(destinationAddr));
 	        pRtvcConnection->handleCmd_notEnoughBandwidth();
          return;
        }
      }
      else
      {
        // SETUP in existing session: let base class handle it
        RTSPClientSession::handleCmd_SETUP(ourClientConnection, urlPreSuffix, urlSuffix, fullRequestStr);
        return;
      }
    }

    LiveRtspServer* m_pParent;
    unsigned m_uiSessionId;
  };

    // If you subclass "RTSPClientConnection", then you must also redefine this virtual function in order
  // to create new objects of your subclass:
  virtual RTSPClientConnection*
  createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr)
  {
    return new LiveRTSPClientConnection(*this, clientSocket, clientAddr);
  }

  // If you subclass "RTSPClientSession", then you must also redefine this virtual function in order
  // to create new objects of your subclass:
  virtual RTSPClientSession*
  createNewClientSession(u_int32_t sessionId)
  {
    LiveRTSPClientSession* pSession = new LiveRTSPClientSession(*this, sessionId);
    m_mRtspClientSessions[sessionId] = pSession;
    return pSession;
  }

  void removeClientSession(unsigned sessionId)
  {
    LiveClientSessionMap_t::iterator it = m_mRtspClientSessions.find(sessionId);
    if (it != m_mRtspClientSessions.end())
    {
      VLOG(15) << "Removing client session " << sessionId;
      m_mRtspClientSessions.erase(it);
    }
    else
    {
      VLOG(15) << "Unable to remove client session " << sessionId;
    }
  }

private:
	/**
   * @brief Constructor: called only by createNew();
   */
  LiveRtspServer(UsageEnvironment& env, int ourSocket, Port ourPort, UserAuthenticationDatabase* authDatabase, 
                 IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl);
	/**
   * @brief Destructor
   */
	virtual ~LiveRtspServer();

private: 
	/// redefined virtual functions: this method returns the session identified by streamName provided its valid
	virtual ServerMediaSession* lookupServerMediaSession(char const* streamName);

  typedef std::map<unsigned, LiveRTSPClientSession*> LiveClientSessionMap_t;
  /// map to store a pointer to client sessions on creation
  LiveClientSessionMap_t m_mRtspClientSessions;

  void checkClientSessions();
  void doCheckClientSessions();
  static void clientSessionsTask(LiveRtspServer* pRtspServer);
  TaskToken m_checkClientSessionTask;

  /// maximum permitted number of clients that can be served by this RTSP server
  /// a value of 0 means that no restrictions exist
  unsigned m_uiMaxConnectedClients;
  /// adaptation factory 
  IRateAdaptationFactory* m_pFactory;
  /// Rate control
  IRateController* m_pGlobalRateControl;
};

} // lme



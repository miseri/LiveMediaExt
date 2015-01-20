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
#include <boost/function.hpp>
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
 * @brief Callback for when a client session is PLAYed.
 * @param client session id
 */
typedef boost::function<void(unsigned)> OnClientSessionPlayHandler;

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
 *
 * It also adds the capability to set the maximum number of connected clients.
 * It adds the ability to kick clients off the server.
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
  /**
   * @brief Getter for max connected clients
   */
  unsigned getMaxConnectedClients() const;
  /**
   * @brief Setter for max connected clients
   */
  void setMaxConnectedClients(unsigned val);
  /**
   * @brief Getter for number of connected clients
   */
  unsigned getNumberOfConnectedClients() const { return m_mRtspClientSessions.size(); }
	/**
   * @brief adds the RTSP session described by channel to the RTSP server if it doesn't already exist.
   */
	void addRtspMediaSession(const Channel& channel);
	/**
   * @brief removes the RTSP session described by channel from the RTSP server.
   */
  void removeRtspMediaSession(const Channel& channel);
  /**
   * @brief Setter for notifications when a client session is issues the PLAY command to the RTSP server.
   */
  void setOnClientSessionPlayCallback(OnClientSessionPlayHandler onClientSessionPlay){ m_onClientSessionPlay = onClientSessionPlay; }
  /**
   * @brief handler to be called when clients join
   */
  void onClientJoin(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId, std::string& sIpAddress);
  /**
   * @brief handler to be called when clients update
   */
  void onClientUpdate(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId, uint32_t uiChannelIndex);
  /**
   * @brief handler to be called when clients leave
   */
  void onClientLeave(uint32_t uiChannelId, uint32_t uiSourceId, uint32_t uiClientId);

protected:
  /**
   * @brief ends the server session
   *
   * To do this it must first kick all clients connected to the session.
   */
  void endServerSession(const std::string& sSessionName);
  /**
   * @brief This is called when the client session processes the 
   * RTSP PLAY request.
   */
  void onRtspClientSessionPlay(unsigned uiClientSessionId);
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
      // RTSPServer child class implementation! In that case
      // the RTSPServer destructor deletes all the client 
      // sessions, but at this point m_pParent is not valid 
      // anymore. This is the reason for the orphan method.
      if (m_pParent)
        m_pParent->removeClientSession(m_uiSessionId);
    }
    /**
     * @brief invalidates the pointer to the LiveRtspServer object.
     */
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
          VLOG(2) << "TODO: parse request str to get user: " << fullRequestStr;
          return;
        }
        else
        {
          LiveRTSPClientConnection* pRtvcConnection = dynamic_cast<LiveRTSPClientConnection*>(ourClientConnection);
          assert(pRtvcConnection);

          // Convert IpAddress to readable format
          struct in_addr destinationAddr; destinationAddr.s_addr = pRtvcConnection->getClientAddr().sin_addr.s_addr;
          std::string sIpAddress(inet_ntoa(destinationAddr));
          VLOG(5) << "Max connections (" << m_pParent->getMaxConnectedClients() << ") reached.Rejecting connection request from " << sIpAddress;
 	        pRtvcConnection->handleCmd_notEnoughBandwidth();
          return;
        }
      }
      else
      {
        // SETUP in existing session: let base class handle it
        RTSPClientSession::handleCmd_SETUP(ourClientConnection, urlPreSuffix, urlSuffix, fullRequestStr);
        VLOG(2) << "TODO: parse request str to get user: " << fullRequestStr;
        return;
      }
    }
    /**
     * @brief Overriding this to fire off a callback informing the RTSP service that a new client has PLAYed the stream.
     *
     * This is useful in the case of a live media pipeline as it allows e.g. IDR frame generation.
     */
    virtual void handleCmd_PLAY(RTSPClientConnection* ourClientConnection,
      ServerMediaSubsession* subsession, char const* fullRequestStr)
    {
      m_pParent->onRtspClientSessionPlay(m_uiSessionId);
      RTSPClientSession::handleCmd_PLAY(ourClientConnection, subsession, fullRequestStr);
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
  /**
   * @brief Kicks clients from the server
   * this method SHOULD only be called from within the live555 eventloop!
   */
  void kickClient(unsigned uiClientId);

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
  /// On client session play callback
  OnClientSessionPlayHandler m_onClientSessionPlay;
};

} // lme



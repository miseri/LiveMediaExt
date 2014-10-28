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
#include <map>
#include <vector>
#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif
#include <Media/MediaSample.h>

namespace lme
{

// Forward
class IMediaSampleBuffer;
class LiveDeviceSource;
class LiveRtspServer;
class IRateAdaptationFactory;
class IRateController;

// Typedefs
typedef std::vector<LiveDeviceSource*> LiveDeviceSourcePtrList_t;

/**
 * @brief Base class for live media subsessions.
 *
 * This class is the entry point into the live555 class hierarchy:
 * - createNewStreamSource and createNewRTPSink allow overriding of
 *   source and sink creation
 * - getStreamParameters and deleteStream allow overriding for the
 *   purpose of client connection management
 * createSubsessionSpecificSource() and createSubsessionSpecificRTPSink() 
 * must be overridden by subclasses to create the appropriate sources and sinks.
 */
class LiveMediaSubsession : public OnDemandServerMediaSubsession
{
public:
  /**
   * @brief Destructor
   */
	virtual ~LiveMediaSubsession();
  /**
   * @brief Getter for source ID
   */
	uint32_t getSourceID() const { return m_uiSourceID; }
  /**
   * @brief Getter for RTSP Session name
   */
  std::string getSessionName() const { return m_sSessionName; }
  /**
   * @brief Getter for is this an video subsession
   */
  bool isVideo() const { return m_bVideo; }
  /**
   * @brief Getter for is this an audio subsession
   */
  bool isAudio() const { return !m_bVideo; }
  /**
   * @brief Subsession is switchable if there is more than one channel
   */
  bool isSwitchable() const { return m_uiTotalChannels > 1; }
	/**
   * @brief register live device source with subsession
   */
	void addDeviceSource(LiveDeviceSource* pDeviceSource);
	/**
   * @brief deregister live device source from subsession
   */
	void removeDeviceSource(LiveDeviceSource* pDeviceSource);
	/**
   * @brief Add a media sample to the subsession
   */
  virtual void addMediaSample(const MediaSample& mediaSample);
  /**
   * @brief This method processes the received receiver reports
   */
  void processClientStatistics();
protected:
  /**
   * @brief Constructor
   *
   * The uiChannelId + uiSourceID is used to allow this register itself with the scheduler on construction
	 * and to deregister itself from the scheduler on destruction
   */
  LiveMediaSubsession(UsageEnvironment& env, LiveRtspServer& rParent, 
                          const unsigned uiChannelId, unsigned uiSourceID, 
                          const std::string& sSessionName, 
                          bool bVideo, const unsigned uiTotalChannels = 1,
                          IRateAdaptationFactory* pFactory = NULL,
                          IRateController* pGlobalRateControl = NULL);

  /**
   * @brief This method must be overridden by subclasses
   * @param clientSessionId [in] The id assigned to the client by live555
   * @param pMediaSampleBuffer [in] The media sample buffer that the device 
   *        source will retrieve sample from
   * @param pRateAdaptationFactory Factory used to create rate adaptation module
   * @param pRateControl Rate control to be used for subsession. This allows the subsession to
   *        create different rate-control mechanisms based on the type of media subsession.
   */
  virtual FramedSource* createSubsessionSpecificSource(unsigned clientSessionId,
                                                       IMediaSampleBuffer* pMediaSampleBuffer, 
                                                       IRateAdaptationFactory* pRateAdaptationFactory,
                                                       IRateController* pRateControl) = 0;
  /**
   * @brief This method must be overridden by subclasses
   * The implementation should be the same as for createNewRTPSink. We intercept the method.
   */
  virtual RTPSink* createSubsessionSpecificRTPSink(Groupsock* rtpGroupsock, 
                                                   unsigned char rtpPayloadTypeIfDynamic, 
                                                   FramedSource* inputSource) = 0;
  /**
   * @brief Subclasses must override this instead of createNewStreamSource
   */
  virtual void setEstimatedBitRate(unsigned& estBitrate) = 0;
protected:
  /**
   * @brief Overridden from OnDemandServermediaSubsession for RTP source creation
   */
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	/**
   * @brief Overridden from OnDemandServermediaSubsession for RTP sink creation. "estBitrate" is the stream's estimated bitrate, in kbps
   */
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	/**
   * @brief Overridden so that we can store the client connection info of connecting RTP clients
   */
	virtual void getStreamParameters(unsigned clientSessionId,
		netAddressBits clientAddress,
		Port const& clientRTPPort,
		Port const& clientRTCPPort,
		int tcpSocketNum,
		unsigned char rtpChannelId,
		unsigned char rtcpChannelId,
		netAddressBits& destinationAddress,
		u_int8_t& destinationTTL,
		Boolean& isMulticast,
		Port& serverRTPPort,
		Port& serverRTCPPort,
		void*& streamToken);
	/**
   * @brief Overridden so that we can manage connecting client info
   */
	virtual void deleteStream(unsigned clientSessionId, void*& streamToken);

protected:
  /// Rtsp server
  LiveRtspServer& m_rRtspServer;
  /// Unique channel Id: channels are assigned by the media web server
  uint32_t m_uiChannelId;
  /// Session ID that is used to register the media subsession with the scheduler
  /// The channel ID is not sufficient since a single channel will have at least 
  /// one audio and one video subsession
  uint32_t m_uiSourceID;
  /// RTSP session name
  std::string m_sSessionName;
  /// RTP clients listed for this subsession 
	LiveDeviceSourcePtrList_t m_vDeviceSources;
  /// video or audio: helps find applicable sessions
  bool m_bVideo;
  /// Total number of channels: more than one implies that a parallel sample buffer will be created
  /// The term 'channel' in this context is not to be confused with the channels in the audio media subtype
  unsigned m_uiTotalChannels;
  /// Buffer for the samples
  IMediaSampleBuffer* m_pSampleBuffer;
  /// Rate adaptation factory
  IRateAdaptationFactory* m_pFactory;
  /// Global rate control: depending on the type of rate control i.e. per source or per client.
  IRateController* m_pGlobalRateControl;
};

typedef std::vector<LiveMediaSubsession*> LiveMediaSubsessionPtrList_t; 

} // lme


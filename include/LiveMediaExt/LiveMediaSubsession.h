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

// Typedefs
typedef std::vector<LiveDeviceSource*> LiveDeviceSourcePtrList_t;

/**
 * @brief This class contains the common media subsession functionality
 *
 * This class is the entry point into the live555 class hierarchy:
 * - createNewStreamSource and createNewRTPSink allow overriding of
 *   source and sink creation
 * - getStreamParameters and deleteStream allow overriding for the
 *   purpose of client connection management
 * createSubsessionSpecificSource must be overridden by subclasses
 * to create the appropriate sources.
 */
class LiveMediaSubsession : public OnDemandServerMediaSubsession
{
public:
  /**
   * @brief Destructor
   */
	virtual ~LiveMediaSubsession();

	/// source ID
	uint32_t getSourceID() const { return m_uiSourceID; }

  /// RTSP Session name
  std::string getSessionName() const { return m_sSessionName; }
  /// accessor for media type
  bool isVideo() const { return m_bVideo; }
  bool isAudio() const { return !m_bVideo; }
  /// Subsession is switchable if there is more than one channel
  bool isSwitchable() const { return m_uiTotalChannels > 1; }
    
	/// The liveMedia event loop is single threaded: we don't need to lock the vector
	void addDeviceSource(LiveDeviceSource* pDeviceSource);
	/// The liveMedia event loop is single threaded: we don't need to lock the vector
	void removeDeviceSource(LiveDeviceSource* pDeviceSource);

	/// Add a media sample to the subsession
  virtual void addMediaSample(const MediaSample& mediaSample);

  //std::vector<unsigned> getConnectedClientIds() const;

protected:

  /// This method must be overridden by subclasses
  /// @param clientSessionId [in] The id assigned to the client by live555
  /// @param pMediaSampleBuffer [in] The media sample buffer that the device 
  ///        source will retrieve sample from
  virtual FramedSource* createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer) = 0;

	/// The uiChannelId + uiSubsessionID is used to allow this register itself with the scheduler on construction
	/// and to deregister itself from the scheduler on destruction
	LiveMediaSubsession(UsageEnvironment& env, LiveRtspServer& rParent, 
                          const unsigned uiChannelId, unsigned uiSourceID, 
                          const std::string& sSessionName, 
                          bool bVideo, const unsigned uiTotalChannels = 1 );

	/// Subclasses must override this instead of createNewStreamSource
	virtual void setEstimatedBitRate(unsigned& estBitrate) = 0;

	/// Overridden from OnDemandServermediaSubsession for RTP source creation
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);

	/// Overridden from OnDemandServermediaSubsession for RTP sink creation. "estBitrate" is the stream's estimated bitrate, in kbps
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource) = 0;

	/// Overridden so that we can store the client connection info of connecting RTP clients
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

	/// Overridden so that we can manage connecting client info
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
};

typedef std::vector<LiveMediaSubsession*> LiveMediaSubsessionPtrList_t; 

} // lme


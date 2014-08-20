#pragma once
#include "LiveMediaSubsession.h"

namespace lme
{

// Forward
class IMediaSampleBuffer;
class MediaSubsession;

class LiveAMRSubsession : public LiveMediaSubsession
{
public:
  LiveAMRSubsession(UsageEnvironment& env, LiveRtspServer& rParent, 
    const unsigned uiChannelId, unsigned uniqueSessionID, 
    const std::string& sSessionName,
    const unsigned uiNumChannels, const unsigned uiBitsPerSample, const unsigned uiSamplingFrequency);

  virtual ~LiveAMRSubsession();

protected:

  /// Overridden from RtvcLiveMediaSubsession
  virtual LiveDeviceSource* createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer);
	// Overridden from RtvcLiveMediaSubsession
	virtual void setEstimatedBitRate(unsigned& estBitrate);
  /// Overriding from OnDemandServermediaSubsession
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
	/// Parameters required to create Device Source
	unsigned m_numChannels;
	unsigned m_bitsPerSample;
	unsigned m_samplingFrequency;
	unsigned m_bitsPerSecond;
};

} // lme


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
    const unsigned uiNumChannels, const unsigned uiBitsPerSample, const unsigned uiSamplingFrequency,
    IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl);

  virtual ~LiveAMRSubsession();

protected:

  /// Overridden from LiveMediaSubsession
  virtual FramedSource* createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer, 
                                                       IRateAdaptationFactory* pRateAdaptationFactory, IRateController* pRateControl);
	// Overridden from LiveMediaSubsession
	virtual void setEstimatedBitRate(unsigned& estBitrate);
  /// Overriding from LiveMediaSubsession
  virtual RTPSink* createSubsessionSpecificRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
	/// Parameters required to create Device Source
	unsigned m_numChannels;
	unsigned m_bitsPerSample;
	unsigned m_samplingFrequency;
	unsigned m_bitsPerSecond;
};

} // lme


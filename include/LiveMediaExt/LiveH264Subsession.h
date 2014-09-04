#pragma once
#include "LiveMediaSubsession.h"

namespace lme
{

// fwd
class LiveRtspServer;

class LiveH264Subsession : public LiveMediaSubsession
{
public:
  LiveH264Subsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                      const unsigned uiChannelId, unsigned uiSourceId, 
                      const std::string& sSessionName, 
                      const std::string& sSps, const std::string& sPps,
                      IRateAdaptationFactory* pFactory,
                      IRateController* pGlobalRateControl);

	virtual ~LiveH264Subsession();

protected:

  /// Overridden from RtvcLiveMediaSubsession
  virtual FramedSource* createSubsessionSpecificSource(unsigned clientSessionId, 
                                                       IMediaSampleBuffer* pMediaSampleBuffer, 
                                                       IRateAdaptationFactory* pRateAdaptationFactory,
                                                       IRateController* pRateControl);
  /// Overridden from
  virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
	/// Overridden from RtvcLiveMediaSubsession
	virtual void setEstimatedBitRate(unsigned& estBitrate);
  /// Overriding from LiveMediaSubsession
  virtual RTPSink* createSubsessionSpecificRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
  std::string m_sSps;
  std::string m_sPps;
  char* fAuxSDPLine;
  char* fFmtpSDPLine;
};

} // lme

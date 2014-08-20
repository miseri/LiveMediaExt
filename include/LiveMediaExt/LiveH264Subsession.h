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
                      const std::string& sSps, const std::string& sPps);
	virtual ~LiveH264Subsession(void);

protected:

  /// Overridden from RtvcLiveMediaSubsession
  virtual LiveDeviceSource* createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer);
  /// Overridden from
  virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
	/// Overridden from RtvcLiveMediaSubsession
	virtual void setEstimatedBitRate(unsigned& estBitrate);
  /// Overriding from OnDemandServermediaSubsession
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
  std::string m_sSps;
  std::string m_sPps;
  char* fAuxSDPLine;
  char* fFmtpSDPLine;
};

} // lme

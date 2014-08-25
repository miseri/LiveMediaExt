#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveAMRSubsession.h>
#include <LiveMediaExt/LiveAMRAudioDeviceSource.h>
#include <LiveMediaExt/LiveAMRAudioRTPSink.h>
#include <LiveMediaExt/LiveRtspServer.h>

namespace lme
{

LiveAMRSubsession::LiveAMRSubsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                                      const unsigned uiChannelId, unsigned uniqueSessionID, 
                                      const std::string& sSessionName,
                                      const unsigned uiNumChannels, const unsigned uiBitsPerSample, const unsigned uiSamplingFrequency )
  :LiveMediaSubsession(env, rParent, uiChannelId, uniqueSessionID, sSessionName, false, 1),
  m_numChannels(uiNumChannels),
  m_bitsPerSample(uiBitsPerSample),
  m_samplingFrequency(uiSamplingFrequency),
  m_bitsPerSecond(m_samplingFrequency * m_bitsPerSample * m_numChannels)
{
#if 0
  RtvcLogger& rLogger = RtvcLogger::getInstance();
  "Subsession created: Sampling frequency: " << m_samplingFrequency << "Hz Bits per sample: " << m_bitsPerSample << " Channels: " << m_numChannels << " Bits per second: " << m_bitsPerSecond);
	env << "Audio source parameters:\n\t" << m_samplingFrequency << " Hz, ";
	env << m_bitsPerSample << " bits-per-sample, ";
	env << m_numChannels << " channels => ";
	env << m_bitsPerSecond << " bits-per-second\n";
#endif
}

LiveAMRSubsession::~LiveAMRSubsession()
{

}

FramedSource* LiveAMRSubsession::createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer)
{
  return LiveAMRAudioDeviceSource::createNew(envir(), clientSessionId, this, pMediaSampleBuffer);
}

void LiveAMRSubsession::setEstimatedBitRate( unsigned& estBitrate )
{
	// Set estimated session band width (add 500 for integer arith rounding)
	estBitrate = (m_bitsPerSecond + 500)/1000;
}

RTPSink* LiveAMRSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
	// Create an appropriate audio RTP sink (using "SimpleRTPSink") from the RTP 'groupsock':
	RTPSink* pRtpSink = LiveAMRAudioRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, false, m_numChannels);
	return pRtpSink;
}

} // lme


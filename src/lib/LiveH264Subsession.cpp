#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveH264Subsession.h>
#include <LiveMediaExt/LiveDeviceSource.h>
//#include <LiveMedia/LiveH264RtpSink.h>
#include <LiveMediaExt/LiveH264VideoDeviceSource.h>
#include <H264VideoRTPSink.hh>
#include "Base64.hh"
#include "H264VideoStreamDiscreteFramer.hh"

namespace lme
{

LiveH264Subsession::LiveH264Subsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                                        const unsigned uiChannelId, unsigned uiSourceId, 
                                        const std::string& sSessionName,
                                        const std::string& sSps, const std::string& sPps)
  :LiveMediaSubsession(env, rParent, uiChannelId, uiSourceId, sSessionName, true, 1),
  m_sSps(sSps),
  m_sPps(sPps),
  fAuxSDPLine(NULL),
  fFmtpSDPLine(NULL)
{
  VLOG(2) << "LiveH264Subsession() SPS: " << m_sSps << " PPS: " << m_sPps;
}

LiveH264Subsession::~LiveH264Subsession()
{
  delete[] fAuxSDPLine;
  delete[] fFmtpSDPLine;
}

FramedSource* LiveH264Subsession::createSubsessionSpecificSource(unsigned clientSessionId, IMediaSampleBuffer* pMediaSampleBuffer)
{
  // Create standard device source
  //return LiveDeviceSource::createNew(envir(), clientSessionId, this, pMediaSampleBuffer );
  FramedSource* pLiveDeviceSource = LiveH264VideoDeviceSource::createNew(envir(), clientSessionId, this, pMediaSampleBuffer, m_sSps, m_sPps);

  H264VideoStreamDiscreteFramer* pFramer = H264VideoStreamDiscreteFramer::createNew(envir(), pLiveDeviceSource);
  return pFramer;
}

void LiveH264Subsession::setEstimatedBitRate(unsigned& estBitrate)
{
	// Set estimated session band width
	estBitrate = 1000;
}

RTPSink* LiveH264Subsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
  std::string sPropParameterSets = m_sSps + "," + m_sPps;
  H264VideoRTPSink* pSink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, sPropParameterSets.c_str());
	// TODO: What packet sizes should be set?!?
	pSink->setPacketSizes(1000, 1400);
	return pSink;
}

char const* LiveH264Subsession::getAuxSDPLine( RTPSink* rtpSink, FramedSource* inputSource )
{
  // Trying to use different setup method that live555 H264 file source
  //fAuxSDPLine = strDup(rtpSink->auxSDPLine());
  //return fAuxSDPLine;
  
  //u_int8_t* sps; unsigned spsSize;
  //u_int8_t* pps; unsigned ppsSize;
  //framerSource->getSPSandPPS(sps, spsSize, pps, ppsSize);
  //if (sps == NULL || pps == NULL) return NULL; // our source isn't ready
  const char* sps = m_sSps.c_str();
  unsigned spsSize = m_sSps.length();

  const char* pps = m_sPps.c_str();
  unsigned ppsSize = m_sPps.length();

  u_int32_t profile_level_id;
  if (spsSize < 4) { // sanity check
    profile_level_id = 0;
  } else {
    profile_level_id = (sps[1]<<16)|(sps[2]<<8)|sps[3]; // profile_idc|constraint_setN_flag|level_idc
  }

  // The parameter sets are base64 encoded already
  // Set up the "a=fmtp:" SDP line for this stream:
#if 0
  char* sps_base64 = base64Encode((char*)sps, spsSize);
  char* pps_base64 = base64Encode((char*)pps, ppsSize);

  char const* fmtpFmt =
    "a=fmtp:%d packetization-mode=1"
    ";profile-level-id=%06X"
    ";sprop-parameter-sets=%s,%s\r\n";
  unsigned fmtpFmtSize = strlen(fmtpFmt)
    + 3 /* max char len */
    + 6 /* 3 bytes in hex */
    + strlen(sps_base64) + strlen(pps_base64);
  char* fmtp = new char[fmtpFmtSize];
  sprintf(fmtp, fmtpFmt,
    rtpSink->rtpPayloadType(),
    profile_level_id,
    sps_base64, pps_base64);
  delete[] sps_base64;
  delete[] pps_base64;
#else
  char const* fmtpFmt =
    "a=fmtp:%d packetization-mode=1"
    ";profile-level-id=%06X"
    ";sprop-parameter-sets=%s,%s\r\n";
  unsigned fmtpFmtSize = strlen(fmtpFmt)
    + 3 /* max char len */
    + 6 /* 3 bytes in hex */
    + spsSize + ppsSize;
  char* fmtp = new char[fmtpFmtSize];
  sprintf(fmtp, fmtpFmt,
    rtpSink->rtpPayloadType(),
    profile_level_id,
    sps, pps);
#endif

  delete[] fFmtpSDPLine; fFmtpSDPLine = fmtp;
  return fFmtpSDPLine;
}

} // lme



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
#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveH264Subsession.h>
#include <LiveMediaExt/LiveDeviceSource.h>
#include <LiveMediaExt/LiveH264VideoDeviceSource.h>
#include <H264VideoRTPSink.hh>
#include "Base64.hh"
#include "H264VideoStreamDiscreteFramer.hh"

namespace lme
{

LiveH264Subsession::LiveH264Subsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                                        const unsigned uiChannelId, unsigned uiSourceId, 
                                        const std::string& sSessionName,
                                        const std::string& sSps, const std::string& sPps, 
                                        IRateAdaptationFactory* pFactory,
                                        IRateController* pGlobalRateControl)
  :LiveMediaSubsession(env, rParent, uiChannelId, uiSourceId, sSessionName, true, 1, pFactory, pGlobalRateControl),
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

FramedSource* LiveH264Subsession::createSubsessionSpecificSource(unsigned clientSessionId, 
                                                                 IMediaSampleBuffer* pMediaSampleBuffer, 
                                                                 IRateAdaptationFactory* pRateAdaptationFactory,
                                                                 IRateController* pRateControl)
{
  FramedSource* pLiveDeviceSource = LiveH264VideoDeviceSource::createNew(envir(), clientSessionId, this, m_sSps, m_sPps, 
                                                                         pMediaSampleBuffer, pRateAdaptationFactory, pRateControl);
  // wrap framer around our device source
  H264VideoStreamDiscreteFramer* pFramer = H264VideoStreamDiscreteFramer::createNew(envir(), pLiveDeviceSource);
  return pFramer;
}

void LiveH264Subsession::setEstimatedBitRate(unsigned& estBitrate)
{
	// Set estimated session band width
	estBitrate = 500;
}

RTPSink* LiveH264Subsession::createSubsessionSpecificRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
  // HACKERY
  std::string sPropParameterSets = m_sSps + "," + m_sPps;
  H264VideoRTPSink* pSink = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, sPropParameterSets.c_str());
	pSink->setPacketSizes(1000, 1400);
	return pSink;
}

char const* LiveH264Subsession::getAuxSDPLine( RTPSink* rtpSink, FramedSource* inputSource )
{
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

  delete[] fFmtpSDPLine; fFmtpSDPLine = fmtp;
  return fFmtpSDPLine;
}

} // lme

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
#include <LiveMediaExt/LiveAACSubsession.h>
#include <LiveMediaExt/LiveDeviceSource.h>
#include "MPEG4GenericRTPSink.hh"

namespace lme
{

LiveAACSubsession::LiveAACSubsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                                      const unsigned uiChannelId, unsigned uiSourceID, 
                                      const std::string& sSessionName,
                                      const unsigned uiNumChannels, const unsigned uiBitsPerSample, const unsigned uiSamplingFrequency,
                                      const std::string& sConfigStr,
                                      IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl)
  :LiveMediaSubsession(env, rParent, uiChannelId, uiSourceID, sSessionName, false, 1, pFactory, pGlobalRateControl),
  m_numChannels(uiNumChannels),
  m_bitsPerSample(uiBitsPerSample),
  m_samplingFrequency(uiSamplingFrequency),
  m_bitsPerSecond(m_samplingFrequency * m_bitsPerSample * m_numChannels),
  m_sConfigStr(sConfigStr)
{
#if 0
	env << "Audio source parameters:\n\t" << m_samplingFrequency << " Hz, ";
	env << m_bitsPerSample << " bits-per-sample, ";
	env << m_numChannels << " channels => ";
	env << m_bitsPerSecond << " bits-per-second\n";
#endif
}

LiveAACSubsession::~LiveAACSubsession()
{;}

FramedSource* LiveAACSubsession::createSubsessionSpecificSource(unsigned clientSessionId,
                                                                IMediaSampleBuffer* pMediaSampleBuffer, 
                                                                IRateAdaptationFactory* /*pRateAdaptationFactory*/,
                                                                IRateController* /*pRateControl*/)
{
  // not performing rate adaptation in this module
  return LiveDeviceSource::createNew(envir(), clientSessionId, this, pMediaSampleBuffer, NULL, NULL);
}

void LiveAACSubsession::setEstimatedBitRate( unsigned& estBitrate )
{
	// Set estimated session bitrate
  estBitrate = 96; // kbps, estimate: see ADTSAudioFileServerMediaSubsession.cpp
}

RTPSink* LiveAACSubsession::createSubsessionSpecificRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
	// Create an appropriate audio RTP sink (using "SimpleRTPSink") from the RTP 'groupsock':
  return MPEG4GenericRTPSink::createNew(envir(), rtpGroupsock,
					rtpPayloadTypeIfDynamic,
          m_samplingFrequency,
					"audio", "AAC-hbr", m_sConfigStr.c_str(),
					m_numChannels);
}

} // lme


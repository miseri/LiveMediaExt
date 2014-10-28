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
#include <string>
#include "LiveMediaSubsession.h"

namespace lme
{

// Forward
class IMediaSampleBuffer;
class MediaSubsession;

class LiveAACSubsession : public LiveMediaSubsession
{
public:
  LiveAACSubsession(UsageEnvironment& env, LiveRtspServer& rParent,
    const unsigned uiChannelId, unsigned uiSourceID,
    const std::string& sSessionName,
    const unsigned uiNumChannels, const unsigned uiBitsPerSample, const unsigned uiSamplingFrequency,
    const std::string& sConfigStr,
    IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl);

  virtual ~LiveAACSubsession();

protected:

  /// Overridden from LiveMediaSubsession
  virtual FramedSource* createSubsessionSpecificSource(unsigned clientSessionId,
                                                           IMediaSampleBuffer* pMediaSampleBuffer,
                                                           IRateAdaptationFactory* pRateAdaptationFactory,
                                                           IRateController* pRateControl);
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

  std::string m_sConfigStr;
};

} //lme


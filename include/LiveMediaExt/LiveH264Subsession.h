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

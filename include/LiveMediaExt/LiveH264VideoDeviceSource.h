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
#include "LiveDeviceSource.h"

namespace lme
{

// fwd
class IMediaSampleBuffer;
class LiveMediaSubsession;

class LiveH264VideoDeviceSource : public LiveDeviceSource
{
public:
  virtual ~LiveH264VideoDeviceSource();

  static LiveH264VideoDeviceSource* createNew(UsageEnvironment& env, unsigned uiClientId,
                                              LiveMediaSubsession* pParent, const std::string& sSps, const std::string& sPps, 
                                              IMediaSampleBuffer* pSampleBuffer, 
                                              IRateAdaptationFactory* pRateAdaptationFactory,
                                              IRateController* pRateControl);

  // method to add data to the device
  virtual bool retrieveMediaSampleFromBuffer();
  
protected:
  LiveH264VideoDeviceSource(UsageEnvironment& env, unsigned uiClientId, 
                            LiveMediaSubsession* pParent,
                            const std::string& sSps, const std::string& sPps, 
                            IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory,
                            IRateController* pGlobalRateControl);

protected:

private:
  std::deque<MediaSample> splitPayloadIntoMediaSamples(BYTE* pBuffer, uint32_t uiSize, double dPts);
  void printNalInfo( unsigned char* pNal, unsigned uiSize );

private:
  bool m_bWaitingForIdr;
  Boolean fPictureEndMarker;
};

} // lme


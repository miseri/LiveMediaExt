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
// "liveMedia"
// Copyright (c) 1996-2014 Live Networks, Inc.  All rights reserved.
#pragma once
#include <LiveMediaExt/LiveDeviceSource.h>

namespace lme
{

  // fwd
class IMediaSampleBuffer;
class LiveMediaSubsession;

class LiveAMRAudioDeviceSource : public LiveDeviceSource
{
public:
  /**
   * @brief Destructor
   */
  virtual ~LiveAMRAudioDeviceSource();
  /**
   * @brief Named constructor
   */
  static LiveAMRAudioDeviceSource* createNew(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
                                             IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory,
                                             IRateController* pGlobalRateControl);
  /**
   * @brief Retrieves data from buffer and adds to the device
   */
  virtual bool retrieveMediaSampleFromBuffer();  
  /**
   * @brief Required to be compatible with LiveAMRAudioRTPSink and AMRAudioRTPSink
   */
  virtual Boolean isAMRAudioSource() const;
  /**
   * @brief Getter for if AMR is wide-band
   */
  Boolean isWideband() const { return fIsWideband; }
  /**
   * @brief Getter for number of channels
   */
  unsigned numChannels() const { return fNumChannels; }
  /**
   * @brief Returns the frame header for the most recently read frame (RFC 3267, sec. 5.3)
   */
  u_int8_t lastFrameHeader() const { return fLastFrameHeader; }

protected:
  /**
   * @brief Constructor
   */
  LiveAMRAudioDeviceSource(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
                           IMediaSampleBuffer* pSampleBuffer, 
                           IRateAdaptationFactory* pRateAdaptationFactory,
                           IRateController* pRateControl);
  /**
   * brief redefined virtual function from FramedSource
   */
  virtual void doGetNextFrame();

protected:
  Boolean fIsWideband;
  unsigned fNumChannels;
  u_int8_t fLastFrameHeader;
};

} // lme


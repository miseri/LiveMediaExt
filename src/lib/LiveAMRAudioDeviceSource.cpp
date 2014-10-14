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
#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveAMRAudioDeviceSource.h>
#include <Media/SimpleFrameGrabber.h>

namespace lme
{

LiveAMRAudioDeviceSource::LiveAMRAudioDeviceSource(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
                                                   IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory,
                                                   IRateController* pGlobalRateControl)
  :LiveDeviceSource(env, uiClientId, pParent, new SimpleFrameGrabber(pSampleBuffer), pRateAdaptationFactory, pGlobalRateControl),
  fIsWideband(False),
  fNumChannels(1),
  fLastFrameHeader(0)
{

}

LiveAMRAudioDeviceSource::~LiveAMRAudioDeviceSource()
{

}

LiveAMRAudioDeviceSource* LiveAMRAudioDeviceSource::createNew(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
                                                              IMediaSampleBuffer* pSampleBuffer, 
                                                              IRateAdaptationFactory* pRateAdaptationFactory,
                                                              IRateController* pRateControl)
{
  LiveAMRAudioDeviceSource* pSource = new LiveAMRAudioDeviceSource(env, uiClientId, pParent, pSampleBuffer, pRateAdaptationFactory, pRateControl);
  return pSource;
}

// From liveMedia
#define FT_INVALID 65535
static unsigned short frameSize[16] = {
  12, 13, 15, 17,
  19, 20, 26, 31,
  5, FT_INVALID, FT_INVALID, FT_INVALID,
  FT_INVALID, FT_INVALID, FT_INVALID, 0
};
static unsigned short frameSizeWideband[16] = {
  17, 23, 32, 36,
  40, 46, 50, 58,
  60, 5, FT_INVALID, FT_INVALID,
  FT_INVALID, FT_INVALID, 0, 0
};

// #define DEBUG
bool LiveAMRAudioDeviceSource::retrieveMediaSampleFromBuffer()
{
  unsigned uiSize = 0;
  double dStartTime = 0.0;
  BYTE* pBuffer = m_pFrameGrabber->getNextFrame(uiSize, dStartTime);
#if 1
  fLastFrameHeader = pBuffer[0]; // Should be pos 0 or 1???
  if ((fLastFrameHeader & 0x83) != 0) 
  {
#ifdef DEBUG
    fprintf(stderr, "Invalid frame header 0x%02x (padding bits (0x83) are not zero)\n", fLastFrameHeader);
    LOG(WARNING) << "Invalid frame header " << fLastFrameHeader << " (padding bits(0x83) are not zero)";
#endif
  }
  else 
  {
    unsigned char ft = (fLastFrameHeader & 0x78) >> 3;
    fFrameSize = fIsWideband ? frameSizeWideband[ft] : frameSize[ft];
    if (fFrameSize == FT_INVALID) 
    {
#ifdef DEBUG
      fprintf(stderr, "Invalid FT field %d (from frame header 0x%02x)\n", ft, fLastFrameHeader);
      LOG(WARNING) << "Invalid FT field  " << ft << " (from frame header " << fLastFrameHeader;
#endif
    }
    else 
    {
      // The frame header is OK
#ifdef DEBUG
      fprintf(stderr, "Valid frame header 0x%02x -> ft %d -> frame size %d\n", fLastFrameHeader, ft, fFrameSize);
      VLOG(2) << "Valid frame header " << fLastFrameHeader << " -> ft " << ft << " -> frame size " << fFrameSize;
#endif
    }
  }
#endif

  // Make sure there's data, the frame grabber should return null if it doesn't have any
  if (!pBuffer)
  {
#if 0
    VLOG(10) << "NULL Sample retrieved from frame grabber";
#endif
    return false;
  }
  else
  {
    // Without TOC!!!!!!
    MediaSample mediaSample;
    mediaSample.setPresentationTime(dStartTime);
    BYTE* pData = new uint8_t[uiSize - 1];
    memcpy(pData, pBuffer + 1, uiSize - 1);
    mediaSample.setData(Buffer(pData, uiSize - 1));
    m_qMediaSamples.push_back(mediaSample);
    return true;
  }
}

void LiveAMRAudioDeviceSource::doGetNextFrame()
{
  LiveDeviceSource::doGetNextFrame();
}

Boolean LiveAMRAudioDeviceSource::isAMRAudioSource() const
{
  return True;
}

} // lme

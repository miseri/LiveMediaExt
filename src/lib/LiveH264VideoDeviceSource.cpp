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
#include <LiveMediaExt/LiveH264VideoDeviceSource.h>
#include <Media/SimpleFrameGrabber.h>

namespace lme
{

LiveH264VideoDeviceSource::LiveH264VideoDeviceSource( UsageEnvironment& env, unsigned uiClientId,  
                                                      LiveMediaSubsession* pParent, 
                                                      const std::string& sSps, const std::string& sPps,
                                                      IMediaSampleBuffer* pSampleBuffer, 
                                                      IRateAdaptationFactory* pRateAdaptationFactory,
                                                      IRateController* pGlobalRateControl)
  :LiveDeviceSource(env, uiClientId, pParent, new SimpleFrameGrabber(pSampleBuffer), pRateAdaptationFactory, pGlobalRateControl),
  m_bWaitingForIdr(true)
{

}

LiveH264VideoDeviceSource::~LiveH264VideoDeviceSource()
{

}

LiveH264VideoDeviceSource* LiveH264VideoDeviceSource::createNew(UsageEnvironment& env, unsigned uiClientId,
                                                                 LiveMediaSubsession* pParent, 
                                                                 const std::string& sSps, const std::string& sPps,
                                                                 IMediaSampleBuffer* pSampleBuffer, 
                                                                 IRateAdaptationFactory* pRateAdaptationFactory,
                                                                 IRateController* pGlobalRateControl)
{
  // When constructing a 'simple' LiveDeviceSource we'll just create a simple frame grabber
  LiveH264VideoDeviceSource* pSource = new LiveH264VideoDeviceSource(env, uiClientId, pParent, 
                                                                     sSps, sPps, 
                                                                     pSampleBuffer, pRateAdaptationFactory, pGlobalRateControl);
  return pSource;
}

std::deque<MediaSample> LiveH264VideoDeviceSource::splitPayloadIntoMediaSamples(BYTE* pBuffer, uint32_t uiSize, double dPts)
{
  std::deque<MediaSample> mediaSamples;
  if (!pBuffer || uiSize == 0) return mediaSamples;

  // check for picture start codes
  unsigned char startCode[4] = { 0, 0, 0, 1 };
  // check for first start code
  if (memcmp(startCode, pBuffer, 4) == 0)
  {
    // attempt 1: search for start codes and create samples
    unsigned uiStartingPos = 4;
    for (size_t i = uiStartingPos; i < uiSize - 4; ++i)
    {
      if (memcmp(startCode, pBuffer + i, 4) == 0)
      {
        unsigned uiNalUnitSize = i - uiStartingPos;
        printNalInfo(pBuffer + uiStartingPos, uiNalUnitSize);

        VLOG(15) << "Found start code at pos " << i << " - storing NAL starting at: " << uiStartingPos << " Size: " << uiNalUnitSize;
        // found next starting pos, push previous NAL
        MediaSample mediaSample;
        mediaSample.setPresentationTime(dPts);
        BYTE* pData = new uint8_t[uiNalUnitSize];
        memcpy(pData, pBuffer + uiStartingPos, uiNalUnitSize);
        Buffer dataBuffer(pData, uiNalUnitSize);
        mediaSample.setData(dataBuffer);
        mediaSamples.push_back(mediaSample);
        uiStartingPos = i + 4;
        i = i + 4;
      }
    }
    // Push last remaining sample
    unsigned uiNalUnitSize = uiSize - uiStartingPos;
    printNalInfo(pBuffer + uiStartingPos, uiNalUnitSize);

    VLOG(15) << "Pushing final NAL - storing NAL starting at: " << uiStartingPos << " Size: " << uiNalUnitSize;
    MediaSample mediaSample;
    mediaSample.setPresentationTime(dPts);
    BYTE* pData = new uint8_t[uiNalUnitSize];
    memcpy(pData, pBuffer + uiStartingPos, uiNalUnitSize);
    Buffer dataBuffer(pData, uiNalUnitSize);
    mediaSample.setData(dataBuffer);
    mediaSamples.push_back(mediaSample);
    VLOG(15) << "Multiple NAL units found: " << mediaSamples.size() << " Sample size : " << uiSize;
  }
  return mediaSamples;
}

bool isIdrFrame(unsigned char nalUnitHeader)
{
  unsigned uiForbiddenZeroBit = nalUnitHeader & 0x80;
  //assert(uiForbiddenZeroBit == 0);
  unsigned uiNalRefIdc = nalUnitHeader & 0x60;
  unsigned char uiNalUnitType = nalUnitHeader & 0x1f;
  switch (uiNalUnitType)
  {
    // IDR nal unit types
  case 5:
    return true;
  default:
    return false;
  }
}
bool isSps(unsigned char nalUnitHeader)
{
  unsigned uiForbiddenZeroBit = nalUnitHeader & 0x80;
  unsigned uiNalRefIdc = nalUnitHeader & 0x60;
  unsigned char uiNalUnitType = nalUnitHeader & 0x1f;
  switch (uiNalUnitType)
  {
    // IDR nal unit types
  case 7:
    return true;
  default:
    return false;
  }
}
bool isPps(unsigned char nalUnitHeader)
{
  unsigned uiForbiddenZeroBit = nalUnitHeader & 0x80;
  unsigned uiNalRefIdc = nalUnitHeader & 0x60;
  unsigned char uiNalUnitType = nalUnitHeader & 0x1f;
  switch (uiNalUnitType)
  {
    // IDR nal unit types
  case 8:
    return true;
  default:
    return false;
  }
}

bool LiveH264VideoDeviceSource::retrieveMediaSampleFromBuffer()
{
  VLOG(15) << "LiveH264VideoDeviceSource::retrieveMediaSampleFromBuffer";
  unsigned uiSize = 0;
  double dStartTime = 0.0;
  BYTE* pBuffer = m_pFrameGrabber->getNextFrame(uiSize, dStartTime);
  VLOG(10) << "Retrieved sample from buffer: " << uiSize << " PTS: " << dStartTime;
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
    if (m_bWaitingForIdr)
    {
      std::deque<MediaSample> mediaSamples = splitPayloadIntoMediaSamples(pBuffer, uiSize, dStartTime);

      for (std::deque<MediaSample>::iterator it = mediaSamples.begin(); it != mediaSamples.end(); ++it)
      {
        const uint8_t* pPayload = it->getDataBuffer().data();
        if (isSps(pPayload[0]))
        {
          // Found SPS
          VLOG(2) << "Found SPS NAL unit";
        }
        if (isPps(pPayload[0]))
        {
          // Found SPS
          VLOG(2) << "Found PPS NAL unit";
        }
        if (isIdrFrame(pPayload[0]))
        {
          // Found IDR
          VLOG(2) << "Found IDR frame";
          m_bWaitingForIdr = false;
        }
      }
      if (!m_bWaitingForIdr)
      {
        //for (const MediaSample& mediaSample : mediaSamples)
        //{
        //  m_qMediaSamples.push_back(mediaSample);
        //}
        m_qMediaSamples.insert(m_qMediaSamples.end(), mediaSamples.begin(), mediaSamples.end());
        return true;
      }
      VLOG(2) << "Waiting for IDR frame";
      return false;
    }
    else
    {
      std::deque<MediaSample> mediaSamples = splitPayloadIntoMediaSamples(pBuffer, uiSize, dStartTime);
      //for (const MediaSample& mediaSample : mediaSamples)
      //{
      //  m_qMediaSamples.push_back(mediaSample);
      //}
      m_qMediaSamples.insert(m_qMediaSamples.end(), mediaSamples.begin(), mediaSamples.end());
      return true;
    }
  }
}

void LiveH264VideoDeviceSource::printNalInfo( unsigned char* pNal, unsigned uiSize )
{  
  VLOG(10) << "LiveH264VideoDeviceSource::printNalInfo";
  // Get the "nal_unit_type", to see if this NAL unit is one that we want to save a copy of:
  u_int8_t nal_unit_type = pNal[0]&0x1F;
  Boolean const isVCL = nal_unit_type <= 5 && nal_unit_type > 0; // Would need to include type 20 for SVC and MVC #####

  if (nal_unit_type == 7) 
  { // Sequence parameter set (SPS)
    VLOG(10) << "NAL: SPS (Size: " << uiSize << " bytes)";
  } else if (nal_unit_type == 8) 
  { // Picture parameter set (PPS)
    VLOG(10) << "NAL: PPS  (Size: " << uiSize << " bytes)";
  }
  else
  {
    VLOG(10) << "NAL (type: " << (int)nal_unit_type << ", is VCL: " << isVCL << " Size: " << uiSize << ") ";
  }
}

} // lme



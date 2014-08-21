#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveH264VideoDeviceSource.h>
#include <Media/SimpleFrameGrabber.h>

namespace lme
{

LiveH264VideoDeviceSource::LiveH264VideoDeviceSource( UsageEnvironment& env, unsigned uiClientId,  
                                                      LiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer,
                                                      const std::string& sSps, const std::string& sPps )
  :LiveDeviceSource(env, uiClientId, pParent, new SimpleFrameGrabber(pSampleBuffer)),
  fLastSeenSPS(NULL), fLastSeenSPSSize(0), fLastSeenPPS(NULL), fLastSeenPPSSize(0)
{
  //LOG_CONSTRUCTION
}

LiveH264VideoDeviceSource::~LiveH264VideoDeviceSource()
{
//#ifdef RTVC_LOG_CONSTRUCTION
//  LOG_DESTRUCTION
//#endif
}

LiveH264VideoDeviceSource* LiveH264VideoDeviceSource::createNew(UsageEnvironment& env, unsigned uiClientId,
                                                                 LiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer, 
                                                                 const std::string& sSps, const std::string& sPps )
{
  // When constructing a 'simple' LiveDeviceSource we'll just create a simple frame grabber
  LiveH264VideoDeviceSource* pSource = new LiveH264VideoDeviceSource(env, uiClientId, pParent, pSampleBuffer, sSps, sPps);
  return pSource;
}

bool LiveH264VideoDeviceSource::retrieveMediaSampleFromBuffer()
{
  unsigned uiSize = 0;
  double dStartTime = 0.0;
  BYTE* pBuffer = m_pFrameGrabber->getNextFrame(uiSize, dStartTime);
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
    // check for picture start codes
    unsigned char startCode[4] = { 0, 0, 0, 1 };
    // check for first start code
    if (memcmp(startCode, pBuffer, 4) == 0)
    {
      // attempt 1: search for start codes and create samples
      unsigned uiStartingPos = 4;
      for (int i = uiStartingPos; i < uiSize - 4; ++i)
      {
        if (memcmp(startCode, pBuffer + i, 4) == 0)
        {
          unsigned uiNalUnitSize = i - uiStartingPos;
          printNalInfo(pBuffer + uiStartingPos, uiNalUnitSize);

          VLOG(5) << "Found start code at pos " << i << " - storing NAL starting at: " << uiStartingPos << " Size: " << i - uiStartingPos;
          // found next starting pos, push previous NAL
          MediaSample mediaSample;
          mediaSample.setPresentationTime(dStartTime);
          BYTE* pData = new uint8_t[uiNalUnitSize];
          memcpy(pData, pBuffer + uiStartingPos, uiNalUnitSize);
          mediaSample.setData(Buffer(pData, uiNalUnitSize));
          m_qMediaSamples.push(mediaSample);
          uiStartingPos = i + 4;
        }
      }
      // Push last remaining sample
      unsigned uiNalUnitSize = uiSize - uiStartingPos;
      printNalInfo(pBuffer + uiStartingPos, uiNalUnitSize);
      
      VLOG(5) << "Pushing final NAL - storing NAL starting at: " << uiStartingPos << " Size: " << uiSize - uiStartingPos;
      MediaSample mediaSample;
      mediaSample.setPresentationTime(dStartTime);
      BYTE* pData = new uint8_t[uiNalUnitSize];
      memcpy(pData, pBuffer + uiStartingPos, uiNalUnitSize);
      mediaSample.setData(Buffer(pData, uiNalUnitSize));
      m_qMediaSamples.push(mediaSample);

      VLOG(5) << "Multiple NAL units found: Sample size: " << uiSize;

      //std::vector<unsigned> vStartCodePositions;
      // attempt 2: strip start codes out of stream
#if 0
      VLOG(2) << "Start code found in NAL";
      // found start codes
      // DO NOT save start code in media sample
      MediaSampleEx2* pSample = new MediaSampleEx2(pBuffer + 4, uiSize - 4, dStartTime);
      assert(pSample);
      m_qMediaSamples.push(pSample);
#endif
      return true;
    }
    else
    {
      LOG(INFO) << "No start code found in NAL";
      // TODO: some validation???
      MediaSample mediaSample;
      mediaSample.setPresentationTime(dStartTime);
      BYTE* pData = new uint8_t[uiSize];
      memcpy(pData, pBuffer, uiSize);
      mediaSample.setData(Buffer(pData, uiSize));
      m_qMediaSamples.push(mediaSample);

      return true;
    }
  }
}

void LiveH264VideoDeviceSource::afterGettingFrame(void* clientData, unsigned frameSize,
                                                  unsigned numTruncatedBytes,
                                                  struct timeval presentationTime,
                                                  unsigned durationInMicroseconds) 
{
  LiveH264VideoDeviceSource* source = (LiveH264VideoDeviceSource*)clientData;
  source->afterGettingFrame1(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void LiveH264VideoDeviceSource::afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
                                                   struct timeval presentationTime,
                                                   unsigned durationInMicroseconds) 
{
  // Get the "nal_unit_type", to see if this NAL unit is one that we want to save a copy of:
  u_int8_t nal_unit_type = frameSize == 0 ? 0xFF : fTo[0]&0x1F;

  // Check for a (likely) common error: NAL units that (erroneously) begin with a 0x00000001 or 0x000001 'start code'
  //     (Those start codes should only be in byte-stream data; not data that consists of discrete NAL units.)
  if (nal_unit_type == 0) {
    VLOG(2) << "Warning: Invalid 'nal_unit_type': 0.  Does the NAL unit begin with a MPEG 'start code' by mistake?";
    envir() << "Warning: Invalid 'nal_unit_type': 0.  Does the NAL unit begin with a MPEG 'start code' by mistake?\n";
  } else if (nal_unit_type == 7) { // Sequence parameter set (SPS)
    VLOG(2) << "Found SPS";
    saveCopyOfSPS(fTo, frameSize);
  } else if (nal_unit_type == 8) { // Picture parameter set (PPS)
    VLOG(2) << "Found PPS";
    saveCopyOfPPS(fTo, frameSize);
  }

  // Next, check whether this NAL unit ends the current 'access unit' (basically, a video frame).  Unfortunately, we can't do this
  // reliably, because we don't yet know anything about the *next* NAL unit that we'll see.  So, we guess this as best as we can,
  // by assuming that if this NAL unit is a VCL NAL unit, then it ends the current 'access unit'.
  Boolean const isVCL = nal_unit_type <= 5 && nal_unit_type > 0; // Would need to include type 20 for SVC and MVC #####
  if (isVCL)
  {
    fPictureEndMarker = True;
    VLOG(2) << "Setting picture end marker";
  }
  // Finally, complete delivery to the client:
  fFrameSize = frameSize;
  fNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = durationInMicroseconds;
  afterGetting(this);
}

void LiveH264VideoDeviceSource::printNalInfo( unsigned char* pNal, unsigned uiSize )
{  
  // Get the "nal_unit_type", to see if this NAL unit is one that we want to save a copy of:
  u_int8_t nal_unit_type = pNal[0]&0x1F;
  Boolean const isVCL = nal_unit_type <= 5 && nal_unit_type > 0; // Would need to include type 20 for SVC and MVC #####

  if (nal_unit_type == 7) 
  { // Sequence parameter set (SPS)
    VLOG(5) << "NAL: SPS (Size: " << uiSize << " bytes)";
  } else if (nal_unit_type == 8) 
  { // Picture parameter set (PPS)
    VLOG(5) << "NAL: PPS  (Size: " << uiSize << " bytes)";
  }
  else
  {
    VLOG(5) << "NAL (type: " << (int)nal_unit_type << ", is VCL: " << isVCL << " Size: " << uiSize << ") ";
  }
}

} // lme



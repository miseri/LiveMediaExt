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
                                              LiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer,
                                              const std::string& sSps, const std::string& sPps);

#if 0
  void getSPSandPPS(u_int8_t*& sps, unsigned& spsSize, u_int8_t*& pps, unsigned& ppsSize) const{
    // Returns pointers to copies of the most recently seen SPS (sequence parameter set) and PPS (picture parameter set) NAL unit.
    // (NULL pointers are returned if the NAL units have not yet been seen.)
    sps = fLastSeenSPS; spsSize = fLastSeenSPSSize;
    pps = fLastSeenPPS; ppsSize = fLastSeenPPSSize;
  }
#endif

  // method to add data to the device
  virtual bool retrieveMediaSampleFromBuffer();
  
  // redefined virtual functions:
  virtual Boolean isH264VideoStreamFramer() const
  {
    return True;
  }

  Boolean& pictureEndMarker() { return fPictureEndMarker; }
      // a hack for implementing the RTP 'M' bit
protected:
  LiveH264VideoDeviceSource(UsageEnvironment& env, unsigned uiClientId, 
                            LiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer,
                            const std::string& sSps, const std::string& sPps);

protected:
  /*
  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
                                struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame1(unsigned frameSize,
                          unsigned numTruncatedBytes,
                          struct timeval presentationTime,
                          unsigned durationInMicroseconds);
  */
#if 0
  void saveCopyOfSPS(u_int8_t* from, unsigned size)
  {
    delete[] fLastSeenSPS;
    fLastSeenSPS = new u_int8_t[size];
    memmove(fLastSeenSPS, from, size);

    fLastSeenSPSSize = size;
  }
  void saveCopyOfPPS(u_int8_t* from, unsigned size)
  {
    delete[] fLastSeenPPS;
    fLastSeenPPS = new u_int8_t[size];
    memmove(fLastSeenPPS, from, size);

    fLastSeenPPSSize = size;
  }
#endif

private:
  void printNalInfo( unsigned char* pNal, unsigned uiSize );
  //void setPresentationTime() { fPresentationTime = fNextPresentationTime; }

private:

#if 0
  //Boolean fIncludeStartCodeInOutput;
  u_int8_t* fLastSeenSPS;
  unsigned fLastSeenSPSSize;
  u_int8_t* fLastSeenPPS;
  unsigned fLastSeenPPSSize;
  //struct timeval fNextPresentationTime; // the presentation time to be used for the next NAL unit to be parsed/delivered after this
  //friend class H264VideoStreamParser; // hack
#endif

  Boolean fPictureEndMarker;
};

} // lme


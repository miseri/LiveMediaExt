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
  
  //// redefined virtual functions:
  //virtual Boolean isH264VideoStreamFramer() const
  //{
  //  return True;
  //}

  //Boolean& pictureEndMarker() { return fPictureEndMarker; }
      // a hack for implementing the RTP 'M' bit
protected:
  LiveH264VideoDeviceSource(UsageEnvironment& env, unsigned uiClientId, 
                            LiveMediaSubsession* pParent,
                            const std::string& sSps, const std::string& sPps, 
                            IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory,
                            IRateController* pGlobalRateControl);

protected:

private:
  std::deque<MediaSample> splitPayloadIntoMediaSamples(unsigned char* pBuffer, uint32_t uiSize, double dPts);
  void printNalInfo( unsigned char* pNal, unsigned uiSize );

private:
  bool m_bWaitingForIdr;
  Boolean fPictureEndMarker;
};

} // lme


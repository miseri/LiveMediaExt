#pragma once
#include <Media/MediaSample.h>

#ifndef BYTE
	typedef unsigned char BYTE;
#endif

namespace lme
{

/// This class abstracts the multiple buffers that a (multiplexed) sample gets copied into
/// The interface tries to cater for both media samples where there is only one channel such as PCM or H263
/// As well as our custom multiplexed H263 data type
class IMediaSampleBuffer
{
public:
  IMediaSampleBuffer()
  {;}

  virtual ~IMediaSampleBuffer()
  {;}

#pragma chMSG(To be refactored)
  virtual void addMediaSample(const MediaSample& mediaSample) = 0;
  virtual unsigned getCurrentChannel()                        = 0;
  virtual void setCurrentChannel(unsigned uiChannel)          = 0;
  virtual BYTE* getCurrentBuffer()                            = 0;
  virtual double getCurrentStartTime()                        = 0;
  virtual unsigned getCurrentSize()                           = 0;
  virtual unsigned getNumberOfChannels()                      = 0;
  virtual BYTE* getBufferAt(unsigned nIndex)                  = 0;
};

} // lme




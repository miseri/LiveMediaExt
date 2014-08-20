#pragma once
#include <Media/MediaSampleStorageBuffer.h>

namespace lme
{

/// This class abstracts the multiple buffers that a sample gets copied into
class SingleMediaSampleBuffer : public IMediaSampleBuffer
{
public:
  SingleMediaSampleBuffer(unsigned uiMaxFrameSize = 10000) //TOREVISE: what default size?
    :m_uiCurrentBufferSize(uiMaxFrameSize),
    m_pBuffer(NULL),
    m_uiSize(0),
    m_uiCurrentChannel(0),
    m_dStartTime(0.0)
  {
    m_pBuffer = new BYTE[uiMaxFrameSize];
  }

  virtual ~SingleMediaSampleBuffer()
  {
    delete[] m_pBuffer;
  }

  virtual unsigned getCurrentChannel() { return 0; }
  virtual void setCurrentChannel(unsigned uiChannel){ return; } // only has one channel

  /// This method should copy the media sample in the buffers according to the media type
  virtual void addMediaSample(const MediaSample& mediaSample)
  {
    // TODO: how was remaining used?!?
    // unsigned uiRemaining = pDataBuffer->remaining();
    // BYTE* pMediaData = const_cast<BYTE*>(pDataBuffer->read(uiRemaining));
    const uint8_t* pMediaData = mediaSample.getDataBuffer().data();
    unsigned uiRemaining = mediaSample.getMediaSize();

    m_dStartTime = mediaSample.getPresentationTime();
    // Grow buffer if too small
    m_uiSize = uiRemaining;
    if (m_uiSize > m_uiCurrentBufferSize)
    {
      // Increase the current buffer size
      if (m_pBuffer)
      {
        delete[] m_pBuffer;
      }
      m_pBuffer = new BYTE[m_uiSize];
      m_uiCurrentBufferSize = m_uiSize;
    }
    memcpy(m_pBuffer, pMediaData, m_uiSize);
  }

  virtual unsigned getNumberOfChannels() { return 1; }
  virtual unsigned getCurrentSize() { return m_uiSize; }
  virtual double getCurrentStartTime() { return m_dStartTime; }
  virtual BYTE* getCurrentBuffer() { return m_pBuffer; }
  virtual BYTE* getBufferAt(unsigned uiIndex)
  {
    if (uiIndex == 0)
    {
      return m_pBuffer;
    }
    return NULL;
  }

private:

  unsigned m_uiCurrentBufferSize;
  BYTE* m_pBuffer;
  unsigned m_uiSize;
  unsigned m_uiCurrentChannel;
  double m_dStartTime;
};

} // lme





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
#include <map>
#include <Media/MediaSampleStorageBuffer.h>
#include <Media/MultiplexedMediaHeader.h>

namespace lme
{

/**
 * @brief Buffer class for multiplexed media samples
 *
 * Media samples must contain the MultiplexedMediaHeader format
 */
class MultiMediaSampleBuffer : public IMediaSampleBuffer
{
public:
  MultiMediaSampleBuffer(unsigned uiChannels, unsigned uiMaxFrameSize = 10000) // TOREVISE: what default size?
    :m_uiChannels(uiChannels),
    m_uiMaxFrameSize(uiMaxFrameSize),
    m_uiCurrentChannel(0),
    m_dStartTime(-1.0)
  {
    // Create the static media sample buffers at this point
    for (size_t i = 0; i < m_uiChannels; i++)
    {
      BYTE* pBuffer = new BYTE[m_uiMaxFrameSize];
      memset(pBuffer, 0, m_uiMaxFrameSize);
      m_vSampleBuffers.push_back(pBuffer);
      m_vSizes.push_back(0);
      // Store current buffer size for this channel since it might need to get bigger
      m_mBufferSizes[i] = m_uiMaxFrameSize;
    }
  }

  virtual ~MultiMediaSampleBuffer()
  {
    for (size_t i = 0; i < m_uiChannels; i++)
    {
      delete[] m_vSampleBuffers[i];
    }
    m_vSampleBuffers.clear();
    m_vSizes.clear();
  }

  virtual unsigned getCurrentChannel() { return m_uiCurrentChannel; }

  virtual void setCurrentChannel(unsigned uiChannel)
  {
    if (uiChannel < m_uiChannels)
    {
      m_uiCurrentChannel = uiChannel;
    }
  }

  /// This method should copy the media sample in the buffers according to the media type
  virtual void addMediaSample(const MediaSample& mediaSample)
  {
    // Get start time
    m_dStartTime = mediaSample.getPresentationTime();

    // Get raw data stream
    // BYTE* pRawData = const_cast<BYTE*>(pMediaSampleEx->getMediaData()->getData());
    const uint8_t* pMediaData = mediaSample.getDataBuffer().data();

    boost::optional<MultiplexedMediaHeader> pMediaHeader = MultiplexedMediaHeader::read(pMediaData, mediaSample.getDataBuffer().getSize());

    if (!pMediaHeader)
    {
      LOG(ERROR) << "Invalid multiplexed media header received";
      return;
    }

    // Read media stream information
    int nNumberOfStreams = pMediaHeader->getStreamCount();
    if (nNumberOfStreams != m_uiChannels)
    {
      LOG(ERROR) << "Unexpected error: Number of Streams: " << nNumberOfStreams << " Number of Channels: " << m_uiChannels;
      return;
    }

    // For now let's just extract the first stream and pass this back
    const uint8_t* pSource = pMediaData + pMediaHeader->getHeaderLength();

    for (size_t i = 0; i < m_vSampleBuffers.size(); i++)
    {
      BYTE* pDestination = m_vSampleBuffers[i];
      unsigned uiLengthOfStream = pMediaHeader->getStreamLength(i);
      unsigned uiBufferSize = m_mBufferSizes[i];
      while (uiLengthOfStream > m_mBufferSizes[i])
      {
        // Double the buffer size
        m_mBufferSizes[i] <<= 1;
        LOG(WARNING) << "Framesize was too large : Size: " << uiLengthOfStream << " Prev Max: " << (m_mBufferSizes[i]/2) << " New Max: " << m_mBufferSizes[i];
      }	
      if (uiBufferSize != m_mBufferSizes[i])
      {
        // free old memory
        delete[] pDestination;
        pDestination = new BYTE[m_mBufferSizes[i]];
        m_vSampleBuffers[i] = pDestination;
      }

      assert(uiLengthOfStream <= m_mBufferSizes[i]);
      // Copy data			
      memcpy(pDestination, pSource, uiLengthOfStream);
      m_vSizes[i] = uiLengthOfStream;

      // Point to next media sample
      pSource += uiLengthOfStream;
    }
  }

  virtual BYTE* getCurrentBuffer()
  {
    if (m_uiCurrentChannel < m_vSampleBuffers.size())
    {
      return m_vSampleBuffers[m_uiCurrentChannel];
    }
    return NULL;
  }

  virtual BYTE* getBufferAt(unsigned uiIndex)
  {
    if (uiIndex < m_vSampleBuffers.size()) 
      return m_vSampleBuffers[uiIndex];
    return NULL;
  }

  virtual unsigned getNumberOfChannels(){return m_uiChannels;}
  virtual unsigned getCurrentSize()
  {
    if (m_uiCurrentChannel < m_vSampleBuffers.size())
    {
      return m_vSizes[m_uiCurrentChannel];
    }
    return 0;
  }
  virtual double getCurrentStartTime(){ return m_dStartTime; }

private:

  std::vector<BYTE*> m_vSampleBuffers;
  std::vector<int> m_vSizes;
  unsigned m_uiChannels;
  unsigned m_uiMaxFrameSize;
  std::map<unsigned, unsigned> m_mBufferSizes;

  unsigned m_uiCurrentChannel;
  double m_dStartTime;
};

} //lme

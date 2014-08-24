#include "LiveMediaExtPch.h"
#include <Media/VirtualMediaSource.h>
#include <cassert>
#include <boost/bind.hpp>

namespace lme
{

VirtualMediaSource::VirtualMediaSource(boost::asio::io_service& ioService, ReceiveMediaCb_t mediaCallback, 
                                       uint32_t uiIntervalMs, uint32_t uiSampleSize)
  :m_timer(ioService, boost::posix_time::milliseconds(uiIntervalMs)),
  m_mediaCallback(mediaCallback),
  m_uiIntervalMs(uiIntervalMs),
  m_uiMediaSampleSize(uiSampleSize),
  m_dLastTimestamp(0.0)
{
  assert(m_mediaCallback);
  assert(m_uiMediaSampleSize > 0);
}

boost::system::error_code VirtualMediaSource::start()
{
  boost::system::error_code ec;
  generateMediaSampleTimeout(ec);
  return boost::system::error_code();
}

boost::system::error_code VirtualMediaSource::stop()
{
  m_timer.cancel();
  return boost::system::error_code();
}

void VirtualMediaSource::generateMediaSampleTimeout(const boost::system::error_code& ec)
{
  if (!ec)
  {
    VLOG(10) << "Generating media sample with PTS " << m_dLastTimestamp;
    std::vector<MediaSample> mediaSamples;
    Buffer buffer(new uint8_t[m_uiMediaSampleSize], m_uiMediaSampleSize);
    char* pBuffer = reinterpret_cast<char*>(const_cast<uint8_t*>(buffer.data()));

    // HACK for now: H264
    pBuffer[0] = 0;
    pBuffer[1] = 0;
    pBuffer[2] = 0;
    pBuffer[3] = 1;
    pBuffer[4] = 65;
    for (size_t j = 5; j < m_uiMediaSampleSize; ++j)
        pBuffer[j] = rand() % 256;


    MediaSample mediaSample;
    mediaSample.setPresentationTime(m_dLastTimestamp);
    mediaSample.setData(buffer);
    mediaSamples.push_back(mediaSample);
    m_mediaCallback(mediaSamples);

    m_dLastTimestamp += (m_uiIntervalMs / 1000.0);
    m_timer.expires_from_now(boost::posix_time::milliseconds(m_uiIntervalMs));
    m_timer.async_wait(boost::bind(&VirtualMediaSource::generateMediaSampleTimeout, this, _1));

  }
  else
  {
    if (ec != boost::system::errc::operation_canceled)
    {
      LOG(WARNING) << "Error: " << ec.message();
    }
  }
}

} // lme

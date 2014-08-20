#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include <Media/MediaChannel.h>

namespace lme
{

/**
 * @brief Implementation of packet manager-based media channel
 *
 * This implementation will test using the lock-free queue implementations
 * of the boost libraries.
 */
class PacketManagerMediaChannel : public MediaChannel
{
public:
  /**
   * @brief Constructor
   */
  PacketManagerMediaChannel(uint32_t uiChannelId)
    :MediaChannel(uiChannelId)
  {

  }
  /**
   * @brief The subclass must implement delivery of video media samples to the media sink
   */
  virtual boost::system::error_code deliverVideo(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples)
  {
    std::vector<MediaSample>::const_iterator it = m_videoSamples.push(mediaSamples.begin(), mediaSamples.end());
    assert(it == mediaSamples.end());
    return boost::system::error_code();
  }

  /**
  * @brief The subclass must implement delivery of audio media samples to the media sink
  */
  virtual boost::system::error_code deliverAudio(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples)
  {
    std::vector<MediaSample>::const_iterator it = m_audioSamples.push(mediaSamples.begin(), mediaSamples.end());
    assert(it == mediaSamples.end());
    return boost::system::error_code();
  }
  /**
   * @brief This method returns a video media sample if available, and a null pointer otherwise
   */
  boost::optional<MediaSample> getVideo()
  {
    MediaSample mediaSample;
    if (m_videoSamples.pop(mediaSample))
    {
      return boost::optional<MediaSample>(mediaSample);
    }
    else
    {
      // empty
      return boost::optional<MediaSample>();
    }
  }
  /**
   * @brief This method returns a video media sample if available, and a null pointer otherwise
   */
  boost::optional<MediaSample> getAudio()
  {
    MediaSample mediaSample;
    if (m_audioSamples.pop(mediaSample))
    {
      return boost::optional<MediaSample>(mediaSample);
    }
    else
    {
      // empty
      return boost::optional<MediaSample>();
    }
  }

private:
  /// lock free queue to store video media samples in
  boost::lockfree::spsc_queue<MediaSample, boost::lockfree::capacity<1024> > m_videoSamples;
  /// lock free queue to store audio media samples in
  boost::lockfree::spsc_queue<MediaSample, boost::lockfree::capacity<1024> > m_audioSamples;
};

} //lme


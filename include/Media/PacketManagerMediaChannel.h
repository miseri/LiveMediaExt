#pragma once
//#define LOCK_FREE
#ifdef LOCK_FREE
#include <boost/lockfree/spsc_queue.hpp>
#else
#include <deque>
#include <boost/thread/mutex.hpp>
#endif
#include <boost/optional.hpp>
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
  PacketManagerMediaChannel(uint32_t uiChannelId);
  /**
   * @brief The subclass must implement delivery of video media samples to the media sink
   */
  virtual boost::system::error_code deliverVideo(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples);
  /**
   * @brief The subclass must implement delivery of audio media samples to the media sink
   */
  virtual boost::system::error_code deliverAudio(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples);
  /**
   * @brief This method returns a video media sample if available, and a null pointer otherwise
   */
  boost::optional<MediaSample> getVideo();
  /**
   * @brief This method returns a video media sample if available, and a null pointer otherwise
   */
  boost::optional<MediaSample> getAudio();

private:
#ifdef LOCK_FREE
#define LF_CAPACITY 10240
  /// lock free queue to store video media samples in
  boost::lockfree::spsc_queue<MediaSample, boost::lockfree::capacity<LF_CAPACITY> > m_videoSamples;
  /// lock free queue to store audio media samples in
  boost::lockfree::spsc_queue<MediaSample, boost::lockfree::capacity<LF_CAPACITY> > m_audioSamples;
#else
  std::deque<MediaSample> m_videoSamples;
  std::deque<MediaSample> m_audioSamples;
  boost::mutex m_videoLock;
  boost::mutex m_audioLock;
#endif
};

} //lme


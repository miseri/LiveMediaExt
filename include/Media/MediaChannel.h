#pragma once
#include <cstdint>
#include <vector>
#include <boost/system/error_code.hpp>
#include <Media/MediaSample.h>

namespace lme
{

/**
 * @brief A MediaChannel is comprised of an audio and a video channel.
 * The MediaChannel abstracts the delivery of media from a source to some
 * media sink.
 *
 * A MediaChannel is identified by the channel ID. 
 */
class MediaChannel
{
public:
  /**
   * @brief Constructor.
   */
  MediaChannel(uint32_t uiChannelId)
    :m_uiChannelId(uiChannelId)
  {

  }
  /**
   * @brief Destructor.
   */
  virtual ~MediaChannel()
  {

  }
  /**
   * @brief The addVideoMediaSamples() can be called to deliver media samples to 
   * the media sink.
   */
  boost::system::error_code addVideoMediaSamples(const std::vector<MediaSample>& mediaSamples)
  {
    return deliverVideo(m_uiChannelId, mediaSamples);
  }
  /**
   * @brief The addAudioMediaSamples() can be called to deliver media samples to 
   * the media sink.
   */
  boost::system::error_code addAudioMediaSamples(const std::vector<MediaSample>& mediaSamples)
  {
    return deliverAudio(m_uiChannelId, mediaSamples);
  }
private:
  /**
   * @brief The subclass must implement delivery of video media samples to the media sink
   */
  virtual boost::system::error_code deliverVideo(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples) = 0;
  /**
   * @brief The subclass must implement delivery of audio media samples to the media sink
   */
  virtual boost::system::error_code deliverAudio(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples) = 0;

private:
  /// unique channel id
  uint32_t m_uiChannelId;
};

} //lme

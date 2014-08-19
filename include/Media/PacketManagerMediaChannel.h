#pragma once
#include <Media/MediaChannel.h>

namespace lme
{

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
    return boost::system::error_code();
  }

  /**
  * @brief The subclass must implement delivery of audio media samples to the media sink
  */
  virtual boost::system::error_code deliverAudio(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples)
  {
    return boost::system::error_code();
  }
};

} //lme


#pragma once
#include <cstdint>
#include <boost/optional.hpp>
#include <Media/MediaSample.h>

namespace lme
{

/**
 * @brief Base class for channel managers. A channel manager must make media accessible to the liveMedia task scheduler
 * Media can be retrieved by using the channel id and source id.
 */
class ChannelManager
{
public:
  /**
   * @brief Destructor
   */
  virtual ~ChannelManager()
  {

  }
  /**
   * @brief the implementation must return a media sample if the channel has received a media sample
   * of the specified channel and source id.
   */
  virtual boost::optional<MediaSample> getMedia(uint32_t uiChannelId, uint32_t uiSourceId) = 0;
  
};

} // lme


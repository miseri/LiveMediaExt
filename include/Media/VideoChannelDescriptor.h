#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace lme
{

/**
 * @brief Simple descriptor for video parameters
 */
struct VideoChannelDescriptor
{
  /// video codec
  std::string Codec;
  /// resolution width
  uint32_t Width;
  /// resolution height
  uint32_t Height;
  /// H.264/H.265 sequence parameter set
  std::string Sps;
  /// H.264/H.265 picture parameter set
  std::string Pps;
  /// frame bit limits for bitrate switching multiplexed media types
  std::vector<uint32_t> FrameBitLimits;
  /// initial channel
  uint32_t InitialChannel;

  /**
   * @brief Constructor
   */
  VideoChannelDescriptor()
    :Width(0),
    Height(0),
    InitialChannel(0)
  {

  }
};

} // lme

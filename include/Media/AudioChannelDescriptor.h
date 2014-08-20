#pragma once
#include <cstdint>
#include <string>

namespace lme
{

/**
 * @brief Simple descriptor for audio parameters
 */
struct AudioChannelDescriptor
{
  /// audio codec
  std::string Codec;
  /// number of channels
  uint32_t Channels;
  /// audio bits per sample (8, 16)
  uint32_t BitsPerSample;
  /// audio sampling frequency
  uint32_t SamplingFrequency;  
  /// config string. Needed for AAC
  std::string ConfigString;

  /**
   * @brief Constructor
   */
  AudioChannelDescriptor()
    :Channels(0),
    BitsPerSample(0),
    SamplingFrequency(0)
  {

  }

};

} // lme

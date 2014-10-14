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

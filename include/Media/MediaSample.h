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
#include <cpputil/Buffer.h>

namespace lme
{

// #define DEBUG_LME

  /**
 * @brief The MediaSample contains the data as well as timestamp information.
 *
 * The channel and source ids can be used to identify the media sample as belonging 
 * to a specific channel and source (e.g. audio/video) respectively.
 */
class MediaSample
{
public:
  /**
   * @brief Constructor
   */
  MediaSample()
    :m_dPresentationTime(0.0)
  {
#ifdef DEBUG_LME
    VLOG(2) << "MediaSample()";
#endif
  }
  ~MediaSample()
  {
#ifdef DEBUG_LME
    VLOG(2) << "~MediaSample()";
#endif
  }
  /**
   * @brief Getter for presentation time
   */
  double getPresentationTime() const { return m_dPresentationTime; }
  /**
   * @brief Setter for presentation time
   */
  void setPresentationTime(double dPresentationTime) { m_dPresentationTime = dPresentationTime; }
  /**
   * @brief Getter for media size
   */
  uint32_t getMediaSize() const { return m_data.getSize(); }
  /**
   * @brief Getter for media buffer
   */
  const Buffer& getDataBuffer() const { return m_data; }
  /**
   * @brief Setter for media data
   *
   * This class takes ownership of the data
   */
  void setData(uint8_t* pData, uint32_t uiSize)
  {
    m_data.setData(pData, uiSize);
  }
  /**
   * @brief Setter for media data
   */
  void setData(Buffer data) { m_data = data; }
  /**
   * @brief Getter for channel id
   */
  uint32_t getChannelId() const { return m_uiChannelId; }
  /**
   * @brief Setter for channel id
   */
  void setChannelId(uint32_t uiChannelId) { m_uiChannelId = uiChannelId;  }
  /**
   * @brief Getter for source id
   */
  uint32_t getSourceId() const { return m_uiSourceId; }
  /**
   * @brief Setter for source id
   */
  void setSourceId(uint32_t uiSourceId) { m_uiSourceId = uiSourceId;  }
private:
  /// Presentation time of media sample
  double m_dPresentationTime;
  /// media data
  Buffer m_data;
  /// channel id
  uint32_t m_uiChannelId;
  /// source id
  uint32_t m_uiSourceId;
};

} // lme

#pragma once
#include <cstdint>
#include <cpputil/Buffer.h>

namespace lme
{

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

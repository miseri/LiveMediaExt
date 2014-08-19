#pragma once
#include <cpputil/Buffer.h>

namespace lme
{

/**
 * @brief The MediaSample contains the data as well as timestamp information.
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

private:
  /// Presentation time of media sample
  double m_dPresentationTime;
  /// media data
  Buffer m_data;
};

} // lme

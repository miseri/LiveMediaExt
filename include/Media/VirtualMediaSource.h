#pragma once
#include <cstdint>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <Media/MediaSample.h>

namespace lme
{
/**
 * @brief typedef for receive callback. A vector struct is used for media types such as H.264
 * where an access unit is comprised of multiple NAL units.
 */
typedef boost::function<boost::system::error_code(const std::vector<MediaSample>&)> ReceiveMediaCb_t;

/**
 * @brief The VirtualMediaSource generates periodic data simulating a capture device.
 */
class VirtualMediaSource
{
public:
  /**
   * @brief Constructor
   */
  VirtualMediaSource(boost::asio::io_service& ioService, ReceiveMediaCb_t mediaCallback, uint32_t uiIntervalMs, uint32_t uiSampleSize);
  /**
   * @brief starts periodic generation of media samples
   */
  boost::system::error_code start();
  /**
   * @brief stops periodic generation of media samples
   */
  boost::system::error_code stop();

private:
  /**
   * @brief generateMediaSampleTimeout is called periodically and generates media samples.
   */
  void generateMediaSampleTimeout(const boost::system::error_code& ec);
  /// asio timer for sample scheduling
  boost::asio::deadline_timer m_timer;
  /// callback to be called on sample generation
  ReceiveMediaCb_t m_mediaCallback;
  /// interval between sample generation
  uint32_t m_uiIntervalMs;
  /// media sample size
  uint32_t m_uiMediaSampleSize;
  /// presentation time
  double m_dLastTimestamp;
};

} // lme

#pragma once
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <UsageEnvironment.hh>
#ifndef _RTSP_SERVER_HH
#include <RTSPServer.hh>
#endif

// fwd
class BasicUsageEnvironment;

namespace lme
{

// fwd
class LiveSourceTaskScheduler;

class RtspService : private boost::noncopyable
{
public:
  /**
   * @brief Constructor
   */
  RtspService();
  /**
   * @brief starts periodic generation of media samples
   */
  boost::system::error_code start();
  /**
   * @brief stops periodic generation of media samples
   */
  boost::system::error_code stop();


private:
  /// condition variable to control event loop lifetime
  char m_cEventloop;
  /// live555 RTSP server
  RTSPServer* m_pRtspServer;
  /// live555 task scheduler
  LiveSourceTaskScheduler* m_pScheduler;
  /// live555 usage environment
  BasicUsageEnvironment* m_pEnv;
};

} // lme

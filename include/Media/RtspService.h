#pragma once
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <UsageEnvironment.hh>
#ifndef _RTSP_SERVER_HH
#include <RTSPServer.hh>
#endif
#include <Media/AudioChannelDescriptor.h>
#include <Media/ChannelManager.h>
#include <Media/VideoChannelDescriptor.h>

// fwd
class BasicUsageEnvironment;

namespace lme
{

// fwd
class LiveSourceTaskScheduler;

/**
 * @brief The RtspService manages the delivery of media samples to clients via an RTSP server.
 *
 * Media samples can be handed over to the RTSP service for delivery by calling.
 * Each live stream or channel is identified by a channel ID. A channel may consist
 * of multiple media streams e.g. one for audio and one for video.
 */
class RtspService : private boost::noncopyable
{
public:
  /**
   * @brief Constructor
   */
  RtspService(ChannelManager& channelManager);
  /**
   * @brief starts periodic generation of media samples
   */
  boost::system::error_code start();
  /**
   * @brief stops periodic generation of media samples
   */
  boost::system::error_code stop();
  /**
   * @brief creates a channel for distribution.
   *
   * This results in a ServerMediaSession being added to the RTSP server.
   */
  boost::system::error_code createChannel(uint32_t uiChannelId, const VideoChannelDescriptor& videoDescriptor, const AudioChannelDescriptor& audioDescriptor);
  /**
   * @brief removes a channel for distribution
   *
   * This results in a ServerMediaSession being removed from the RTSP server.
   */
  boost::system::error_code removeChannel(uint32_t uiChannelId);

private:
  void live555EventLoop();
  void cleanupLiveMediaEnvironment();
  static void checkSessionsTask(void* clientData);
  void doCheckSessionsTask();
private:

  /// Channel manager
  ChannelManager& m_channelManager;
  /// condition variable to control event loop lifetime
  char m_cEventloop;
  /// live555 RTSP server
  RTSPServer* m_pRtspServer;
  /// live555 task scheduler
  LiveSourceTaskScheduler* m_pScheduler;
  /// live555 usage environment
  BasicUsageEnvironment* m_pEnv;
  /// live thread
  std::unique_ptr<boost::thread> m_pLive555Thread;
  /// flag for event loop status
  bool m_bEventLoopRunning;
  /// live555 tasks
  TaskToken m_pCheckSessionsTask;
};

} // lme

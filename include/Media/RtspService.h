#pragma once
#include <deque>
#include <map>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <UsageEnvironment.hh>
#include <LiveMediaExt/LiveRtspServer.h>
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
   * This results in a ServerMediaSession being added to the RTSP server. This method is thread-safe and can be called by any thread.
   */
  boost::system::error_code createChannel(uint32_t uiChannelId, const std::string& sChannelName, const VideoChannelDescriptor& videoDescriptor, const AudioChannelDescriptor& audioDescriptor);
  /**
   * @brief creates a video-only channel for distribution.
   *
   * This results in a ServerMediaSession being added to the RTSP server.
   */
  boost::system::error_code createChannel(uint32_t uiChannelId, const std::string& sChannelName, const VideoChannelDescriptor& videoDescriptor);
  /**
   * @brief creates a audio-only channel for distribution.
   *
   * This results in a ServerMediaSession being added to the RTSP server.
   */
  boost::system::error_code createChannel(uint32_t uiChannelId, const std::string& sChannelName, const AudioChannelDescriptor& audioDescriptor);
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
  static void checkChannelsTask(void* clientData);
  void doCheckChannelsTask();
private:

  /// Channel manager
  ChannelManager& m_channelManager;
  /// condition variable to control event loop lifetime
  char m_cEventloop;
  /// live555 RTSP server
  LiveRtspServer* m_pRtspServer;
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

  // At least the audio or video descriptor, must be set!
  std::deque<Channel> m_qChannelsToBeAdded;
  std::deque<Channel> m_qChannelsToBeRemoved;
  boost::mutex m_channelLock;

  typedef std::unordered_map<uint32_t, Channel> ChannelMap_t;
  ChannelMap_t m_mChannels;
};

} // lme

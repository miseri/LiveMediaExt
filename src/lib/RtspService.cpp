#include "LiveMediaExtPch.h"
#include <Media/RtspService.h>
#include <LiveMediaExt/LiveSourceTaskScheduler.h>
#include <LiveMediaExt/LiveRtspServer.h>

namespace lme
{

RtspService::RtspService(ChannelManager& channelManager)
  :m_channelManager(channelManager),
  m_cEventloop(0),
  m_pRtspServer(NULL),
  m_pScheduler(NULL),
  m_pEnv(NULL),
  m_bEventLoopRunning(false)
{

}

boost::system::error_code RtspService::start()
{
  VLOG(2) << "Starting RTSP service";
  // init live555 environment
  // Setup the liveMedia environment
  m_pScheduler = LiveSourceTaskScheduler::createNew(m_channelManager);
  // live media env
  m_pEnv = BasicUsageEnvironment::createNew(*m_pScheduler);
  VLOG(2) << "Creating RTSP server";
  m_pRtspServer = LiveRtspServer::createNew(*m_pEnv);
  if (m_pRtspServer == NULL)
  {
    *m_pEnv << "Failed to create RTSP server: " << m_pEnv->getResultMsg() << "\n";
    LOG(WARNING) << "Failed to create RTSP server";
    // TODO: add custom error codes!!!
    return boost::system::error_code(boost::system::errc::bad_file_descriptor, boost::system::get_generic_category());
  }

  // Add task that checks if there's new data in the queue
  checkSessionsTask(this);
  checkChannelsTask(this);

  // start live 555 in new thread
  m_pLive555Thread = std::unique_ptr<boost::thread>(new boost::thread(boost::bind(&RtspService::live555EventLoop, this)));
  return boost::system::error_code();
}

boost::system::error_code RtspService::stop()
{
  VLOG(2) << "Stopping RTSP service";
  // stop live555 thread
  if (m_pLive555Thread)
  {
    m_cEventloop = 1;
    m_pLive555Thread->join();
  }

  VLOG(2) << "End of RTSP service";
  return boost::system::error_code();
}

void RtspService::live555EventLoop()
{
  m_bEventLoopRunning = true;

  VLOG(2) << "Entering LiveMedia event loop";
  m_pEnv->taskScheduler().doEventLoop(&m_cEventloop); // does not return

  m_bEventLoopRunning = false;
  VLOG(2) << "LiveMedia event loop complete";

  cleanupLiveMediaEnvironment();
}

void RtspService::cleanupLiveMediaEnvironment()
{
  if (m_pEnv)
  {
    m_pEnv->taskScheduler().unscheduleDelayedTask(m_pCheckSessionsTask);
  }

  if (m_pRtspServer)
  {
    // Shutdown the server
    VLOG(2) << "Shutting down RTSP Server";
    Medium::close(m_pRtspServer);
    m_pRtspServer = NULL;
  }
  VLOG(2) << "Cleaning up";
  // clean up live555 environment
  m_pEnv->reclaim();
  if (m_pScheduler) delete m_pScheduler; m_pScheduler = NULL;
}

void RtspService::checkSessionsTask(void* clientData)
{
  RtspService* pRtspService = (RtspService*)clientData;
  pRtspService->doCheckSessionsTask();
}

void RtspService::doCheckSessionsTask()
{ 
  // TODO: process RTCP reports and update RTCP rate control module
  VLOG(10) << "doCheckSessionsTask";
  // Call this again, after a brief delay:
  int uSecsToDelay = 50000; // 50 ms
  m_pCheckSessionsTask = m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)&RtspService::checkSessionsTask, this);
}

void RtspService::checkChannelsTask(void* clientData)
{
  RtspService* pRtspService = (RtspService*)clientData;
  pRtspService->doCheckChannelsTask();
}

void RtspService::doCheckChannelsTask()
{
  boost::mutex::scoped_lock l(m_channelLock);
  // Remove old sessions:
  while (!m_qChannelsToBeRemoved.empty())
  {
    Channel channel = m_qChannelsToBeRemoved.front();
    m_qChannelsToBeRemoved.pop_front();
    assert(m_mChannels.find(channel.ChannelId) != m_mChannels.end());
    m_mChannels.erase(channel.ChannelId);
    LOG(INFO) << "Removing RTSP media session from RTSP server - Channel: " << channel.ChannelId << " Name: " << channel.ChannelName;
    m_pRtspServer->removeRtspMediaSession(channel);
  }

  // Add new sessions:
  while (!m_qChannelsToBeAdded.empty())
  {
    Channel newChannel = m_qChannelsToBeAdded.front();
    m_qChannelsToBeAdded.pop_front();
    m_mChannels[newChannel.ChannelId] = newChannel;
    LOG(INFO) << "Adding RTSP media session to RTSP server - Channel: " << newChannel.ChannelId << " Name: " << newChannel.ChannelName;
    m_pRtspServer->addRtspMediaSession(newChannel);
  }

  // Call this again, after a brief delay:
  int uSecsToDelay = 50000; // 50 ms
  m_pCheckSessionsTask = m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)&RtspService::checkChannelsTask, this);
}

boost::system::error_code RtspService::createChannel(uint32_t uiChannelId, const std::string& sChannelName, 
                                                     const VideoChannelDescriptor& videoDescriptor, const AudioChannelDescriptor& audioDescriptor)
{
  VLOG(2) << "createChannel: " << uiChannelId;
  boost::mutex::scoped_lock l(m_channelLock);
  ChannelMap_t::iterator it = m_mChannels.find(uiChannelId);
  if (it != m_mChannels.end())
  {
    return boost::system::error_code(boost::system::errc::file_exists, boost::system::get_generic_category());
  }
  else
  {
    m_qChannelsToBeAdded.push_back(Channel(uiChannelId, sChannelName, videoDescriptor, audioDescriptor));
    return boost::system::error_code();
  }
}

boost::system::error_code RtspService::createChannel(uint32_t uiChannelId, const std::string& sChannelName, const VideoChannelDescriptor& videoDescriptor)
{
  VLOG(2) << "createChannel: " << uiChannelId;
  boost::mutex::scoped_lock l(m_channelLock);
  ChannelMap_t::iterator it = m_mChannels.find(uiChannelId);
  if (it != m_mChannels.end())
  {
    return boost::system::error_code(boost::system::errc::file_exists, boost::system::get_generic_category());
  }
  else
  {
    m_qChannelsToBeAdded.push_back(Channel(uiChannelId, sChannelName, videoDescriptor));
    VLOG(2) << "createChannel: " << uiChannelId << " channel added";
    return boost::system::error_code();
  }
  return boost::system::error_code();
}

boost::system::error_code RtspService::createChannel(uint32_t uiChannelId, const std::string& sChannelName, const AudioChannelDescriptor& audioDescriptor)
{
  VLOG(2) << "createChannel: " << uiChannelId;
  boost::mutex::scoped_lock l(m_channelLock);
  ChannelMap_t::iterator it = m_mChannels.find(uiChannelId);
  if (it != m_mChannels.end())
  {
    return boost::system::error_code(boost::system::errc::file_exists, boost::system::get_generic_category());
  }
  else
  {
    m_qChannelsToBeAdded.push_back(Channel(uiChannelId, sChannelName, audioDescriptor));
    return boost::system::error_code();
  }
  return boost::system::error_code();
}

boost::system::error_code RtspService::removeChannel(uint32_t uiChannelId)
{
  VLOG(2) << "removeChannel: " << uiChannelId;
  boost::mutex::scoped_lock l(m_channelLock);
  ChannelMap_t::iterator it = m_mChannels.find(uiChannelId);
  if (it != m_mChannels.end())
  {
    m_qChannelsToBeRemoved.push_back(m_mChannels[uiChannelId]);
    return boost::system::error_code();
  }
  else
  {
    return boost::system::error_code(boost::system::errc::no_such_file_or_directory, boost::system::get_generic_category());
  }
  return boost::system::error_code();
}

} //lme


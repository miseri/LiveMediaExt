#include "LiveMediaExtPch.h"
#include <Media/RtspService.h>
#include <LiveMediaExt/LiveSourceTaskScheduler.h>
#include <LiveMediaExt/LiveRtspServer.h>

// #define TEST_STREAMS
#ifdef TEST_STREAMS
#include "liveMedia.hh"
#endif

namespace lme
{

RtspService::RtspService(ChannelManager& channelManager, IRateAdaptationFactory* pFactory, IRateController* pGlobalRateControl)
  :m_channelManager(channelManager),
  m_cEventloop(0),
  m_pRtspServer(NULL),
  m_pScheduler(NULL),
  m_pEnv(NULL),
  m_bEventLoopRunning(false),
  m_pFactory(pFactory),
  m_pGlobalRateControl(pGlobalRateControl)
{

}

boost::system::error_code RtspService::init()
{
  VLOG(2) << "Initialising RTSP service";
  // init live555 environment
  // Setup the liveMedia environment
  m_pScheduler = LiveSourceTaskScheduler::createNew(m_channelManager);
  // live media env
  m_pEnv = BasicUsageEnvironment::createNew(*m_pScheduler);
  VLOG(2) << "Creating RTSP server";
  m_pRtspServer = LiveRtspServer::createNew(*m_pEnv, 554, 0, m_pFactory, m_pGlobalRateControl);
  if (m_pRtspServer == NULL)
  {
    *m_pEnv << "Failed to create RTSP server: " << m_pEnv->getResultMsg() << "\n";
    LOG(WARNING) << "Failed to create RTSP server";
    // TODO: add custom error codes!!!
    return boost::system::error_code(boost::system::errc::bad_file_descriptor, boost::system::get_generic_category());
  }
#if 0
  // disable TCP streaming for testing
  m_pRtspServer->disableStreamingRTPOverTCP();
#endif

  // taken from testOnDemandRTSPServer
#ifdef TEST_STREAMS
  char const* descriptionString
    = "Session streamed by \"testOnDemandRTSPServer\"";

  // An AAC audio stream (ADTS-format file):
  {
    char const* streamName = "aacAudioTest";
    char const* inputFileName = "test.aac";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*m_pEnv, streamName, streamName,
      descriptionString);
    sms->addSubsession(ADTSAudioFileServerMediaSubsession
      ::createNew(*m_pEnv, inputFileName, false));
    m_pRtspServer->addServerMediaSession(sms);

    //announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // An AMR audio stream:
  {
    char const* streamName = "amrAudioTest";
    char const* inputFileName = "test.amr";
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*m_pEnv, streamName, streamName,
      descriptionString);
    sms->addSubsession(AMRAudioFileServerMediaSubsession
      ::createNew(*m_pEnv, inputFileName, false));
    m_pRtspServer->addServerMediaSession(sms);

    // announceStream(m_pRtspServer, sms, streamName, inputFileName);
  }
#endif
  // Add task that checks if there's new data in the queue
  checkSessionsTask(this);
  checkChannelsTask(this);

  return boost::system::error_code();
}

boost::system::error_code RtspService::start()
{
  VLOG(2) << "Starting RTSP service";
#ifdef RUN_LIV555_IN_NEW_THREAD
  // start live 555 in new thread
  m_pLive555Thread = std::unique_ptr<boost::thread>(new boost::thread(boost::bind(&RtspService::live555EventLoop, this)));

#else
  live555EventLoop();
#endif
  return boost::system::error_code();
}

boost::system::error_code RtspService::stop()
{
  VLOG(2) << "Stopping RTSP service";
  // TODO: could check that we are actually running
#ifdef RUN_LIV555_IN_NEW_THREAD
  // stop live555 thread
  if (m_pLive555Thread)
  {
    m_cEventloop = 1;
    m_pLive555Thread->join();
  }
#else
  m_cEventloop = 1;
  // in this case we can't wait for the end and the application layer has to do it?!?
#endif
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
    m_pEnv->taskScheduler().unscheduleDelayedTask(m_pCheckChannelsTask);    
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
  VLOG(2) << "Clean up done";
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
  m_pScheduler->processLiveMediaSessions();
  // Call this again, after a brief delay:
  int uSecsToDelay = 1000000; // 1s
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
  m_pCheckChannelsTask = m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)&RtspService::checkChannelsTask, this);
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


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

  // start live 555 in new thread
  m_pLive555Thread = std::unique_ptr<boost::thread>(new boost::thread(boost::bind(&RtspService::live555EventLoop, this)));
  return boost::system::error_code();
}

boost::system::error_code RtspService::stop()
{
  // stop live555 thread
  if (m_pLive555Thread)
  {
    m_cEventloop = 1;
    m_pLive555Thread->join();
  }

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
  // TODO: add/remove dynamic channels
  
  // TODO: process RTCP reports and update RTCP rate control module

  //  // Remove old sessions:
//  while (m_qMediaSessionsToBeRemoved.hasItems())
//  {
//    std::string sSession = m_qMediaSessionsToBeRemoved.pop();
//
//    LOG(INFO) << "Ending RTSP media session: " << sSession;
//    /// code to kick clients before removing session so that there are no outstanding references
//#if 1
//    m_pRtspServer->endServerSession(sSession);
//#endif
//
//    LOG(INFO) << "Removing RTSP media session from RTSP server: " << sSession;
//    m_pRtspServer->removeTranscoderMediaSession(sSession);
//  }
//
//  // Add new sessions:
//  while (m_qMediaSessions.hasItems())
//  {
//    std::string sSession = m_qMediaSessions.pop();
//    LOG(INFO) << "Adding RTSP media session to RTSP server: " << sSession;
//    m_pRtspServer->addTranscoderMediaSession(sSession);
//  }
//
  // Call this again, after a brief delay:
  int uSecsToDelay = 50000; // 50 ms
  m_pCheckSessionsTask = m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)&RtspService::checkSessionsTask, this);
}

} //lme


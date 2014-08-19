#include "stdafx.h"
#include "VersionInfo.h"
#include <boost/filesystem.hpp>
#include <cpputil/ConsoleApplicationUtil.h>
#include <cpputil/GenericParameters.h>
#include <cpputil/ServiceManager.h>
#include <Media/PacketManagerMediaChannel.h>
#include <Media/RtspService.h>
#include <Media/VirtualMediaSource.h>

using namespace lme;

/**
 * @brief Test program that simulates receiving data from a capture device 
 * and passing it to live555 for delivery.
 */
int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);

  boost::filesystem::path currentDirectory = boost::filesystem::current_path() / "../Log/";
  const std::string sLogFile = currentDirectory.string() + std::string("LiveMediaSourceStreamer");
  // update the log file: we want to be able to parse this file
  google::SetLogDestination(google::GLOG_INFO, (sLogFile + ".INFO").c_str());
  google::SetLogDestination(google::GLOG_WARNING, (sLogFile + ".WARNING").c_str());
  google::SetLogDestination(google::GLOG_ERROR, (sLogFile + ".ERROR").c_str());

  std::ostringstream ostr;
  ostr << "(LiveMediaSourceStreamer v" << VersionInfo::toString() << ") ";
  for (int i = 0; i < argc; ++i)
    ostr << argv[i] << " ";
  LOG(INFO) << ostr.str();

  // event loop manager
  ServiceManager serviceManager;

  // Create a channel to bridge from the virtual capture device to the RTSP service
  uint32_t uiChannelId = 12345;
  PacketManagerMediaChannel channel(uiChannelId);

  // create source thread to simulate live capture
  VirtualMediaSource virtualSource(serviceManager.getIoService(), boost::bind(&PacketManagerMediaChannel::addVideoMediaSamples, boost::ref(channel), _1), 40, 1000);
  RtspService rtspService;

  // register media session with manager
  uint32_t uiCaptureServiceId;
  bool bSuccess = serviceManager.registerService(boost::bind(&VirtualMediaSource::start, boost::ref(virtualSource)),
    boost::bind(&VirtualMediaSource::stop, boost::ref(virtualSource)),
    uiCaptureServiceId);

  if (!bSuccess)
  {
    LOG(WARNING) << "Failed to register virtual media source service";
    return -1;
  }

  uint32_t uiRtspServiceId;
  bSuccess = serviceManager.registerService(boost::bind(&RtspService::start, boost::ref(rtspService)),
    boost::bind(&RtspService::stop, boost::ref(rtspService)),
    uiRtspServiceId);

  if (!bSuccess)
  {
    LOG(WARNING) << "Failed to register RTSP service";
    return -1;
  }

  // the user can now specify the handlers for incoming samples.
  startEventLoop(boost::bind(&ServiceManager::start, boost::ref(serviceManager)),
    boost::bind(&ServiceManager::stop, boost::ref(serviceManager)));

  return 0;
}

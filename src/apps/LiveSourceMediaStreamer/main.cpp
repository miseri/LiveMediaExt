/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "CSIR"
// Copyright (c) 2014 CSIR.  All rights reserved.
#include "stdafx.h"
#include "VersionInfo.h"
#include <boost/filesystem.hpp>
#include <cpputil/ConsoleApplicationUtil.h>
#include <cpputil/GenericParameters.h>
#include <cpputil/ServiceManager.h>
#include <Media/MediaTypes.h>
#include <Media/SingleChannelManager.h>
#include <Media/RtspService.h>
#include <Media/VirtualMediaSource.h>
#include <SimpleRateAdaptation/SimpleRateAdaptationFactory.h>

using namespace lme;

/**
 * @brief Test program that simulates receiving data from a capture device 
 * and passing it to live555 for delivery. 
 *
 * @TODO: use live555 classes to read files
 * and use a custom sink to pass it to the LME for final delivery also using live555
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

// #define VIDEO
#define AUDIO

  // Create a channel to bridge from the virtual capture device to the RTSP service
  uint32_t uiChannelId = 12345;
  std::string sRtspUri("test");
  // HACK for now
#ifdef VIDEO
#ifdef AUDIO
  uint32_t uiVideoId = 0;
  uint32_t uiAudioId = 1;
#else
  uint32_t uiVideoId = 0;
  uint32_t uiAudioId = 1;
#endif
#else
#ifdef AUDIO
  uint32_t uiVideoId = 1;
  uint32_t uiAudioId = 0;
#endif
#endif
  SingleChannelManager channelManager(uiChannelId, uiVideoId, uiAudioId);
  PacketManagerMediaChannel& packetManager = channelManager.getPacketManager();
  bool bSuccess;

  VideoChannelDescriptor videoDescriptor;
  videoDescriptor.Codec = H264;
  videoDescriptor.Width = 352;
  videoDescriptor.Height = 288;
#ifdef VIDEO
  // create source thread to simulate live capture
  VirtualMediaSource videoSource(serviceManager.getIoService(), boost::bind(&PacketManagerMediaChannel::addVideoMediaSamples, boost::ref(packetManager), _1), 40, 1000);
  // register media session with manager
  VLOG(2) << "Registering capture source";
  uint32_t uiVideoCaptureServiceId;
  bSuccess = serviceManager.registerService(boost::bind(&VirtualMediaSource::start, boost::ref(videoSource)),
    boost::bind(&VirtualMediaSource::stop, boost::ref(videoSource)),
    uiVideoCaptureServiceId);
  if (!bSuccess)
  {
    LOG(WARNING) << "Failed to register video media source service";
    return -1;
  }
#endif

  AudioChannelDescriptor audioDescriptor;
  audioDescriptor.Codec = AMR;
  audioDescriptor.BitsPerSample = 8;
  audioDescriptor.SamplingFrequency = 8000;
  audioDescriptor.Channels = 1;

#ifdef AUDIO
  VirtualMediaSource audioSource(serviceManager.getIoService(), boost::bind(&PacketManagerMediaChannel::addAudioMediaSamples, boost::ref(packetManager), _1), 20, 13);
  // register media session with manager
  VLOG(2) << "Registering audio capture source";
  uint32_t uiAudioCaptureServiceId;
  bSuccess = serviceManager.registerService(boost::bind(&VirtualMediaSource::start, boost::ref(audioSource)),
    boost::bind(&VirtualMediaSource::stop, boost::ref(audioSource)),
    uiAudioCaptureServiceId);
  if (!bSuccess)
  {
    LOG(WARNING) << "Failed to register audio media source service";
    return -1;
  }
#endif

  IRateAdaptationFactory* pFactory = new SimpleRateAdaptationFactory();
  RtspService rtspService(channelManager, pFactory);

  boost::system::error_code ec = rtspService.init();

  if (ec)
  {
    LOG(WARNING) << "Failed to initialise RTSP service";
    return -1;
  }

  VLOG(2) << "Registering RTSP service";
  uint32_t uiRtspServiceId;
  bSuccess = serviceManager.registerService(boost::bind(&RtspService::start, boost::ref(rtspService)),
                                            boost::bind(&RtspService::stop, boost::ref(rtspService)),
                                            uiRtspServiceId);

  if (!bSuccess)
  {
    LOG(WARNING) << "Failed to register RTSP service";
    return -1;
  }

#ifdef VIDEO
  // Bogus video descriptor for testing
  boost::system::error_code ec = rtspService.createChannel(uiChannelId, sRtspUri, videoDescriptor);
  if (ec)
  {
    LOG(WARNING) << "Error creating RTSP service channel: " << ec.message();
  }
#else
#ifdef AUDIO
  // Bogus audio descriptor for testing
  ec = rtspService.createChannel(uiChannelId, sRtspUri, audioDescriptor);
  if (ec)
  {
    LOG(WARNING) << "Error creating RTSP service channel: " << ec.message();
  }
#else
  // Bogus video and audio descriptors for testing
  boost::system::error_code ec = rtspService.createChannel(uiChannelId, sRtspUri, videoDescriptor, audioDescriptor);
  if (ec)
  {
    LOG(WARNING) << "Error creating RTSP service channel: " << ec.message();
  }
#endif
#endif
  // the user can now specify the handlers for incoming samples.
  startEventLoop(boost::bind(&ServiceManager::start, boost::ref(serviceManager)),
    boost::bind(&ServiceManager::stop, boost::ref(serviceManager)));

  LOG(INFO) << "Complete.";

  delete pFactory;
  return 0;
}

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
#pragma once
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <cpputil/Conversion.h>
#include <LiveMediaExt/LiveAACSubsession.h>
#include <LiveMediaExt/LiveAMRSubsession.h>
#include <LiveMediaExt/LiveH264Subsession.h>
#include <LiveMediaExt/LiveMediaSubsession.h>
#include <Media/AudioChannelDescriptor.h>
#include <Media/VideoChannelDescriptor.h>
#include <Media/MediaTypes.h>

namespace lme
{

typedef std::map<std::string, std::string> StringMap_t;

class LiveRtspServer;

/// This class is responsible for creating the Live555 media subsession according to 
/// passed in media subsession
class LiveMediaSubsessionFactory
{
public:

  /// This method creates the appropriate live555 media subsession
  /// @param env The live555 environment
  /// @param rRtspServer The live555 RTSP server
  /// @param sSessionName A human readable string for the session name
  /// @param uiChannelId The channel Id assigned by the media web server
  /// @param uiSubsessionId A unique Id: this is currently derived
  ///        from the port and subsession id of the incoming media subsession
  static LiveMediaSubsession* createVideoSubsession( UsageEnvironment& env,
                                          LiveRtspServer& rRtspServer,
                                          const std::string& sSessionName,
                                          const unsigned uiChannelId,
                                          unsigned uiSubsessionId,
                                          const VideoChannelDescriptor& videoDescriptor,
                                          IRateAdaptationFactory* pFactory,
                                          IRateController* pRateControl)
  {
    VLOG(2) << "Creating new LiveMediaSubsession " << sSessionName 
            << " Channel Id: " << uiChannelId
            << " Subsession Id: " << uiSubsessionId;

    LiveMediaSubsession* pMediaSubsession = NULL;
    if (videoDescriptor.Codec == H264)
    {
      // TODO: return null pointer if SPS/PPS are not set?
      VLOG(2) << "Adding H264 subsession";
      pMediaSubsession = new LiveH264Subsession(env, rRtspServer,
        uiChannelId, uiSubsessionId,
        sSessionName,
        videoDescriptor.Sps, videoDescriptor.Pps, 
        pFactory, pRateControl);
    }
    else
    {
      LOG(WARNING) << "Invalid media subtype: " << videoDescriptor.Codec;
      pMediaSubsession = NULL;
    }
    
    return pMediaSubsession;
  }

  /**
   * @brief 
   */
  static LiveMediaSubsession* createAudioSubsession(UsageEnvironment& env,
                                                    LiveRtspServer& rRtspServer,
                                                    const std::string& sSessionName,
                                                    const unsigned uiChannelId,
                                                    unsigned uiSubsessionId,
                                                    const AudioChannelDescriptor& audioDescriptor,
                                                    IRateAdaptationFactory* pFactory,
                                                    IRateController* pRateControl)
  {
    VLOG(2) << "Creating new audio LiveMediaSubsession " << sSessionName
            << " Channel Id: " << uiChannelId
            << " Subsession Id: " << uiSubsessionId;
    LiveMediaSubsession* pMediaSubsession = NULL;
    if (audioDescriptor.Codec == AMR)
    {
      // TODO: return null pointer if audio parameters are not set?
      VLOG(2) << "Adding AMR subsession: channels: " << audioDescriptor.Channels 
              << " bits per sample: " << audioDescriptor.BitsPerSample 
              << " sampling frequency: " << audioDescriptor.SamplingFrequency;
      pMediaSubsession = new LiveAMRSubsession(env, rRtspServer,
        uiChannelId, uiSubsessionId,
        sSessionName,
        audioDescriptor.Channels, audioDescriptor.BitsPerSample, audioDescriptor.SamplingFrequency, 
        pFactory, pRateControl);
    }
    else if (audioDescriptor.Codec == AAC)
    {
      // TODO: return null pointer if audio parameters are not set?
      VLOG(2) << "Adding AAC subsession: channels: " << audioDescriptor.Channels
        << " bits per sample: " << audioDescriptor.BitsPerSample
        << " sampling frequency: " << audioDescriptor.SamplingFrequency
        << " AAC config string: " << audioDescriptor.ConfigString;
      pMediaSubsession = new LiveAACSubsession(env, rRtspServer,
        uiChannelId, uiSubsessionId,
        sSessionName,
        audioDescriptor.Channels, audioDescriptor.BitsPerSample, audioDescriptor.SamplingFrequency,
        audioDescriptor.ConfigString,
        pFactory, pRateControl);
    }
    else
    {
      LOG(WARNING) << "Invalid media subtype: " << audioDescriptor.Codec;
      pMediaSubsession = NULL;
    }
    return pMediaSubsession;
  }
};

} // lme

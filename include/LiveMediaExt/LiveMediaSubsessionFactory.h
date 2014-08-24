#pragma once
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <cpputil/Conversion.h>
//#include "LiveAACSubsession.h"
#include <LiveMediaExt/LiveAMRSubsession.h>
//#include "LiveH263Subsession.h"
#include <LiveMediaExt/LiveH264Subsession.h>
#include <LiveMediaExt/LiveMediaSubsession.h>
//#include "LiveMultiplexedH263Subsession.h"
//#include "LiveMultiplexedH264Subsession.h"
//#include "LivePcmSubsession.h"
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
                                          const VideoChannelDescriptor& videoDescriptor
                                          )
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
        videoDescriptor.Sps, videoDescriptor.Pps);
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
                                                    const AudioChannelDescriptor& audioDescriptor
    )
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
        audioDescriptor.Channels, audioDescriptor.BitsPerSample, audioDescriptor.SamplingFrequency);
    }
    else
    {
      LOG(WARNING) << "Invalid media subtype: " << audioDescriptor.Codec;
      pMediaSubsession = NULL;
    }
    return pMediaSubsession;
  }

private:


};

} // lme

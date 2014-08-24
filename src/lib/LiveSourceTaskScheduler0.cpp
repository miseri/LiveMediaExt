#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveSourceTaskScheduler0.h>
#include <LiveMediaExt/LiveMediaSubsession.h>
#include <Media/ChannelManager.h>

namespace lme
{

const unsigned MAX_SAMPLES_TO_BE_PROCESSED_IN_EVENT_LOOP = 10;
const unsigned MAX_GROUP_SAMPLES_TO_BE_PROCESSED_IN_EVENT_LOOP = 5;

LiveSourceTaskScheduler0::LiveSourceTaskScheduler0(ChannelManager& channelManager)
  : m_uMaxDelayTime(100000), // default time of once per second,
  BasicTaskScheduler(m_uMaxDelayTime),
  m_channelManager(channelManager)
{

}

LiveSourceTaskScheduler0::~LiveSourceTaskScheduler0()
{

}

void LiveSourceTaskScheduler0::doEventLoop( char* watchVariable )
{
  // Repeatedly loop, handling readable sockets and timed events:
  while (1) 
  {
    if (watchVariable != NULL)
    {
      if (*watchVariable != 0)
      {
        bool bExitEventLoop = false;
        switch ((*watchVariable))
        {
          // Exit event loop
        case 1:
          {
            bExitEventLoop = true;
            break;
          }
        }
        if (bExitEventLoop)
        {
          // Break out the while
          break;
        }
      }
    }

#ifdef DEBUG
    VLOG(2) << "Calling processLiveSources";
#endif
    processLiveSources();
#ifdef DEBUG
    VLOG(2) << "Done processLiveSources, calling SingleStep";
#endif
    // Call the live media single step
    SingleStep(m_uMaxDelayTime);
#ifdef DEBUG
    VLOG(2) << "Done  SingleStep";
#endif
  }
}

void LiveSourceTaskScheduler0::addMediaSubsession(uint32_t  uiChannelId, uint32_t uiSourceId, LiveMediaSubsession* pMediaSubsession)
{
#ifdef DEBUG
  VLOG(2) << "LiveSourceTaskScheduler0::addMediaSubsession: " << uiChannelId << " source: " << uiSourceId;
#endif
  MediaSessionMap_t::iterator it = m_mMediaSubsessions.find(uiChannelId);
  if (it != m_mMediaSubsessions.end())
  {
    MediaSession_t::iterator it2 = it->second.find(uiSourceId);
    if (it2 != it->second.end())
    {
      LOG(WARNING) << "Media subsession with source ID " << uiSourceId << " already registered in scheduler!";
      assert(false);
    }
    else
    {
      VLOG(5) << "Registering media subsession for channel: " << uiChannelId 
              << " source: " << uiSourceId;
      it->second[uiSourceId] = pMediaSubsession;
    }
  }
  else
  {
    VLOG(5) << "Registering media subsession for channel: " << uiChannelId
      << " source: " << uiSourceId;
    m_mMediaSubsessions[uiChannelId][uiSourceId] = pMediaSubsession;
  }
}

void LiveSourceTaskScheduler0::removeMediaSubsession(uint32_t  uiChannelId, uint32_t uiSourceId, LiveMediaSubsession* pMediaSubsession)
{
#ifdef DEBUG
  VLOG(5) << "Trying to remove media subsession with channel: " << uiChannelId << " source: " << uiSourceId;
#endif

  MediaSessionMap_t::iterator it = m_mMediaSubsessions.find(uiChannelId);
  if (it != m_mMediaSubsessions.end())
  {
    MediaSession_t::iterator it2 = it->second.find(uiSourceId);
    if (it2 != it->second.end())
    {
      VLOG(5) << "Removing media subsession for channel: " << uiChannelId << " source: " << uiSourceId;
      m_mMediaSubsessions.erase(it);
    }
    else
    {
      LOG(WARNING) << "Unable to remove media subsession with channel: " << uiChannelId << " source: " << uiSourceId;
    }
  }
  else
  {
    LOG(WARNING) << "Unable to remove media subsession with ID: " << uiChannelId;
  }
}

void LiveSourceTaskScheduler0::processLiveSources()
{
#ifdef DEBUG
  VLOG(2) << "Processing live sources";
#endif
  // try and retrieve a sample for each channel
  for (MediaSessionMap_t::iterator it = m_mMediaSubsessions.begin(); it != m_mMediaSubsessions.end(); ++it)
  {
#ifdef DEBUG
    VLOG(2) << "Retrieving samples for channel: " << it->first;
#endif
    for (MediaSession_t::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
    {
#ifdef DEBUG
      VLOG(2) << "Retrieving samples for channel " << it->first << " source " << it2->first;
#endif
      boost::optional<MediaSample> pMediaSample = m_channelManager.getMedia(it->first, it2->first);
      if (pMediaSample)
      {
#ifdef DEBUG
        VLOG(2) << "Got media sample for channel " << it->first << " source " << it2->first << " PTS: " << pMediaSample->getPresentationTime();
#endif
        // make sure channel and source ids are set
        pMediaSample->setChannelId(it->first);
        pMediaSample->setSourceId(it2->first);
#ifdef DEBUG
        VLOG(2) << "Calling addMediaSample for channel " << it->first << " source " << it2->first << " PTS: " << pMediaSample->getPresentationTime();
#endif
        it2->second->addMediaSample(*pMediaSample);
#ifdef DEBUG
        VLOG(2) << "Done calling addMediaSample for channel " << it->first << " source " << it2->first << " PTS: " << pMediaSample->getPresentationTime();
#endif
      }
    }
  }
}

} // lme


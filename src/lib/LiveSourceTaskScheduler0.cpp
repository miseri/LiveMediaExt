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
  MediaSessionMap_t::iterator it = m_mMediaSessions.find(uiChannelId);
  if (it != m_mMediaSessions.end())
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
    m_mMediaSessions[uiChannelId][uiSourceId] = pMediaSubsession;
  }
}

void LiveSourceTaskScheduler0::removeMediaSubsession(uint32_t  uiChannelId, uint32_t uiSourceId, LiveMediaSubsession* pMediaSubsession)
{
#ifdef DEBUG
  VLOG(5) << "Trying to remove media subsession with channel: " << uiChannelId << " source: " << uiSourceId;
#endif

  MediaSessionMap_t::iterator it = m_mMediaSessions.find(uiChannelId);
  if (it != m_mMediaSessions.end())
  {
    MediaSession_t::iterator it2 = it->second.find(uiSourceId);
    if (it2 != it->second.end())
    {
      VLOG(5) << "Removing media subsession for channel: " << uiChannelId << " source: " << uiSourceId;
      m_mMediaSessions.erase(it);
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
  for (MediaSessionMap_t::iterator it = m_mMediaSessions.begin(); it != m_mMediaSessions.end(); ++it)
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

void LiveSourceTaskScheduler0::processLiveMediaSessions()
{
  VLOG(15) << "LiveSourceTaskScheduler0::processLiveMediaSessions()";
  for (MediaSessionMap_t::iterator it = m_mMediaSessions.begin(); it != m_mMediaSessions.end(); ++it)
  {
    for (MediaSession_t::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
    {
      it2->second->processClientStatistics();
    }
  }
}

} // lme


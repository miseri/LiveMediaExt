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
#include <Media/PacketManagerMediaChannel.h>

namespace lme
{

#define AUDIO_TYPE 0
#define VIDEO_TYPE 1
#define MAX_QUEUE_DURATION_SECONDS 0.2 // 200 ms

#ifndef LOCK_FREE

  void printQueueInfo(const std::deque<MediaSample_t>& mediaSamples)
  {
    std::ostringstream ostr;
    for (auto& pair : mediaSamples)
    {
      if (pair.first == AUDIO_TYPE)
      {
        ostr << "(A, " << pair.second.getPresentationTime() << ") ";
      }
      else
      {
        ostr << "(V, " << pair.second.getPresentationTime() << ") ";
      }
    }

    VLOG(2) << "Queue info: " << ostr.str();
  }

  void printQueueInfo2(const std::deque<MediaSample_t>& mediaSamples)
  {
    std::ostringstream ostr;
    for (auto& pair : mediaSamples)
    {
      if (pair.first == AUDIO_TYPE)
      {
        ostr << "A";
      }
      else
      {
        ostr << "V";
      }
    }

    VLOG(2) << "Queue info: (1st: " << mediaSamples.front().second.getPresentationTime() << " " << ostr.str();
  }

#endif

  PacketManagerMediaChannel::PacketManagerMediaChannel(uint32_t uiChannelId)
    :MediaChannel(uiChannelId)
#ifndef LOCK_FREE
#ifdef SINGLE_QUEUE
    , m_dQueueDuration(0.0)
#endif
#endif
  {
#ifdef LOCK_FREE
    VLOG(2) << "Using lock free implementation";
#else
    VLOG(2) << "Using mutex-based implementation";
#endif

#ifdef SINGLE_QUEUE
    VLOG(2) << "Using single queue for audio and video";
#else
    VLOG(2) << "Using separate queues for audio and video";
#endif
  }

  boost::system::error_code PacketManagerMediaChannel::deliverVideo(uint32_t uiChannelId, const std::vector<MediaSample>& mediaSamples)
  {
#ifdef LOCK_FREE
    std::vector<MediaSample>::const_iterator it = m_videoSamples.push(mediaSamples.begin(), mediaSamples.end());
    assert(it == mediaSamples.end());
    return boost::system::error_code();
#else
#ifdef SINGLE_QUEUE
    boost::mutex::scoped_lock l(m_mediaLock);
    for (const MediaSample& mediaSample : mediaSamples)
    {
      if (m_mediaSamples.empty())
      {
        m_mediaSamples.push_back(std::make_pair(VIDEO_TYPE, mediaSample));
      }
      else
      {
        for (std::deque<MediaSample_t>::reverse_iterator it = m_mediaSamples.rbegin();
          it != m_mediaSamples.rend(); ++it)
        {
          if (mediaSample.getPresentationTime() > it->second.getPresentationTime())
          {
            m_mediaSamples.insert(it.base(), std::make_pair(VIDEO_TYPE, mediaSample));
            break;
          }
        }
      }
    }
    m_dQueueDuration = m_mediaSamples.back().second.getPresentationTime() - m_mediaSamples.front().second.getPresentationTime();
    if (m_dQueueDuration >= MAX_QUEUE_DURATION_SECONDS)
    {
      VLOG(5) << "Queue duration is longer than " << MAX_QUEUE_DURATION_SECONDS << "s: " << m_dQueueDuration << "s";
    }
#if 0
    printQueueInfo(m_mediaSamples);
#else
    printQueueInfo2(m_mediaSamples);
#endif
#else
    boost::mutex::scoped_lock l(m_videoLock);
    m_videoSamples.insert(m_videoSamples.end(), mediaSamples.begin(), mediaSamples.end());
#endif
    return boost::system::error_code();
#endif
  }

  boost::system::error_code PacketManagerMediaChannel::deliverAudio(uint32_t uiChannelId, const std::vector<MediaSample>& mediaSamples)
  {
    //    fprintf(stderr, "deliverAudio");
#ifdef LOCK_FREE
    std::vector<MediaSample>::const_iterator it = m_audioSamples.push(mediaSamples.begin(), mediaSamples.end());
    assert(it == mediaSamples.end());
    return boost::system::error_code();
#else
#ifdef SINGLE_QUEUE
    boost::mutex::scoped_lock l(m_mediaLock);
    for (const MediaSample& mediaSample : mediaSamples)
    {
      if (m_mediaSamples.empty())
      {
        m_mediaSamples.push_back(std::make_pair(AUDIO_TYPE, mediaSample));
      }
      else
      {
        for (std::deque<MediaSample_t>::reverse_iterator it = m_mediaSamples.rbegin();
          it != m_mediaSamples.rend(); ++it)
        {
          if (mediaSample.getPresentationTime() > it->second.getPresentationTime())
          {
            m_mediaSamples.insert(it.base(), std::make_pair(AUDIO_TYPE, mediaSample));
            break;
          }
        }
      }
    }
    m_dQueueDuration = m_mediaSamples.back().second.getPresentationTime() - m_mediaSamples.front().second.getPresentationTime();
    if (m_dQueueDuration >= MAX_QUEUE_DURATION_SECONDS)
    {
      VLOG(5) << "Queue duration is longer than " << MAX_QUEUE_DURATION_SECONDS << "s: " << m_dQueueDuration << "s";
    }
#if 0
    printQueueInfo(m_mediaSamples);
#else
    printQueueInfo2(m_mediaSamples);
#endif
#else
    boost::mutex::scoped_lock l(m_audioLock);
    m_audioSamples.insert(m_audioSamples.end(), mediaSamples.begin(), mediaSamples.end());
#endif
    return boost::system::error_code();
#endif
  }

  boost::optional<MediaSample> PacketManagerMediaChannel::getVideo()
  {
#ifdef LOCK_FREE
    MediaSample mediaSample;
    if (m_videoSamples.pop(mediaSample))
    {
      return boost::optional<MediaSample>(mediaSample);
    }
    else
    {
      // empty
      return boost::optional<MediaSample>();
    }
#else
#ifdef SINGLE_QUEUE
    boost::mutex::scoped_lock l(m_mediaLock);
    // make sure there is at least one audio sample in the queue so we can align the two streams
    auto itAudio = std::find_if(m_mediaSamples.begin(), m_mediaSamples.end(), [](const MediaSample_t& pair)
    {
      return pair.first == AUDIO_TYPE;
    });
    if (itAudio == m_mediaSamples.end())
    {
#if 0
      VLOG(2) << "No audio in queue to align to";
#endif
      return boost::optional<MediaSample>();
    }
    if (!m_mediaSamples.empty())
    {
      MediaSample_t& mediaSample = m_mediaSamples.front();
      if (mediaSample.first == VIDEO_TYPE)
      {
        MediaSample_t mediaSampleCopy = mediaSample;
        m_mediaSamples.pop_front();
        return boost::optional<MediaSample>(mediaSampleCopy.second);
      }
    }
    return boost::optional<MediaSample>();
#else
    boost::mutex::scoped_lock l(m_videoLock);
    if (!m_videoSamples.empty())
    {
      MediaSample mediaSample = m_videoSamples.front();
#if 1
      MediaSample& lastMediaSample = m_videoSamples.back();
      VLOG(2) << "Getting video " << mediaSample.getPresentationTime() << " Last in queue: " << lastMediaSample.getPresentationTime() << " queue size: " << m_videoSamples.size();
#endif
      m_videoSamples.pop_front();
      return boost::optional<MediaSample>(mediaSample);
    }
    return boost::optional<MediaSample>();
#endif
#endif
  }

  boost::optional<MediaSample> PacketManagerMediaChannel::getAudio()
  {
#ifdef LOCK_FREE
    MediaSample mediaSample;
    if (m_audioSamples.pop(mediaSample))
    {
      return boost::optional<MediaSample>(mediaSample);
    }
    else
    {
      // empty
      return boost::optional<MediaSample>();
    }
#else
#ifdef SINGLE_QUEUE
    boost::mutex::scoped_lock l(m_mediaLock);
    // make sure there is at least one video sample in the queue so we can align the two streams
    auto itVideo = std::find_if(m_mediaSamples.begin(), m_mediaSamples.end(), [](const MediaSample_t& pair)
    {
      return pair.first == VIDEO_TYPE;
    });
    if (itVideo == m_mediaSamples.end())
    {
#if 0
      VLOG(2) << "No video in queue to align to";
#endif
      return boost::optional<MediaSample>();
    }
    if (!m_mediaSamples.empty())
    {
      MediaSample_t& mediaSample = m_mediaSamples.front();
      if (mediaSample.first == AUDIO_TYPE)
      {
        MediaSample_t mediaSampleCopy = mediaSample;
        m_mediaSamples.pop_front();
        return boost::optional<MediaSample>(mediaSampleCopy.second);
      }
    }
    return boost::optional<MediaSample>();
#else
    boost::mutex::scoped_lock l(m_audioLock);
    if (!m_audioSamples.empty())
    {
      MediaSample mediaSample = m_audioSamples.front();
#if 1
      MediaSample& lastMediaSample = m_audioSamples.back();
      VLOG(2) << "Getting audio " << mediaSample.getPresentationTime() << " Last in queue: " << lastMediaSample.getPresentationTime() << " queue size: " << m_audioSamples.size();
#endif
      m_audioSamples.pop_front();
      return boost::optional<MediaSample>(mediaSample);
    }
    return boost::optional<MediaSample>();
#endif
#endif
  }

} //lme

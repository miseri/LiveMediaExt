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

PacketManagerMediaChannel::PacketManagerMediaChannel(uint32_t uiChannelId)
  :MediaChannel(uiChannelId)
{

}

boost::system::error_code PacketManagerMediaChannel::deliverVideo(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples)
{
#ifdef LOCK_FREE
  std::vector<MediaSample>::const_iterator it = m_videoSamples.push(mediaSamples.begin(), mediaSamples.end());
  assert(it == mediaSamples.end());
  return boost::system::error_code();
#else
  boost::mutex::scoped_lock l(m_videoLock);
  m_videoSamples.insert(m_videoSamples.end(), mediaSamples.begin(), mediaSamples.end());
  return boost::system::error_code();
#endif
}

boost::system::error_code PacketManagerMediaChannel::deliverAudio(uint32_t m_uiChannelId, const std::vector<MediaSample>& mediaSamples)
{
  //    fprintf(stderr, "deliverAudio");
#ifdef LOCK_FREE
  std::vector<MediaSample>::const_iterator it = m_audioSamples.push(mediaSamples.begin(), mediaSamples.end());
  assert(it == mediaSamples.end());
  return boost::system::error_code();
#else
  boost::mutex::scoped_lock l(m_audioLock);
  m_audioSamples.insert(m_audioSamples.end(), mediaSamples.begin(), mediaSamples.end());
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
  boost::mutex::scoped_lock l(m_videoLock);
  if (!m_videoSamples.empty())
  {
    MediaSample mediaSample = m_videoSamples.front();
    m_videoSamples.pop_front();
    return boost::optional<MediaSample>(mediaSample);
  }
  return boost::optional<MediaSample>();
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
  boost::mutex::scoped_lock l(m_audioLock);
  if (!m_audioSamples.empty())
  {
    MediaSample mediaSample = m_audioSamples.front();
    m_audioSamples.pop_front();
    return boost::optional<MediaSample>(mediaSample);
  }
  return boost::optional<MediaSample>();
#endif
}

} //lme


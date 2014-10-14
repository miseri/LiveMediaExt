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
#include <map>
#include <vector>
#include <boost/system/error_code.hpp>
#include "BasicUsageEnvironment.hh"
#include <LiveMediaExt/LiveDeviceSource.h>

namespace lme
{

// Forward declarations
class ChannelManager;
class LiveMediaSubsession;

/**
 * @brief A MediaSession_t stores a LiveMediaSubsession per source id (e.g 0 for audio, and 1 for video)
 */
typedef std::map<unsigned, LiveMediaSubsession* > MediaSession_t;
/**
 * @brief A MediaSessionMap_t stores a MediaSession_t per channel
 */
typedef std::map<unsigned, MediaSession_t > MediaSessionMap_t;
//typedef std::vector<LiveMediaSubsession*> LiveMediaSubsessionPtrList_t;

/**
 * @brief The LiveSourceTaskScheduler0 class is aware of the media subsessions that
 * have been created. Each media subsession must register itself on construction
 * and deregister itself on destruction.
 */
class LiveSourceTaskScheduler0 : public BasicTaskScheduler
{
  friend class LiveSourceTaskScheduler0Test;

public:
  /**
   * @brief Destructor
   */
  ~LiveSourceTaskScheduler0();
  /**
   * @brief Overriding this so that we can control the watch variable
   */
	virtual void doEventLoop(char* watchVariable);

	/// measured is in microseconds.
	void setMaximumPollDelay(unsigned uMaxDelayTime) { m_uMaxDelayTime = uMaxDelayTime; }
  /**
   * @brief registers a LiveMediaSubsession with the scheduler
   */
  void addMediaSubsession(uint32_t  uiChannelId, uint32_t uiSourceId, LiveMediaSubsession* pMediaSubsession);
  /**
   * @brief deregisters a LiveMediaSubsession from the scheduler
   */
  void removeMediaSubsession(uint32_t uiChannelId, uint32_t uiSourceId, LiveMediaSubsession* pMediaSubsession);
  /**
   * @brief processes all registered media subsessions
   */
  void processLiveMediaSessions();

protected:
  LiveSourceTaskScheduler0(ChannelManager& channelManager);

private:
  /// Helper method to process media samples within the live555 event loop
  void processLiveSources();

private:
  /// "m_uMaxDelayTime" is in microseconds.
	unsigned m_uMaxDelayTime;
	/// Packet manager that receives media packets from device/network interface
  ChannelManager& m_channelManager;
  /// Map which stores ALL media subsessions. Each subsession is identified via a unique ID
  MediaSessionMap_t m_mMediaSessions;
};

} // lme


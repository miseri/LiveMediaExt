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
// "liveMedia"
// Copyright (c) 1996-2014 Live Networks, Inc.  All rights reserved.
// RTP sink for AMR audio (RFC 4867)
// C++ header

// This code was adapted from the AMRAudioRTPSink.hh to be suitable for live sources.
#pragma once
#include "AMRAudioRTPSink.hh"
#include "LiveAMRAudioDeviceSource.h"

namespace lme
{

class LiveAMRAudioRTPSink : public AMRAudioRTPSink
{
public:

  static LiveAMRAudioRTPSink* createNew(UsageEnvironment& env,
    Groupsock* RTPgs,
    unsigned char rtpPayloadFormat,
    Boolean sourceIsWideband = False,
    unsigned numChannelsInSource = 1);

protected:

  LiveAMRAudioRTPSink(UsageEnvironment& env, Groupsock* RTPgs,
    unsigned char rtpPayloadFormat,
    Boolean sourceIsWideband, unsigned numChannelsInSource);
  // called only by createNew()

  virtual ~LiveAMRAudioRTPSink();

private: // redefined virtual functions:
  virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
  virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
    unsigned char* frameStart,
    unsigned numBytesInFrame,
  struct timeval frameTimestamp,
    unsigned numRemainingBytes);
  virtual Boolean  frameCanAppearAfterPacketStart(unsigned char const* frameStart,
				 unsigned numBytesInFrame) const;
  virtual unsigned frameSpecificHeaderSize() const;
  virtual unsigned specialHeaderSize() const;


};

} // lme


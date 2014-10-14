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
// Implementation

// NOTE: At present, this is just a limited implementation, supporting:
// octet-alignment only; no interleaving; no frame CRC; no robust-sorting.

// NOTE: This code was adapted from the AMRAudioRTPSink.hh to be suitable for live sources.
#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveAMRAudioRTPSink.h>

namespace lme
{

LiveAMRAudioRTPSink* LiveAMRAudioRTPSink::createNew( UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat, Boolean sourceIsWideband /*= False*/, unsigned numChannelsInSource /*= 1*/ )
{
  return new LiveAMRAudioRTPSink(env, RTPgs, rtpPayloadFormat,
    sourceIsWideband, numChannelsInSource);
}

LiveAMRAudioRTPSink::LiveAMRAudioRTPSink( UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat, Boolean sourceIsWideband, unsigned numChannelsInSource )
:AMRAudioRTPSink(env, RTPgs, rtpPayloadFormat, sourceIsWideband, numChannelsInSource)
{
  
}

LiveAMRAudioRTPSink::~LiveAMRAudioRTPSink()
{

}

Boolean LiveAMRAudioRTPSink::sourceIsCompatibleWithUs( MediaSource& source )
{
  // Our source must be an AMR audio source:
  if (!source.isAMRAudioSource()) return False;

  // Also, the source must be wideband iff we asked for this:
  LiveAMRAudioDeviceSource& amrSource = (LiveAMRAudioDeviceSource&)source;
  if ((amrSource.isWideband()^sourceIsWideband()) != 0) return False;

  // Also, the source must have the same number of channels that we
  // specified.  (It could, in principle, have more, but we don't
  // support that.)
  if (amrSource.numChannels() != numChannels()) return False;

  // Also, because in our current implementation we output only one
  // frame in each RTP packet, this means that for multi-channel audio,
  // each 'frame-block' will be split over multiple RTP packets, which
  // may violate the spec.  Warn about this:
  if (amrSource.numChannels() > 1) {
    envir() << "AMRAudioRTPSink: Warning: Input source has " << amrSource.numChannels()
      << " audio channels.  In the current implementation, the multi-frame frame-block will be split over multiple RTP packets\n";
  }

  return True;
}

void LiveAMRAudioRTPSink::doSpecialFrameHandling( unsigned fragmentationOffset, unsigned char* frameStart, unsigned numBytesInFrame, struct timeval frameTimestamp, unsigned numRemainingBytes )
{
#define AUDIO_FRAMES_PER_RTP_FRAME 5

  // If this is the 1st frame in the 1st packet, set the RTP 'M' (marker)
  // bit (because this is considered the start of a talk spurt):
  if (isFirstPacket() && isFirstFrameInPacket()) {
    setMarkerBit();
  }

  // If this is the first frame in the packet, set the 1-byte payload
  // header (using CMR 15)
  if (isFirstFrameInPacket()) {
    u_int8_t payloadHeader = 0xF0;
    setSpecialHeaderBytes(&payloadHeader, 1, 0);
  }

  // Set the TOC field for the current frame, based on the "FT" and "Q"
  // values from our source:
  LiveAMRAudioDeviceSource* amrSource = (LiveAMRAudioDeviceSource*)fSource;
  if (amrSource == NULL) return; // sanity check

  u_int8_t toc = amrSource->lastFrameHeader();
  if (numFramesUsedSoFar() < AUDIO_FRAMES_PER_RTP_FRAME - 1) {
	  toc |= 0x80;
  }
  else {
	  toc &=~ 0x80;
  }
  setSpecialHeaderBytes(&toc, 1, 1+numFramesUsedSoFar());
  
  // Removing toc in custom AMR device source
  // update packet toc?
  
  // Important: Also call our base class's doSpecialFrameHandling(),
  // to set the packet's timestamp:
  MultiFramedRTPSink::doSpecialFrameHandling(fragmentationOffset,
    frameStart, numBytesInFrame,
    frameTimestamp,
    numRemainingBytes);
}

Boolean LiveAMRAudioRTPSink::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/, unsigned /*numBytesInFrame*/) const {
	return numFramesUsedSoFar() < AUDIO_FRAMES_PER_RTP_FRAME;

}

unsigned LiveAMRAudioRTPSink::frameSpecificHeaderSize() const {
  return 0;
}

unsigned LiveAMRAudioRTPSink::specialHeaderSize() const {
  return 1 + AUDIO_FRAMES_PER_RTP_FRAME;
}

} // lme



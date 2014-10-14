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
#include <Media/MediaSample.h>

#ifndef BYTE
	typedef unsigned char BYTE;
#endif

namespace lme
{

/// This class abstracts the multiple buffers that a (multiplexed) sample gets copied into
/// The interface tries to cater for both media samples where there is only one channel such as PCM or H263
/// As well as our custom multiplexed H263 data type
class IMediaSampleBuffer
{
public:
  IMediaSampleBuffer()
  {;}

  virtual ~IMediaSampleBuffer()
  {;}

#pragma chMSG(To be refactored)
  virtual void addMediaSample(const MediaSample& mediaSample) = 0;
  virtual unsigned getCurrentChannel()                        = 0;
  virtual void setCurrentChannel(unsigned uiChannel)          = 0;
  virtual BYTE* getCurrentBuffer()                            = 0;
  virtual double getCurrentStartTime()                        = 0;
  virtual unsigned getCurrentSize()                           = 0;
  virtual unsigned getNumberOfChannels()                      = 0;
  virtual BYTE* getBufferAt(unsigned nIndex)                  = 0;
};

} // lme




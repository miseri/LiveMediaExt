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
#include <Media/MediaSampleStorageBuffer.h>

namespace lme
{

class IFrameGrabber
{
public:
	// NB: This class does NOT take ownership of the sample buffer!!!! That belongs to the media subsession
	IFrameGrabber(IMediaSampleBuffer* pSampleBuffer)
		:m_pSampleBuffer(pSampleBuffer)
	{;}

	virtual ~IFrameGrabber()
	{;}

	/// The getNextFrame method should return NULL if there's no data!!!
	virtual BYTE* getNextFrame(unsigned& uiSize, double& dStartTime) = 0;

protected:
	IMediaSampleBuffer* m_pSampleBuffer;
};

} // lme




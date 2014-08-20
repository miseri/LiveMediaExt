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




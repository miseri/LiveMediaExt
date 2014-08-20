#pragma once
#include <Media/FrameGrabber.h>

namespace lme
{

class SimpleFrameGrabber : public IFrameGrabber
{
public:
	SimpleFrameGrabber(IMediaSampleBuffer* pSampleBuffer)
		:IFrameGrabber(pSampleBuffer)
	{;}

	~SimpleFrameGrabber(){;}

	BYTE* getNextFrame(unsigned& uiSize, double& dStartTime)
	{
		uiSize = m_pSampleBuffer->getCurrentSize();
		dStartTime = m_pSampleBuffer->getCurrentStartTime();
		return m_pSampleBuffer->getCurrentBuffer();
	}
};

} // lme






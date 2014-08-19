#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveDeviceSource.h>
#include "GroupsockHelper.hh"
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <artist/Media/FrameGrabber.h>
//#include <artist/Media/SimpleFrameGrabber.h>
//#include <artist/LiveMedia/LiveMediaSubsession.h>
//#include <artist/LiveMedia/LiveRtvcRtpSink.h>

namespace lme
{

LiveDeviceSource* LiveDeviceSource::createNew(UsageEnvironment& env, unsigned uiClientId/* , RtvcLiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer*/)
{
  // When constructing a 'simple' LiveDeviceSource we'll just create a simple frame grabber
  LiveDeviceSource* pSource = new LiveDeviceSource(env, uiClientId/*, pParent, new SimpleFrameGrabber(pSampleBuffer)*/);
  return pSource;
}

LiveDeviceSource::LiveDeviceSource(UsageEnvironment& env, unsigned uiClientId/*, RtvcLiveMediaSubsession* pParent, IFrameGrabber* pFrameGrabber*/)
	: FramedSource(env),
  m_env(env),
  m_uiClientId(uiClientId)//,
	//m_pParentSubsession(pParent),
	//m_pFrameGrabber(pFrameGrabber),
	//m_bOffsetSet(false),
	//m_pSink(NULL),
 // m_bSourceUpdateOccurred(false),
 // m_bIsPlaying(false)
{

	//pParent->addDeviceSource(this);

}

LiveDeviceSource::~LiveDeviceSource(void)
{
	//m_pParentSubsession->removeDeviceSource(this);
 // if (m_pFrameGrabber) delete m_pFrameGrabber ; m_pFrameGrabber  = NULL;
}

void LiveDeviceSource::doGetNextFrame()
{
  //// Arrange here for our "deliverFrame" member function to be called
  //// when the next frame of data becomes available from the device.
  //// This must be done in a non-blocking fashion - i.e., so that we
  //// return immediately from this function even if no data is
  //// currently available.
  ////
  //// If the device can be implemented as a readable socket, then one easy
  //// way to do this is using a call to
  ////     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  //// (See examples of this call in the "liveMedia" directory.)
  //// Check availability
  //if ( m_qMediaSamples.empty() )
  //{
  //  m_bIsPlaying = false;
  //  return;
  //}

  //// We have had a sample
  //m_bIsPlaying = true;

  //// Deliver frame
  //deliverFrame();
}

// method to add data to the device
bool LiveDeviceSource::retrieveMediaSampleFromBuffer()
{
//	unsigned uiSize = 0;
//	double dStartTime = 0.0;
//	BYTE* pBuffer = m_pFrameGrabber->getNextFrame(uiSize, dStartTime);
//	
//	// Make sure there's data, the frame grabber should return null if it doesn't have any
//	if (!pBuffer)
//	{
//#if 0
//  VLOG(15) << "NULL Sample retrieved from frame grabber";
//#endif
//		return false;
//	}
//	else
//	{
//		MediaSampleEx2* pSample = new MediaSampleEx2(pBuffer, uiSize, dStartTime);
//		m_qMediaSamples.push(pSample);
//    return true;
//	}
  return false;
}

void LiveDeviceSource::deliverFrame()
{
  VLOG(15) << "Called";

  // This would be called when new frame data is available from the device.
  // This function should deliver the next frame of data from the device,
  // using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

  //assert( !m_qMediaSamples.empty() );

  //// Deliver the data here:
  //MediaSampleEx2* pMediaSample = m_qMediaSamples.front();
  //m_qMediaSamples.pop();

  //double dStartTime = pMediaSample->getStartTime();
  //int nSize = pMediaSample->getSize();
  //const BYTE* pBuffer = pMediaSample->getMediaData()->getData();
  //// The start time of the first sample is stored as a reference start time for the media samples
  //// Similarly we store the current time obtained by gettimeofday in m_tOffsetTime.
  //// The reason for this is that we need to start timestamping the samples with timestamps starting at gettimeofday
  //// This is a requirement that allows us to receive the original timestamp on the RTP client side.
  //// The reason is that the RTCP implementation starts running using gettimeofday.
  //// If this is not done, samples on the receiver side have really strange time stamps
  //if (!m_bOffsetSet)
  //{
  //  // Set initial values
  //  gettimeofday(&m_tOffsetTime, NULL);
  //  m_dOffsetTime = dStartTime;
  //  m_bOffsetSet = true;

  //  // Set the presentation time of the first sample
  //  gettimeofday(&fPresentationTime, NULL);

  //  VLOG(15) << "Delivering first media frame";
  //}
  //else
  //{
  //    VLOG(15) << "Delivering next media frame";

  //  // Calculate the difference between this samples start time and the initial samples start time
  //  double dDifference = dStartTime - m_dOffsetTime;
  //  long lDiffSecs = (long)dDifference;
  //  long lDiffUSecs = static_cast<long>((dDifference - lDiffSecs) * 1000000);
  //  // Now add these offsets to the initial presentation time obtained through gettimeofday
  //  fPresentationTime.tv_sec = m_tOffsetTime.tv_sec + lDiffSecs;
  //  fPresentationTime.tv_usec = m_tOffsetTime.tv_usec + lDiffUSecs;
  //}

  //if (nSize > (int)fMaxSize)
  //{
  //  // TODONB
  //  //TOREVISE/TODO
  //  fNumTruncatedBytes = nSize -  fFrameSize;
  //  fFrameSize = fMaxSize;
  //  //TODO How do we send the rest in the following packet???
  //  //TODO How do we delete the frame??? Unless we store extra attributes in the MediaFrame class
  //  LOG(WARNING) << "TODO: Truncated packet";
  //}
  //else
  //{
  //  fFrameSize = nSize;
  //  memcpy(fTo, pBuffer, fFrameSize);
  //  // Testing with current time of day
  //  //gettimeofday(&fPresentationTime, NULL);
  //  // 04/04/2008 RG: http://lists.live555.com/pipermail/live-devel/2008-April/008395.html
  //  //Testing with 'live' config
  //  fDurationInMicroseconds = 0;
  //}

  //// free the memory of the sample
  //delete pMediaSample;

  //// After delivering the data, inform the reader that it is now available:
  //FramedSource::afterGetting(this);
}

void LiveDeviceSource::doDeliverFrame( void* pInstance )
{
	LiveDeviceSource* pSource = (LiveDeviceSource*)pInstance;
	if (pSource)
	{
		pSource->deliverFrame();
	}
}

} // lme


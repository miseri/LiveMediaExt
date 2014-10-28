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
#include <LiveMediaExt/LiveDeviceSource.h>
#include "GroupsockHelper.hh"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Media/FrameGrabber.h>
#include <Media/IRateAdaptation.h>
#include <Media/IRateAdaptationFactory.h>
#include <Media/IRateController.h>
#include <Media/SimpleFrameGrabber.h>
#include <LiveMediaExt/LiveMediaSubsession.h>

namespace lme
{

LiveDeviceSource* LiveDeviceSource::createNew(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
  IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory, IRateController* pRateControl)
{
  // When constructing a 'simple' LiveDeviceSource we'll just create a simple frame grabber
  LiveDeviceSource* pSource = new LiveDeviceSource(env, uiClientId, pParent, new SimpleFrameGrabber(pSampleBuffer), pRateAdaptationFactory, pRateControl);
  return pSource;
}

LiveDeviceSource::LiveDeviceSource(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
  IFrameGrabber* pFrameGrabber, IRateAdaptationFactory* pRateAdaptationFactory, IRateController* pRateControl)
	: FramedSource(env),
  m_env(env),
  m_uiClientId(uiClientId),
	m_pParentSubsession(pParent),
	m_pFrameGrabber(pFrameGrabber),
	m_bOffsetSet(false),
  m_dOffsetTime(0.0),
	m_pSink(NULL),
  m_bIsPlaying(false),
  m_pRateAdaptationFactory(pRateAdaptationFactory),
  m_pRateAdaptation(NULL),
  m_pRateControl(pRateControl),
  m_uiLastPacketNumReceived(0)
{
  VLOG(10) << "Constructor: adding device source to parent subsession";
  m_pParentSubsession->addDeviceSource(this);
  if (m_pRateAdaptationFactory)
  {
    m_pRateAdaptation = m_pRateAdaptationFactory->getInstance();
  }
}

LiveDeviceSource::~LiveDeviceSource(void)
{
  VLOG(10) << "Destructor: removing device source from parent subsession";
  m_pParentSubsession->removeDeviceSource(this);
  if (m_pFrameGrabber) delete m_pFrameGrabber ; m_pFrameGrabber  = NULL;
  if (m_pRateAdaptation)
  {
    assert(m_pRateAdaptationFactory);
    m_pRateAdaptationFactory->releaseInstance(m_pRateAdaptation);
  }

  // NB: TODO: Rate control not necessarily owned by source?!?
  // This is especially the case with one non-switching codec per multiple clients
  //if (m_pRateControl)
  //{
  //  delete m_pRateControl;
  //}
}

void LiveDeviceSource::doGetNextFrame()
{
  //  VLOG(2) << "LiveDeviceSource::doGetNextFrame()";
  // Arrange here for our "deliverFrame" member function to be called
  // when the next frame of data becomes available from the device.
  // This must be done in a non-blocking fashion - i.e., so that we
  // return immediately from this function even if no data is
  // currently available.
  //
  // If the device can be implemented as a readable socket, then one easy
  // way to do this is using a call to
  //     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  // (See examples of this call in the "liveMedia" directory.)
  // Check availability
  if ( m_qMediaSamples.empty() )
  {
    m_bIsPlaying = false;
    return;
  }

  // We have had a sample
  m_bIsPlaying = true;

  // Deliver frame
  deliverFrame();
}

// method to add data to the device
bool LiveDeviceSource::retrieveMediaSampleFromBuffer()
{
  //  VLOG(2) << "LiveDeviceSource::retrieveMediaSampleFromBuffer()";
  unsigned uiSize = 0;
	double dStartTime = 0.0;
	BYTE* pBuffer = m_pFrameGrabber->getNextFrame(uiSize, dStartTime);
	
	// Make sure there's data, the frame grabber should return null if it doesn't have any
	if (!pBuffer)
	{
#if 0
  VLOG(15) << "NULL Sample retrieved from frame grabber";
#endif
		return false;
	}
	else
	{
    // TODO: can we get rid of this copy and just use the pointer?!?
    MediaSample mediaSample;
    mediaSample.setPresentationTime(dStartTime);
    BYTE* pData = new uint8_t[uiSize];
    memcpy(pData, pBuffer, uiSize);
    mediaSample.setData(Buffer(pData, uiSize));
    m_qMediaSamples.push_back(mediaSample);
    return true;
	}
  return false;
}

void LiveDeviceSource::deliverFrame()
{
  //  VLOG(2) << "LiveDeviceSource::deliverFrame()";

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

  assert( !m_qMediaSamples.empty() );

  // Deliver the data here:
  MediaSample mediaSample = m_qMediaSamples.front();
  m_qMediaSamples.pop_front();

  double dStartTime = mediaSample.getPresentationTime();
  int nSize = mediaSample.getMediaSize();
  const BYTE* pBuffer = mediaSample.getDataBuffer().data();
  //  VLOG(2) << "LiveDeviceSource::deliverFrame() Sample size: " << nSize;
  // The start time of the first sample is stored as a reference start time for the media samples
  // Similarly we store the current time obtained by gettimeofday in m_tOffsetTime.
  // The reason for this is that we need to start timestamping the samples with timestamps starting at gettimeofday
  // This is a requirement that allows us to receive the original timestamp on the RTP client side.
  // The reason is that the RTCP implementation starts running using gettimeofday.
  // If this is not done, samples on the receiver side have really strange time stamps
  if (!m_bOffsetSet)
  {
    // Set initial values
    gettimeofday(&m_tOffsetTime, NULL);
    m_dOffsetTime = dStartTime;
    m_bOffsetSet = true;

    // Set the presentation time of the first sample
    gettimeofday(&fPresentationTime, NULL);

    //    VLOG(2) << "Delivering first media frame";
  }
  else
  {
    //    VLOG(2) << "Delivering next media frame";

    // Calculate the difference between this samples start time and the initial samples start time
    double dDifference = dStartTime - m_dOffsetTime;
    long lDiffSecs = (long)dDifference;
    long lDiffUSecs = static_cast<long>((dDifference - lDiffSecs) * 1000000);
    // Now add these offsets to the initial presentation time obtained through gettimeofday
    fPresentationTime.tv_sec = m_tOffsetTime.tv_sec + lDiffSecs;
    fPresentationTime.tv_usec = m_tOffsetTime.tv_usec + lDiffUSecs;
  }

  if (nSize > (int)fMaxSize)
  {
    // TODONB
    //TOREVISE/TODO
    fNumTruncatedBytes = nSize -  fFrameSize;
    fFrameSize = fMaxSize;
    //TODO How do we send the rest in the following packet???
    //TODO How do we delete the frame??? Unless we store extra attributes in the MediaFrame class
    LOG(WARNING) << "TODO: Truncated packet";
  }
  else
  {
    fFrameSize = nSize;
    memcpy(fTo, pBuffer, fFrameSize);
    // Testing with current time of day
    //gettimeofday(&fPresentationTime, NULL);
    // 04/04/2008 RG: http://lists.live555.com/pipermail/live-devel/2008-April/008395.html
    //Testing with 'live' config
    fDurationInMicroseconds = 0;
  }

  // After delivering the data, inform the reader that it is now available:
  FramedSource::afterGetting(this);
}

void LiveDeviceSource::doDeliverFrame( void* pInstance )
{
	LiveDeviceSource* pSource = (LiveDeviceSource*)pInstance;
	if (pSource)
	{
		pSource->deliverFrame();
	}
}

void LiveDeviceSource::processReceiverReports()
{
  VLOG(15) << "LiveDeviceSource::processReceiverReports()";
  if (m_pRateAdaptation && m_pSink)
  {
    RTPTransmissionStatsDB::Iterator statsIter(m_pSink->transmissionStatsDB());
    RTPTransmissionStats* stats = statsIter.next();
    if (stats != NULL)
    {
      if (m_uiLastPacketNumReceived == stats->lastPacketNumReceived())
      {
        VLOG(15) << "LiveDeviceSource::processReceiverReports(): no new info";
        // no new info
        return;
      }
      m_uiLastPacketNumReceived = stats->lastPacketNumReceived();
      // convert RtpTransmissionStats to our format
      RtpTransmissionStats rtpStats;
      rtpStats.LastPacketNumReceived = stats->lastPacketNumReceived();
      rtpStats.FirstPacketNumReported = stats->firstPacketNumReported();
      rtpStats.TotalPacketsLost = stats->totNumPacketsLost();
      rtpStats.Jitter = stats->jitter();
      rtpStats.LastSrTime = stats->lastSRTime();
      rtpStats.DiffSrRr = stats->diffSR_RRTime();
      rtpStats.Rtt = stats->roundTripDelay();

      VLOG(2) << "Updating receiver stats: Last packet num received: " << m_uiLastPacketNumReceived;
      lme::SwitchDirection eRateAdvice = m_pRateAdaptation->getRateAdaptAdvice(rtpStats);

      // get hold of rate control interface
      // If we switch for a single source - single client, this will be ok.
      // In the case of multiple clients being connected, this will screw up the rate control as there is currently
      // only one encoder in the media pipeline.
      if (m_pRateControl)
      {
        m_pRateControl->controlBitrate(eRateAdvice);
      }
    }
    else
    {
      VLOG(15) << "LiveDeviceSource::processReceiverReports(): stats null";
    }
  }
}

} // lme


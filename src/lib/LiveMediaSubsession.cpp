#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveMediaSubsession.h>
#include <cassert>
#include <iterator>
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "liveMedia.hh"
#include <LiveMediaExt/LiveAMRAudioDeviceSource.h>
#include <LiveMediaExt/LiveDeviceSource.h>
#include <LiveMediaExt/LiveH264VideoDeviceSource.h>
#include <LiveMediaExt/LiveSourceTaskScheduler.h>
#include <LiveMediaExt/LiveRtspServer.h>
#include <Media/MediaSampleStorageBuffer.h>
#include <Media/MediaTypes.h>
// #include <Media/ParallelMediaSampleBuffer.h>
#include <Media/SingleMediaSampleBuffer.h>

// We don't want to reuse the first source as one usually would for "live" liveMedia sources
// In our case we want a separate device source per client so that we can control the switching on a per client basis
#define REUSE_FIRST_SOURCE false

namespace lme
{

LiveMediaSubsession::LiveMediaSubsession( UsageEnvironment& env, LiveRtspServer& rParent, 
                                                  const unsigned uiChannelId, unsigned uiSourceId, 
                                                  const std::string& sSessionName, 
                                                  bool bVideo, const unsigned uiTotalChannels)
	:OnDemandServerMediaSubsession(env, REUSE_FIRST_SOURCE),
  m_rRtspServer(rParent),
	m_uiChannelId(uiChannelId),
  m_uiSourceID(uiSourceId),
  m_sSessionName(sSessionName),
  m_bVideo(bVideo),
  m_uiTotalChannels(uiTotalChannels),
  m_pSampleBuffer(NULL)
{
#ifdef RTVC_LOG_CONSTRUCTION
  VLOG(15) << "Constructor Id: " << uiChannelId << ":" << m_uniqueSessionID << " " << m_sSessionName << " " << m_sSubsessionName);
#endif

	TaskScheduler* pScheduler = &(envir().taskScheduler());
	LiveSourceTaskScheduler* pLiveScheduler = dynamic_cast<LiveSourceTaskScheduler*>(pScheduler);
  if (pLiveScheduler)
	{
    pLiveScheduler->addMediaSubsession(m_uiChannelId, m_uiSourceID, this);
	}

  // create sample buffer according to number of 'switchable' channels
  assert(m_uiTotalChannels > 0);
  if (m_uiTotalChannels > 1)
    // TODO
    ;
    // m_pSampleBuffer = new ParallelMediaSampleBuffer(m_uiTotalChannels);
  else
    m_pSampleBuffer = new SingleMediaSampleBuffer();

  assert(m_pSampleBuffer);
}

LiveMediaSubsession::~LiveMediaSubsession()
{
#ifdef RTVC_LOG_CONSTRUCTION
  VLOG(15) << "Destructor Id: " << m_uiChannelId << ":" << m_uniqueSessionID << " " << m_sSessionName << " " << m_sSubsessionName;
#endif

	// Deregister from Scheduler
	TaskScheduler* pScheduler = &(envir().taskScheduler());
	// TOREVISE: replace dynamic cast here with static since we know that's always the case
	LiveSourceTaskScheduler* pPollingScheduler = dynamic_cast<LiveSourceTaskScheduler*>(pScheduler);
	if (pPollingScheduler)
	{
		pPollingScheduler->removeMediaSubsession(m_uiChannelId, m_uiSourceID, this);
	}

  if ( m_pSampleBuffer ) delete m_pSampleBuffer; m_pSampleBuffer = NULL;
}

void LiveMediaSubsession::addMediaSample( const MediaSample& mediaSample )
{
  assert (m_pSampleBuffer);

  // Add the sample to the buffer where it will be parsed
  m_pSampleBuffer->addMediaSample(mediaSample);
  // Iterate over all device sources and deliver the samples to the clients
  for (LiveDeviceSourcePtrList_t::iterator it = m_vDeviceSources.begin(); it != m_vDeviceSources.end(); it++)
  {
    bool bGotFrame = (*it)->retrieveMediaSampleFromBuffer();

    if (bGotFrame)
    {
      if (!(*it)->isPlaying())
      {
        (*it)->deliverFrame();
      }
    }
  }
}

FramedSource* LiveMediaSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
  assert(m_pSampleBuffer);
  // delegating creation call to subclasses of LiveMediaSubsession
  LiveDeviceSource* pSource = createSubsessionSpecificSource(clientSessionId, m_pSampleBuffer);
  // make sure subclasses were able to construct source
  assert(pSource);
  // Call virtual subclass method to set estimated bit rate
  setEstimatedBitRate(estBitrate);
  return pSource;
}

void LiveMediaSubsession::addDeviceSource( LiveDeviceSource* pDeviceSource )
{
	m_vDeviceSources.push_back(pDeviceSource);
}

void LiveMediaSubsession::removeDeviceSource( LiveDeviceSource* pDeviceSource )
{
	for (LiveDeviceSourcePtrList_t::iterator it = m_vDeviceSources.begin(); it != m_vDeviceSources.end(); it++)
	{
		if (pDeviceSource == (*it))
		{
			m_vDeviceSources.erase(it);
			break;
		}
	}
}

void LiveMediaSubsession::deleteStream( unsigned clientSessionId, void*& streamToken )
{
	// Call super class method
	OnDemandServerMediaSubsession::deleteStream(clientSessionId, streamToken);

  // TODO: could handle client management here
}

void LiveMediaSubsession::getStreamParameters( unsigned clientSessionId, netAddressBits clientAddress, Port const& clientRTPPort, Port const& clientRTCPPort, int tcpSocketNum, unsigned char rtpChannelId, unsigned char rtcpChannelId, netAddressBits& destinationAddress, u_int8_t& destinationTTL, Boolean& isMulticast, Port& serverRTPPort, Port& serverRTCPPort, void*& streamToken )
{
	if (destinationAddress == 0) destinationAddress = clientAddress;
	struct in_addr destinationAddr; destinationAddr.s_addr = destinationAddress;

  // TODO: could handle client management here
	OnDemandServerMediaSubsession::getStreamParameters( clientSessionId, clientAddress, clientRTPPort, clientRTCPPort, tcpSocketNum, rtpChannelId, 
		rtcpChannelId, destinationAddress, destinationTTL, isMulticast, serverRTPPort, serverRTCPPort, streamToken);
}

} // lme



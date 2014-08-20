#pragma once
#include <queue>
#include <string>
#include "FramedSource.hh"
#include "RTPSink.hh"
#include <Media/MediaSample.h>

const int DIRECT_SHOW_TOLIVE_TIME_FACTOR = 10;

namespace lme
{

class IMediaSampleBuffer;
class IFrameGrabber;
class LiveMediaSubsession;
class LiveRtvcRtpSink;

/**
 * @brief The LiveDeviceSource class allows the insertion of media samples into the liveMedia stack.
 *
 * While the live media library is single-threaded it is often necessary to obtain data from a live
 * source or framework which has it's own event loop. The LiveDeviceSource is based on the live555
 * DeviceSource class and comprises a mechanism to get data into the liveMedia stack for delivery.
 * 
 * NOTE: we do not use the provided trigger mechanism as these extensions are being designed for a 
 * use case in which each RTPSink may receive media of a different quality. For this reason, we can
 * not use the reuseFirstSource option provided by OnDemandServerMediaSession. This results in a 
 * LiveDeviceSource being created per RTSP client. The trigger mechanism in the BasicTaskScheduler
 * is designed for up to MAX_NUM_EVENT_TRIGGERS=32 triggers making the trigger mechanism incompatible
 * with our use case.
 */
class LiveDeviceSource : public FramedSource
{
public:
  /**
   * @brief Named constructor
   */
  static LiveDeviceSource* createNew(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, IMediaSampleBuffer* pSampleBuffer);
  /**
   * @brief Destructor
   */
	virtual ~LiveDeviceSource(void);
  /**
   * @brief Getter for unique client Id
   */  
  unsigned getClientId() const { return m_uiClientId; }
	/**
   * @brief Static method used in live555 callback mechanism
   */
  static void doDeliverFrame(void* pInstance);
  /**
   * @brief
   */
  void deliverFrame();

	//// method to add data to the device
  
  /// returns true if a frame is retrieved from the buffer
	virtual bool retrieveMediaSampleFromBuffer();

  // RTPSink* getRTPSink() { return m_pSink; }
  // void setRTPSink(RTPSink* pSink) { m_pSink = pSink; }

  bool isPlaying() const { return m_bIsPlaying; }

 // // HACK: method that should show if the device source was updated. Used for notification
 // bool sourceUpdateOccurred() const { return m_bSourceUpdateOccurred; }

protected:
  LiveDeviceSource(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, IFrameGrabber* pFrameGrabber);

  // redefined virtual functions:
  virtual void doGetNextFrame();

protected:

  /// live555 environment
  UsageEnvironment& m_env;
  /// unique client id
  unsigned m_uiClientId;

  LiveMediaSubsession* m_pParentSubsession;
  IFrameGrabber* m_pFrameGrabber;
	// queue for outgoing samples
	std::queue<MediaSample> m_qMediaSamples;
//
//	/// Related RTP Sink
//	//LiveRtvcRtpSink* m_pSink;
//  RTPSink* m_pSink;
//  // HACK for client quality updates
//  // Needs to be refactored
//  // Using this var in SwitchingDeviceSource: if the quality switches this flag is set so that 
//  // the ClientSessionManager can be notified of the change in quality.
//  // This hack is required to get DESCRIBE_CLIENTS to work
//  bool m_bSourceUpdateOccurred;
//
//private:
	/// Start time offsets
	bool m_bOffsetSet;
	struct timeval m_tOffsetTime;
	double m_dOffsetTime;

  bool m_bIsPlaying;
};

} // lme


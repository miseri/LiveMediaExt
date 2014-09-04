#pragma once
#include <deque>
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
class IRateAdaptationFactory;
class IRateAdaptation;
class IRateController;

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
  static LiveDeviceSource* createNew(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
    IMediaSampleBuffer* pSampleBuffer, IRateAdaptationFactory* pRateAdaptationFactory, IRateController* pRateControl);
  /**
   * @brief Destructor
   */
	virtual ~LiveDeviceSource();
  /**
   * @brief Getter for unique client Id
   */  
  unsigned getClientId() const { return m_uiClientId; }
	/**
   * @brief Static method used in live555 callback mechanism
   * to call deliverFrame()
   */
  static void doDeliverFrame(void* pInstance);
  /**
   * @brief delivers frame into the live555 pipeline
   */
  void deliverFrame();  
  /**
   * @brief retrieves a sample from the media buffer
   * @return returns true if a frame is retrieved from the live source buffer
   */ 
	virtual bool retrieveMediaSampleFromBuffer();
  /**
   * @brief Can be called periodically to process receiver reports
   */
  void processReceiverReports();
  /**
   * @brief getter for RTPSink.
   */
  RTPSink* getRTPSink() { return m_pSink; }
  /**
   * @brief Setter for RTPSink
   */
  void setRTPSink(RTPSink* pSink) { m_pSink = pSink; }

  bool isPlaying() const { return m_bIsPlaying; }

 // // HACK: method that should show if the device source was updated. Used for notification
 // bool sourceUpdateOccurred() const { return m_bSourceUpdateOccurred; }

protected:
  LiveDeviceSource(UsageEnvironment& env, unsigned uiClientId, LiveMediaSubsession* pParent, 
                   IFrameGrabber* pFrameGrabber, IRateAdaptationFactory* pRateAdaptationFactory, IRateController* pRateControl);

  // redefined virtual functions:
  virtual void doGetNextFrame();

protected:

  /// live555 environment
  UsageEnvironment& m_env;
  /// unique client id
  unsigned m_uiClientId;
  /// Media subsession
  LiveMediaSubsession* m_pParentSubsession;
  /// Framegrabber
  IFrameGrabber* m_pFrameGrabber;
	// queue for outgoing samples
	std::deque<MediaSample> m_qMediaSamples;
//	/// Related RTP Sink
//	//LiveRtvcRtpSink* m_pSink;
  /// live555 RTP sink
  RTPSink* m_pSink;
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
  /// Factory to get optional IRateAdaptation
  IRateAdaptationFactory* m_pRateAdaptationFactory;
  /// IRateAdaptation
  IRateAdaptation* m_pRateAdaptation;
  /// Rate control class
  IRateController* m_pRateControl;
  /// used to determine if RTP transmission stats have new information
  uint32_t m_uiLastPacketNumReceived;
};

} // lme


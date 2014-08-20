#pragma once
#include "LiveSourceTaskScheduler0.h"

namespace lme
{

/**
 * @brief Live media task scheduler that also handles live sources
 */
class LiveSourceTaskScheduler: public LiveSourceTaskScheduler0 {
public:
  /**
   * @brief Named constructor
   */
  static LiveSourceTaskScheduler* createNew(ChannelManager& channelManager);
  /**
   * @brief Destructor
   */
	virtual ~LiveSourceTaskScheduler();

protected:
  /**
   * @brief Constructor
   *
   *	called only by "createNew()"
   */
  LiveSourceTaskScheduler(ChannelManager& channelManager);

protected:
	/**
   * @brief Redefined virtual function
   */
	virtual void SingleStep(unsigned maxDelayTime);

};

} // lme


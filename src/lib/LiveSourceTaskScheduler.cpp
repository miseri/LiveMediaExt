#include "LiveMediaExtPch.h"
#include <LiveMediaExt/LiveSourceTaskScheduler.h>

namespace lme
{

LiveSourceTaskScheduler* LiveSourceTaskScheduler::createNew(ChannelManager& channelManager)
{
	return new LiveSourceTaskScheduler(channelManager);
}

LiveSourceTaskScheduler::LiveSourceTaskScheduler(ChannelManager& channelManager)
  :LiveSourceTaskScheduler0(channelManager)
{

}

LiveSourceTaskScheduler::~LiveSourceTaskScheduler()
{

}

void LiveSourceTaskScheduler::SingleStep( unsigned maxDelayTime )
{
	BasicTaskScheduler::SingleStep(maxDelayTime);
}

} // lme


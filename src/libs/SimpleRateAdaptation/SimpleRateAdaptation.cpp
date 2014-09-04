#include "SimpleRateAdaptationPch.h"
#include "SimpleRateAdaptation.h"

namespace lme
{

SimpleRateAdaptation::SimpleRateAdaptation()
{

}

SwitchDirection SimpleRateAdaptation::getRateAdaptAdvice(const RtpTransmissionStats& stats)
{
  VLOG(2) << "Rate adaptation received: LastPacketNumReceived: " << stats;
  // NOOP
  return STAY;
}

} // lme

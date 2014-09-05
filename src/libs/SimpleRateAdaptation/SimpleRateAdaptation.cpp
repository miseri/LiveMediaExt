#include "SimpleRateAdaptationPch.h"
#include "SimpleRateAdaptation.h"

namespace lme
{

SimpleRateAdaptation::SimpleRateAdaptation()
  :m_dPreviousAverage(-1.0),
  m_lastPacketNumReceived(0),
  nCounter(0),
  nCurrentChannel(0),
  bUp(true)
{

}

lme::SwitchDirection SimpleRateAdaptation::getRateAdaptAdvice(const lme::RtpTransmissionStats& stats)
{
  // Get packet num
  unsigned packetNum = stats.LastPacketNumReceived;
  if (packetNum == m_lastPacketNumReceived)
  {
    // There's no new information
    VLOG(5) << "Staying: no new info";
    return lme::STAY;
  }

  lme::SwitchDirection eSwitchDirection = lme::STAY;

  if (bUp)
  {
    ++nCurrentChannel;
    VLOG(5) << "Switching up";
    eSwitchDirection = lme::SWITCH_UP;
    if (nCurrentChannel == 7)
      bUp = false;
  }
  else
  {
    VLOG(5) << "Switching down";
    --nCurrentChannel;
    eSwitchDirection = lme::SWITCH_DOWN;
    if (nCurrentChannel == 0)
      bUp = true;
  }
  return eSwitchDirection;
}

} // lme

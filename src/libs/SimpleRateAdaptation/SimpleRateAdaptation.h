#pragma once
#include <Media/IRateAdaptation.h>

namespace lme
{

class SimpleRateAdaptation : public IRateAdaptation
{
public:
  SimpleRateAdaptation();

  virtual SwitchDirection getRateAdaptAdvice(const RtpTransmissionStats& stats);

private:

};

} // lme


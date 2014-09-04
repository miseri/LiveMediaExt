#pragma once
#include <Media/IRateAdaptationFactory.h>

#ifdef WIN32
#ifdef SimpleRateAdaptation_EXPORTS
#define RATE_ADAPTATION_API __declspec(dllexport)
#else
#define RATE_ADAPTATION_API __declspec(dllimport)
#endif
#else
#define RATE_ADAPTATION_API 
#endif

namespace lme
{

class RATE_ADAPTATION_API  SimpleRateAdaptationFactory : public IRateAdaptationFactory
{
public:
  SimpleRateAdaptationFactory();
  ~SimpleRateAdaptationFactory();
  virtual IRateAdaptation* getInstance();
  virtual void releaseInstance(IRateAdaptation*);
};

} // lme


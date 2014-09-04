#pragma once
#include <Media/IRateAdaptation.h>

namespace lme
{ 

/**
 * @brief Factory class to create IRateAdaptation instances
 */
class IRateAdaptationFactory
{
public:
  IRateAdaptationFactory()
  {

  }

  virtual ~IRateAdaptationFactory()
  {

  }

  virtual IRateAdaptation* getInstance() = 0;
  virtual void releaseInstance(IRateAdaptation*) = 0;

private:

};

} // lme

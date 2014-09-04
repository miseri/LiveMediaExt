#include "SimpleRateAdaptationPch.h"
#include "SimpleRateAdaptationFactory.h"
#include "SimpleRateAdaptation.h"

namespace lme
{

SimpleRateAdaptationFactory::SimpleRateAdaptationFactory()
{

}

SimpleRateAdaptationFactory::~SimpleRateAdaptationFactory()
{

}

IRateAdaptation* SimpleRateAdaptationFactory::getInstance()
{
  return new SimpleRateAdaptation();
}

void SimpleRateAdaptationFactory::releaseInstance(IRateAdaptation* pRateAdaptation)
{
  if (pRateAdaptation)
  {
    delete pRateAdaptation;
    pRateAdaptation = NULL;
  }
}

} // lme


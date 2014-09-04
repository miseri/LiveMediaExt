#pragma once
#include <Media/RtpTransmissionStats.h>

namespace lme
{

enum SwitchDirection
{
  STAY = 0,             // no advice
  SWITCH_DOWN,          // switch down a single step
  SWITCH_UP,            // switch up a single step
  SWITCH_DOWN_MULTIPLE, // switch down multiple application defined steps
  SWITCH_UP_MULTIPLE,   // switch up multiple application defined steps
  SWITCH_DOWN_MIN,      // switch down to the minimum rate
  SWITCH_UP_MAX         // switch down to the maximum rate
};

/**
 * @brief The IRateAdaptation abstracts the rate adaptation process. 
 */
class IRateAdaptation
{
public:

  /**
   * @brief Virtual destructor
   */
  virtual ~IRateAdaptation()
  {

  }
  /**
   * @brief The subclass must implement the update of the RTCP statistics database
   * and return the advice based on the updated model.
   */
  virtual SwitchDirection getRateAdaptAdvice(const RtpTransmissionStats& stats) = 0;

protected:

  /**
   * @brief Constructor
   */
  IRateAdaptation()
  {

  }

};

} // lme

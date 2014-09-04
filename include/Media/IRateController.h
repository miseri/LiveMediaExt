#pragma once
#include <Media/IRateAdaptation.h>

namespace lme
{

/**
 * @brief Abstract interface for rate control. The application will
 * feed network statistics to the IRateAdaptation interface and should
 * pass the result of IRateAdaptation::getRateAdaptAdvice() to the  
 * IRateController implementation which should adjust the bit rate accordingly
 */
class IRateController
{
public:
  /**
   * @brief Virtual destructor
   */
  virtual ~IRateController()
  {

  }
  /**
   * @brief The implementation should adjust the bitrate
   * of the codec accordingly
   */
  virtual void controlBitrate(SwitchDirection eSwitch) = 0;

protected:
  /**
   * @brief Constructor
   */
  IRateController()
  {

  }
};

} // lme

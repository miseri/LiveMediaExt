/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "CSIR"
// Copyright (c) 2014 CSIR.  All rights reserved.
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

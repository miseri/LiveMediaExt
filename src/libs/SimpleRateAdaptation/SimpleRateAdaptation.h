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
 * @brief SimpleRoundTripDelayAnalyser
 *  This simple round trip delay analyser simply starts on channel 3 (with indexing starting at 0)
 *  It then switches down every 150 frames until it reaches the minimum band width channel (0).
 *  It switches up every 150 frames until it reaches th emaximum bandwidth channel and then repeats the
 *  process. This implementation assumes that the channel has 8 different bit rates!!!!
 */
class SimpleRateAdaptation : public lme::IRateAdaptation 
{
public:

  /**
   * @brief Constructor
   */
  SimpleRateAdaptation();

  /**
   * @brief TODO: describe implementation
   */
  lme::SwitchDirection getRateAdaptAdvice(const lme::RtpTransmissionStats& stats);

private:

  bool isTimeToMeasure();

	double m_dPreviousAverage;
	unsigned m_lastPacketNumReceived;
	int nCounter;
	int nCurrentChannel;
	bool bUp;
};

} // lme


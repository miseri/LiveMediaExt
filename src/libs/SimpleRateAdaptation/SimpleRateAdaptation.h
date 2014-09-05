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


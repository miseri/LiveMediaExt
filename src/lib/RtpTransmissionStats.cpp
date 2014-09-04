#include "LiveMediaExtPch.h"
#include <Media/RtpTransmissionStats.h>

namespace lme
{

std::ostream& operator<< (std::ostream& ostr, const RtpTransmissionStats& stats)
{
  ostr << "LastPacketNumReceived: " << stats.LastPacketNumReceived;
  return ostr;
}

} // lme

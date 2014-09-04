#pragma once
#include <cstdint>

namespace lme
{

struct RtpTransmissionStats
{
  uint32_t LastPacketNumReceived;
  uint32_t FirstPacketNumReported;
  uint32_t TotalPacketsLost;
  uint32_t Jitter;
  uint32_t LastSrTime;
  uint32_t DiffSrRr;
  uint32_t Rtt; // in units of 1/65536 seconds
};

std::ostream& operator<< (std::ostream& ostr, const RtpTransmissionStats& stats);

} // lme

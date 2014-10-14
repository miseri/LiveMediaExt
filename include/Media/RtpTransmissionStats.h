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

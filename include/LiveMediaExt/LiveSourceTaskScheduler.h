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
#include "LiveSourceTaskScheduler0.h"

namespace lme
{

/**
 * @brief Live media task scheduler that also handles live sources
 */
class LiveSourceTaskScheduler: public LiveSourceTaskScheduler0 {
public:
  /**
   * @brief Named constructor
   */
  static LiveSourceTaskScheduler* createNew(ChannelManager& channelManager);
  /**
   * @brief Destructor
   */
	virtual ~LiveSourceTaskScheduler();

protected:
  /**
   * @brief Constructor
   *
   *	called only by "createNew()"
   */
  LiveSourceTaskScheduler(ChannelManager& channelManager);

protected:
	/**
   * @brief Redefined virtual function
   */
	virtual void SingleStep(unsigned maxDelayTime);

};

} // lme


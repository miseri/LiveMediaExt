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

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#endif

// To prevent double inclusion of winsock on windows
#ifdef _WIN32
#define NOMINMAX
#include <WinSock2.h>
#endif

// messaging in compilation output
#ifdef _WIN32
#include <Windows.h>
// From Richter's "Windows via c++"
/*
When the compiler sees a line like this:
#pragma chMSG(Fix this later)

it outputs a line like this:

c:\CD\CmnHdr.h(82):Fix this later

You can easily jump directly to this line and examine the surrounding code.
*/
#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
#else
#define chMSG(desc) ""
#endif

#ifdef _WIN32
#pragma warning(push)     // disable for this header only
#pragma warning(disable:4251) 
// To get around compile error on windows: ERROR macro is defined
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#ifdef _WIN32
#pragma warning(pop)     // restore original warning level
#endif

/**
 * @def LOG_MODIFY_WITH_CARE Use this sentinel for any logging that needs to be extracted
 * from the log files and should be changed carefully, since the scripts might break.
 */

#define LOG_MODIFY_WITH_CARE "_LogSentinel_"

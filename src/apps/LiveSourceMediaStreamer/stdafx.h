#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#endif

// To prevent double inclusion of winsock on windows
#ifdef _WIN32
// To be able to use std::max
#define NOMINMAX
#include <WinSock2.h>
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

#pragma once

#include <sstream>
#include <string>

const unsigned MAJOR_VERSION = 0;
const unsigned MINOR_VERSION = 0;
const unsigned BUILD_VERSION = 1;

struct VersionInfo
{
  static std::string toString()
  {
    std::ostringstream ostr;
    ostr << MAJOR_VERSION << "." << MINOR_VERSION << "." << BUILD_VERSION;
    return ostr.str();
  }

};

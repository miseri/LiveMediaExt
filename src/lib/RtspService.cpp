#include "LiveMediaExtPch.h"
#include <Media/RtspService.h>

namespace lme
{

RtspService::RtspService()
  :m_cEventloop(0),
  m_pRtspServer(NULL),
  m_pScheduler(NULL),
  m_pEnv(NULL)
{

}

boost::system::error_code RtspService::start()
{
  // init live555 environment

  // start live 555 in new thread
  return boost::system::error_code();
}

boost::system::error_code RtspService::stop()
{
  // stop live555 thread
  return boost::system::error_code();
}

} //lme



#ifdef _WIN32
#define _WIN32_WINNT 0x0501

#define _SCL_SECURE_NO_WARNINGS
#define NOMINMAX
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#endif

// TODO: reference additional headers your program requires here

#ifdef _WIN32
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>

/// 1. Dynamic linking of boost test
#define BOOST_TEST_DYN_LINK

/// 2. A must! This boost UTF macro  BOOST_TEST_MODULE  generates the main entry point for this test project and names the master test suite. http://www.boost.org/doc/libs/1_38_0/libs/test/doc/html/utf/user-guide/test-organization/master-test-suite.html
#define BOOST_TEST_MODULE ts_master master_test_suite

/// 3. A must! This boost UTF header file must always be included. We use the static/dynamic library variant of the UTF. 
#include <boost/test/unit_test.hpp>

#include <deque>
#include <numeric>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <Media/MultiMediaSampleBuffer.h>

using namespace lme;

BOOST_AUTO_TEST_CASE (base_unit_test)
{
  BOOST_CHECK_EQUAL( true, true);
}

/**
 * @brief tc1_test_packet_manager_jitter_buffer checks that the packet manager
 * retains the packet for the approximate duration of the jitter buffer
 */
BOOST_AUTO_TEST_CASE( tc1_MM_header_parse_write ) 
{
  MultiplexedMediaHeader header(0);
  uint32_t uiMMFFourCC = ' fmm';
  header.setFourCC(uiMMFFourCC);
  header.addStreamLength(123);
  header.addStreamLength(246);

  uint8_t buffer[13];
  bool res = MultiplexedMediaHeader::writeHeader(header, buffer, 12);
  BOOST_CHECK_EQUAL(res, false);
  res = MultiplexedMediaHeader::writeHeader(header, buffer, 13);
  BOOST_CHECK_EQUAL(res, true);

  boost::optional<MultiplexedMediaHeader> pHeader = MultiplexedMediaHeader::read(buffer, 13);
  BOOST_CHECK_EQUAL(pHeader.is_initialized(), true);
  BOOST_CHECK_EQUAL(pHeader->getHeaderLength(), 13);
  BOOST_CHECK_EQUAL(pHeader->getFourCC(), uiMMFFourCC);
  BOOST_CHECK_EQUAL(pHeader->getStreamCount(), 2);
  BOOST_CHECK_EQUAL(pHeader->getStreamLength(0), 123);
  BOOST_CHECK_EQUAL(pHeader->getStreamLength(1), 246);

  MultiplexedMediaHeader headerWithoutFourCC(0);
  headerWithoutFourCC.addStreamLength(123);
  headerWithoutFourCC.addStreamLength(246);

  uint8_t buffer2[9];
  res = MultiplexedMediaHeader::writeHeader(headerWithoutFourCC, buffer2, 8);
  BOOST_CHECK_EQUAL(res, false);
  res = MultiplexedMediaHeader::writeHeader(headerWithoutFourCC, buffer2, 9);
  BOOST_CHECK_EQUAL(res, true);

  boost::optional<MultiplexedMediaHeader> pHeader2 = MultiplexedMediaHeader::read(buffer2, 9);
  BOOST_CHECK_EQUAL(pHeader2.is_initialized(), true);
  BOOST_CHECK_EQUAL(pHeader2->getHeaderLength(), 9);
  BOOST_CHECK_EQUAL(pHeader2->getFourCC(), 0);
  BOOST_CHECK_EQUAL(pHeader2->getStreamCount(), 2);
  BOOST_CHECK_EQUAL(pHeader2->getStreamLength(0), 123);
  BOOST_CHECK_EQUAL(pHeader->getStreamLength(1), 246);
}

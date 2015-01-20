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
#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace lme
{

/**
 * @brief This header structure is inband of the media stream. It is used to describe the multiplexed media format.
 *
 * Field                          Bit count    Description
 * --------------------------------------------------------
 * Version                        2            Current version is 0
 * FourCC indicator               1            If this flag is set, the FourCC field follows
 * Reserved                       1            Reserved
 * Number of Streams              4            Number of streams multiplexed together
 * Optional FourCC                32           Media type FourCC 
 * n lengths                      32 * n       Frame-bit-limits or sizes of multiplexed streams. The function varies according to if this is a descriptor or in-band header
 * Codec-specific data leng (LEN) 16           Length of codec-specific data following this field
 * Codec-specific data            LEN          Codec specific data e.g. SPS and PPS for H.264
 */
class MultiplexedMediaHeader
{
public:
  /**
   * @brief reads the header in pData and returns MultiplexedMediaHeader if successful, null pointer otherwise
   * @param[in] pData Buffer containing header
   * @param[in] uiSize Size of buffer
   * @return true if MultiplexedMediaHeader was successful, null pointer otherwise
   */
  static boost::optional<MultiplexedMediaHeader> read(const uint8_t* pBuffer, uint32_t uiBufferSize)
  {
    if (uiBufferSize < 1) return boost::optional<MultiplexedMediaHeader>();
    uint32_t uiVersion = pBuffer[0] & 0xC0;
    if (uiVersion != 0) return boost::optional<MultiplexedMediaHeader>();
    bool bFourCC = (pBuffer[0] & 0x20) > 0;
    uint32_t uiStreamCount = pBuffer[0] & 0x0F;
    if (uiBufferSize < 1 + (uiStreamCount * 4) + (bFourCC ? 4 : 0)) return boost::optional<MultiplexedMediaHeader>();
    MultiplexedMediaHeader header(uiVersion);
    const uint8_t* pSrc = pBuffer + 1;
    if (bFourCC)
    {
      memcpy(&header.m_uiFourCC, (void*)pSrc, 4);
      pSrc += 4;
    }
    uint32_t uiOffset = 2 + (bFourCC ? 4 : 0);
    for (size_t i = 0; i < uiStreamCount; ++i)
    {
      uint32_t uiLen = 0;
      memcpy(&uiLen, (void*)pSrc, 4);
      pSrc += 4;
      header.addStreamLength(uiLen);
    }
    return boost::optional<MultiplexedMediaHeader>(header);
  }
  /**
   * @brief writes the header to pData and returns true if successful, false otherwise
   * @param[in] header Header to be written
   * @param[in] pData Buffer where header is to be written
   * @param[in] uiSize Size of buffer
   * @return true if write was successful, false otherwise
   */
  static bool writeHeader(const MultiplexedMediaHeader& header, uint8_t* pData, uint32_t uiSize)
  {
    if (uiSize < (1 + (header.m_vStreamLengths.size()*4) + ((header.m_uiFourCC != 0) ? 4 : 0))) return false;
    pData[0] = 0;
    if (header.m_uiFourCC != 0) pData[0] = pData[0] | 0x20;
    assert(header.m_vStreamLengths.size() < 16);
    pData[0] |= (header.m_vStreamLengths.size() & 0xF);
    const uint8_t* pDest = pData + 1;
    if (header.m_uiFourCC != 0)
    {
      memcpy((void*)pDest, &header.m_uiFourCC, 4);
      pDest += 4;
    }
    for (size_t i = 0; i < header.m_vStreamLengths.size(); ++i)
    {
      memcpy((void*)pDest, &header.m_vStreamLengths[i], 4);
      pDest += 4;
    }
    return true;
  }

  /**
   * @brief Constructor
   */
  MultiplexedMediaHeader(uint32_t uiVersion)
    :m_uiVersion(uiVersion),
    m_uiFourCC(0)
#if 0
    ,m_uiAssetId(0)
#endif
	{
	}
  /**
   * @brief Getter for version
   */
  uint32_t getVersion() const { return m_uiVersion; }
  /**
   * @brief Getter for number of streams
   */
	uint32_t getStreamCount() const { return m_vStreamLengths.size(); }
  /**
   * @brief Getter for asset ID
   */	
  uint32_t getFourCC() const { return m_uiFourCC; }
  /**
   * @brief Setter for asset ID
   */
  void setFourCC(const uint32_t uiFourCC) { m_uiFourCC = uiFourCC; }
#if 0
  /**
   * @brief Getter for asset ID
   */	
  uint32_t getAssetId() const { return m_uiAssetId; }
  /**
   * @brief Setter for asset ID
   */
  void setAssetId(const uint32_t uiAssetId) { m_uiAssetId = uiAssetId; }
#endif
  /**
   * @brief Method to add stream length
   */
  void addStreamLength(uint32_t uiLength)
	{
		m_vStreamLengths.push_back(uiLength);
	}
  /**
   * @brief Method to get stream length at index
   */
  uint32_t getStreamLength(unsigned uiIndex)
	{
  	return m_vStreamLengths.at(uiIndex);
	}
  /**
   * @brief Getter for size of header
   */
  uint32_t getHeaderLength() const
  {
    return 1 + (m_vStreamLengths.size()*4) + (m_uiFourCC != 0 ? 4 : 0);
  }

protected:
  /// Currently only version 0 is supported
  uint32_t m_uiVersion;
  /// Stores FourCC if not 0
  uint32_t m_uiFourCC;
#if 0
  uint32_t m_uiAssetId;
#endif
  /// frame bit limits or sizes
  std::vector<uint32_t> m_vStreamLengths;
  /// Codec specific data
  std::string m_sCodecSpecificData;
};

} //lme

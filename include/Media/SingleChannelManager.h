#pragma once
#include <climits>
#include <Media/ChannelManager.h>
#include <Media/PacketManagerMediaChannel.h>

namespace lme
{

/**
 * @brief This class manages a single channel with audio and video.
 *
 * The channel id is not much use here. It is however needed when there can 
 * be multiple channels that supply the task manager with data.
 */
class SingleChannelManager : public ChannelManager
{
public:
  /**
   * @brief Constructor
   */
  SingleChannelManager(uint32_t uiChannelId)
    :m_packetManager(uiChannelId),
    m_uiChannelId(uiChannelId),
    m_uiVideoSourceId(UINT_MAX),
    m_uiAudioSourceId(UINT_MAX)
  {

  }
  /**
   * @brief Constructor
   */
  SingleChannelManager(uint32_t uiChannelId, uint32_t uiVideoSourceId, uint32_t uiAudioSourceId)
    :m_packetManager(uiChannelId),
    m_uiChannelId(uiChannelId),
    m_uiVideoSourceId(uiVideoSourceId),
    m_uiAudioSourceId(uiAudioSourceId)
  {

  }

  void setVideoSourceId(const uint32_t uiVideoSourceId) 
  { 
    assert(uiVideoSourceId != m_uiAudioSourceId);
    m_uiVideoSourceId = uiVideoSourceId; 
  }

  void setAudioSourceId(const uint32_t uiAudioSourceId) 
  {
    assert(uiAudioSourceId != m_uiVideoSourceId);
    m_uiAudioSourceId = uiAudioSourceId; 
  }

  const PacketManagerMediaChannel& getPacketManager() const { return m_packetManager; }

  PacketManagerMediaChannel& getPacketManager() { return m_packetManager; }

  boost::optional<MediaSample> getMedia(uint32_t uiChannelId, uint32_t uiSourceId)
  {
    assert(uiChannelId == m_uiChannelId);
    if (uiSourceId == m_uiVideoSourceId)
    {
      return m_packetManager.getVideo();
    }
    else if (uiSourceId == m_uiAudioSourceId)
    {
      return m_packetManager.getAudio();
    }
    else
    {
      assert(false);
      return boost::optional<MediaSample>();
    }
  }

protected:
  PacketManagerMediaChannel m_packetManager;
  uint32_t m_uiChannelId;
  uint32_t m_uiVideoSourceId;
  uint32_t m_uiAudioSourceId;
};

} // lme


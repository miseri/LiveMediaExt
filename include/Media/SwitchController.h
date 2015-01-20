#pragma once
#include <algorithm>
#include <vector>

namespace lme
{

/// The SwitchController class stores switching state and
/// allows bitrate and channel switch and limit operations 
/// to be performed.
/// This class takes the constraints such as min/max channel/bitrate
/// into account. If a limit operation results in a required switch
/// operation in order for the current channel to stay within the 
/// constraints, the shouldSwitch flag is set. This flag must be reset
/// by the calling class once it has synchronized channels.
class SwitchController
{
public:

  SwitchController()
    :m_uiCurrentChannel(0),
    m_uiTargetChannel(0),
    m_uiMinChannel(0),
    m_uiMaxChannel(0),
    m_uiMinBitrate(0),
    m_uiMaxBitrate(0),
    m_bInitialised(false),
    m_bSwitchFlag(false)
  {

  }

  /// returns current configured channel
  unsigned getCurrentChannel() const 
  { 
    assert(m_bInitialised);
    return m_uiCurrentChannel;
  }
  /// returns bitate of current configured channel
  unsigned getCurrentBitrate() const { return m_vBitrates[m_uiCurrentChannel];}

  /// This method returns the target channel if the shouldSwitch flag has been set
  unsigned getTargetChannel() const { return m_uiTargetChannel; }

  /// update channel to uiCurrentChannel if within constraints
  bool updateCurrentChannel(unsigned uiCurrentChannel) 
  {
    // Removing assertion: this is set in the SwitchingFrameGrabber constructor
    // at this time the component is not initialised yet
    //assert(m_bInitialised);
#if 0
    if (uiCurrentChannel >= m_uiMinChannel && uiCurrentChannel <= m_uiMaxChannel)
    {
      m_uiCurrentChannel = uiCurrentChannel; 
      return true;
    }
    else
    {
      return false;
    }
#else

      m_uiCurrentChannel = uiCurrentChannel; 
      return true;
#endif
  }

  /// initialises the channel bitrate information
  void setChannelBitrates(const std::vector<unsigned>& vBitrates)
  {
    m_vBitrates = vBitrates;
    std::sort(m_vBitrates.begin(), m_vBitrates.end());
    m_uiMinChannel = 0;
    m_uiMaxChannel = vBitrates.size() - 1;
    m_uiMinBitrate = vBitrates[m_uiMinChannel];
    m_uiMaxBitrate = vBitrates[m_uiMaxChannel];
    m_bInitialised = true;
  }

  /// A limit operation might result in a needed channel switch
  /// if this occurs this flag will indicate that a switch is necessary
  /// The getTargetChannel method can be called to determine which channel should
  /// be switched too.
  /// The resetSwitchFlag once the calling class has performed the appropriate
  /// switches.
  bool shouldSwitch() const { return m_bSwitchFlag; }
  /// Method to reset the switch flag once calling class has sychronised channels
  void resetSwitchFlag() { m_bSwitchFlag = false; }

  /// switches current channel up
  /// returns true if a switch up is possible according to the constraints
  bool switchUp()
  {
    assert(m_bInitialised);

    ++m_uiCurrentChannel;
    if (m_uiCurrentChannel > m_uiMaxChannel)
    {
      --m_uiCurrentChannel;
      return false;
    }
    return true;
  }

  /// switches current channel down
  /// returns true if a switch down is possible according to the constraints
  bool switchDown()
  {
    assert(m_bInitialised);

    // Using temp int since m_ui is unsigned
    int iNewChannel = m_uiCurrentChannel - 1;
    if (iNewChannel < static_cast<int>(m_uiMinChannel))
    {
      return false;
    }
    else
    {
      m_uiCurrentChannel = iNewChannel;
      return true;
    }
  }

  /// switches current channel to uiChannel
  /// returns true if a switch is possible according to the constraints
  bool switchChannel(unsigned uiChannel)
  {
    assert(m_bInitialised);

    // check boundaries
    if (uiChannel < m_uiMinChannel || uiChannel > m_uiMaxChannel)
      return false;
    
    m_uiTargetChannel = uiChannel;
    
    // Schedule if target differs from current
    if (m_uiTargetChannel != m_uiCurrentChannel)
    {
      // flag switch
      m_bSwitchFlag = true;
      return true;
    }
    else
    {
      return false;
    }
  }

  /// Sets channel constraints
  /// Returns true if a limit is possible
  /// This method will set the should switch flag if a switch is needed to meet
  /// the channel constraints
  bool limitChannel(unsigned uiMinChannel, unsigned uiMaxChannel)
  {
    assert(m_bInitialised);

    if (uiMaxChannel < uiMinChannel) return false;

    if (uiMinChannel >= m_vBitrates.size()) return false;

    if (uiMaxChannel >= m_vBitrates.size()) return false;

    m_uiMinChannel = uiMinChannel;
    m_uiMaxChannel = uiMaxChannel;

    m_uiMinBitrate = m_vBitrates[m_uiMinChannel];
    m_uiMaxBitrate = m_vBitrates[m_uiMaxChannel];

    setSwitchFlagIfNecessary();
    return true;
  }

  /// switches the channel to the target bitrate or the 
  /// channel with the next smaller bitrate
  bool switchBitRate(unsigned uiTargetBitrate)
  {
    assert(m_bInitialised);

    int iIndex = findIndexOfFirstBitrateLessOrEqual(uiTargetBitrate);
    if ( iIndex == -1 ) return false;
    return switchChannel(iIndex);
  }

  /// sets channel limits the specified bitrates
  /// The min and max channel are set in the range [uiMinBitrate, uiMaxBitrate]
  bool limitBitRate(unsigned uiMinBitrate, unsigned uiMaxBitrate)
  {
    assert(m_bInitialised);
    int iMinIndex = findIndexOfFirstBitrateGreaterOrEqual(uiMinBitrate);
    int iMaxIndex = findIndexOfFirstBitrateLessOrEqual(uiMaxBitrate);
    if (iMinIndex == -1 ||iMaxIndex == -1) return false;
    return limitChannel(iMinIndex, iMaxIndex);
  }

private:

  // returns first index of less or equal bitrate
  // Otherwise returns 0
  int findIndexOfFirstBitrateLessOrEqual(unsigned uiBitrate)
  {
    int iIndex(-1);
    for (size_t i = 0; i < m_vBitrates.size(); ++i)
    {
      if (m_vBitrates[i] <= uiBitrate)
        iIndex = i;
      else
        break;
    }
    return iIndex;
  }

  int findIndexOfFirstBitrateGreaterOrEqual(unsigned uiBitrate)
  {
    int iIndex(-1);
    for (size_t i = 0; i < m_vBitrates.size(); ++i)
    {
      if (m_vBitrates[i] >= uiBitrate)
      {
        iIndex = i;
        break;
      }
    }
    return iIndex;
  }

  void setSwitchFlagIfNecessary()
  {
    // Now check if current channel is in new bounds, otherwise schedule switch
    if (m_uiCurrentChannel < m_uiMinChannel)
    {
      m_uiTargetChannel = m_uiMinChannel;
      m_bSwitchFlag = true;
    }
    else if (m_uiCurrentChannel > m_uiMaxChannel)
    {
      m_uiTargetChannel = m_uiMaxChannel;
      m_bSwitchFlag = true;
    }
  }

  /// current channel
  unsigned m_uiCurrentChannel;
  unsigned m_uiTargetChannel;
  /// Channel bitrates
  std::vector<unsigned> m_vBitrates;

  /// Min/max channel
  unsigned m_uiMinChannel;
  unsigned m_uiMaxChannel;
  
  /// Max channel
  unsigned m_uiMinBitrate;
  unsigned m_uiMaxBitrate;

  /// Initialise flag
  bool m_bInitialised;
  /// Signals whether a switch to the target channel is necessary
  bool m_bSwitchFlag;
};

} // lme


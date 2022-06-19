#ifndef __SBSP_UBTECH_H__
#define __SBSP_UBTECH_H__

#include "Typedef.h"

//Serial Bus Servo Protocol
class cSbspUbTech
{
  public:

  typedef enum
  {
    Preamble1    = 0xFAAF,
    nPreamble1a  = 0xFA,
    nPreamble1b  = 0xAF,
    Preamble2    = 0xFCCF, // For version
    nPreamble2a  = 0xFC,
    nPreamble2b  = 0xCF,
    nEnd         = 0xED,
    nCmdPos      = 0x03,
    nChkSumPos   = 0x08,
    nEndPos      = 0x09,
    nCmdSetPos   = 0x01,
    nCmdGetPos   = 0x02,
    nCmdLed      = 0x04,
    nCmdSetOffsetAngle = 0xD2,
    nCmdGetOffsetAngle = 0xD4,
    nCmd5,
  }tenCmd;

  u8 maui8CmdBuffer[10];
  u8 mu8ServoId;

  virtual vOnSetPos(u8* laui8CmdBuffer) {}
  virtual vOnGetPos(u8* laui8CmdBuffer) {}
  virtual vOnLed(u8* laui8CmdBuffer) {}
  virtual vOnSetOffsetAngle(u8* laui8CmdBuffer) {}
  virtual vOnGetOffsetAngle(u8* laui8CmdBuffer) {}

  virtual vOnVersion(u8* laui8CmdBuffer) {}

  void vSetServoId(u8 lu8ServoId)
  {
    mu8ServoId = lu8ServoId;
  }


  bool bDecode(u8 *laui8CmdBuffer)
  {

    if ((maui8CmdBuffer[0] == nPreamble1a) &&
      (maui8CmdBuffer[1] == nPreamble1b))
    {
      u8 lu8Checksum;
      lu8Checksum = 0;
      for (u8 lu8t = 2; lu8t < nChkSumPos; lu8t++)
      {
        lu8Checksum += laui8CmdBuffer[lu8t];
      }
      if (lu8Checksum != laui8CmdBuffer[nChkSumPos]) return False;
      if (laui8CmdBuffer[nEndPos] != nEnd) return False;

      switch (laui8CmdBuffer[nCmdPos])
      {
        case nCmdSetPos:
          vOnSetPos(laui8CmdBuffer);
          break;
        case nCmdGetPos:
          vOnGetPos(laui8CmdBuffer);
          break;
        case nCmdLed:
          vOnLed(laui8CmdBuffer);
          break;
        case nCmdSetOffsetAngle:
          vOnSetOffsetAngle(laui8CmdBuffer);
          break;
        case nCmdGetOffsetAngle:
          vOnGetOffsetAngle(laui8CmdBuffer);
          break;
        default:
          return False;
          break;
      }
    }
    else
    if ((maui8CmdBuffer[0] == nPreamble2a) &&
        (maui8CmdBuffer[1] == nPreamble2b))
    {
      u8 lu8Checksum;
      lu8Checksum = 0;
      for (u8 lu8t = 2; lu8t < nChkSumPos; lu8t++)
      {
        lu8Checksum += laui8CmdBuffer[lu8t];
      }
      if (lu8Checksum != laui8CmdBuffer[nChkSumPos]) return False;
      if (laui8CmdBuffer[nEndPos] != nEnd) return False;

      switch (laui8CmdBuffer[nCmdPos])
      {
        case 1:
          vOnVersion(laui8CmdBuffer);
          break;
        default:
          return False;
          break;
      }
    }
    else
    {
      return False;
    }

    return True;
  }

  void vEncode()
  {    
    maui8CmdBuffer[0] = nPreamble1a;
    maui8CmdBuffer[1] = nPreamble1b;
    maui8CmdBuffer[2] = mu8ServoId;

    //Checksumme
    maui8CmdBuffer[nChkSumPos] = 0;
    for (u8 lu8t = 2; lu8t < nChkSumPos; lu8t++)
    {
      maui8CmdBuffer[nChkSumPos] += maui8CmdBuffer[lu8t];
    }
    maui8CmdBuffer[nEndPos] = nEnd;
  }

  void vSetLed(u8 lu8Value)
  {
    maui8CmdBuffer[nCmdPos] = enCmdLed;
    maui8CmdBuffer[4] = lu8Value;
    vEncode();
  }


};

#endif // __SBSP_UBTECH_H__

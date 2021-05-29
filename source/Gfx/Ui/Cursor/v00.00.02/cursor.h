#ifndef __UI_CURSOR__H__
#define __UI_CURSOR__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "gfxdefs.h"
#include "cStringTools.h"
#include "msg.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cCursor
{
public:
  enum tenSmEvent
  {
    //nSmEvFirst = 0, 
    nSmEvReset,
    nSmEvTick10ms,
    //nSmEvLast
  };

  enum tenSmState
  {
    //nSmStFirst = 0,
    nSmStReset,
    nSmStIdle,
    nSmStPressed,
    nSmStMove,
    nSmStDrag,
    //nSmStLast
  };

  enum tenMsgEvent
  {
    nPressStart,
    nPressEnd,
    nPressEndShort,
    nPressEndLong,
    nMove,
    nPressDragStart,
    nPressDragEnd,
    nDrag,
    nSwipe
  };

  int16 mi16Posx;
  int16 mi16Posy;

  int16 mi16PosxOld;
  int16 mi16PosyOld;

  int16 mi16PosxPressed;
  int16 mi16PosyPressed;

  uint16 mu16TimeToShortPressed; // [ms]
  uint16 mu16TimeToLongPressed;  // [ms]
  uint16 mu16TimeCnt;            // [ms]

  tenSmState mSmState;

  cMsgBox *mpcMsgBox;

  struct tstStatus
  {
    u8 PressedOld   : 1;
    u8 Pressed      : 1;
  };

  char mszPos[20];

  tstStatus mstStatus;

  cCursor(uint16 lu16TimeToShortPressed, uint16 lu16TimeToLongPressed)
  {
    cCursor(lu16TimeToShortPressed, lu16TimeToLongPressed, null);
  }

  cCursor(uint16 lu16TimeToShortPressed, uint16 lu16TimeToLongPressed, cMsgBox *lpcMsgBox)
  {
    mu16TimeToShortPressed = lu16TimeToShortPressed;
    mu16TimeToLongPressed  = lu16TimeToLongPressed;
    mpcMsgBox              = lpcMsgBox;
    vSm(nSmEvReset);
  }

  inline bool WasPressed()
  {
    return ((mstStatus.PressedOld == 0) && (mstStatus.Pressed == 1));
  }

  inline bool WasReleased()
  {
    return ((mstStatus.PressedOld == 1) && (mstStatus.Pressed == 0));
  }

  inline bool WasMoved()
  {
    return (mi16PosxOld != mi16Posx) || (mi16PosyOld != mi16Posy);
  }

  inline bool WasDraggedStart()
  {
    return (mi16Posx > mi16PosxPressed + 5 ) || (mi16Posy > mi16PosyPressed + 5) ||
           (mi16Posx < mi16PosxPressed - 5 ) || (mi16Posy < mi16PosyPressed - 5 );
  }


  void SetMsgData(cMsg *lpcMsg)
  {
    int16 *lpi16Data = (int16 *)lpcMsg->mui8Data;

    lpi16Data[0] = (i16)mi16Posx;
    lpi16Data[1] = (i16)mi16Posy;
    lpi16Data[2] = (i16)mi16PosxOld;
    lpi16Data[3] = (i16)mi16PosyOld;
  }

  void vSendMsg(tenMsgEvent lenMsgEvent)
  {
    
    if (mpcMsgBox != null)
    {
      cMsg     lcMsg;
      lcMsg.mID = (uint8)cMsg::tenMsgIDs::nCursor;
      lcMsg.mSubID = (uint8)lenMsgEvent;
      SetMsgData(&lcMsg);

      mpcMsgBox->vput(&lcMsg);
    }
  }

  void vSm(tenSmEvent lenEvent)
  {
    if (lenEvent == nSmEvReset)
    {
      mi16Posx = 0;
      mi16Posy = 0;
      mi16PosxOld = 0;
      mi16PosyOld = 0;
      mi16PosxPressed = 0;
      mi16PosyPressed = 0;

      mu16TimeCnt = 0;

      mstStatus.PressedOld = 0;
      mstStatus.Pressed = 0;

      mSmState = nSmStReset;
    }
    else if (lenEvent == nSmEvTick10ms)
    {
      switch (mSmState)
      {
        case nSmStReset:
          mSmState = nSmStIdle;
          break;

        case nSmStIdle:
          if (WasPressed())
          {
            mSmState = nSmStPressed;
            vSendMsg(nPressStart);
          }
          else if (WasMoved())
          {
            mSmState = nSmStMove;
            vSendMsg(nMove);
          }
          break;

        case nSmStPressed:
          if (WasReleased())
          {
            mSmState = nSmStIdle;
            if (mu16TimeCnt < mu16TimeToShortPressed)
            {
              vSendMsg(nPressEndShort);
            }
            else
            if (mu16TimeCnt > mu16TimeToLongPressed)
            {
              vSendMsg(nPressEndLong);
            }
            else
            {
              vSendMsg(nPressEnd);
            }
          }
          else if (WasDraggedStart())
          {
            mSmState = nSmStDrag;
            mi16PosxOld = mi16PosxPressed;
            mi16PosyOld = mi16PosyPressed;
            vSendMsg(nPressDragStart);
          }
          break;

        case nSmStMove:
          if (WasPressed())
          {
            mSmState = nSmStPressed;
            vSendMsg(nPressStart);
          }
          else if (WasMoved())
          {
            vSendMsg(nMove);
          }
          break;

        case nSmStDrag:
          if (WasReleased())
          {
            mSmState = nSmStIdle;
            vSendMsg(nPressDragEnd);
            vSendMsg(nPressEnd);
          }
          else if (WasMoved())
          {
            vSendMsg(nDrag);
          }
          break;
        default:
          break;
      }
    }
  }

  void vTick10ms(int16 li16Posx, int16 li16Posy, uint8 lu8Pressed)
  {
    if ((mi16PosxOld != li16Posx) ||
        (mi16PosyOld != li16Posy) ||
        (mstStatus.PressedOld != lu8Pressed))
    {
      mi16PosxOld = mi16Posx;
      mi16PosyOld = mi16Posy;
      mstStatus.PressedOld = mstStatus.Pressed;

      mstStatus.Pressed = lu8Pressed;
      mi16Posx = li16Posx;
      mi16Posy = li16Posy;

      if (WasPressed())
      {
        mu16TimeCnt = 0;
        mi16PosxPressed = mi16Posx;
        mi16PosyPressed = mi16Posy;
      }
      else
      {
        mu16TimeCnt += 10;
      }

      vSm(nSmEvTick10ms);
    }
    else
    {
      mu16TimeCnt += 10;
    }
  }

  /*
  void vSetPressed(uint8 lui8State)
  {
    mstStatus.Pressed = lui8State;
    mi16PosxPressed  = mi16Posx;
    mi16PosyPressed  = mi16Posy;
  }
        
  void vSetXY(int16 liGfxPosX, int16 liGfxPosY)
  {
    mi16PosxOld = mi16Posx;
    mi16PosyOld = mi16Posy;

    mi16Posx = liGfxPosX;
    mi16Posy = liGfxPosY;
  }*/

  int16 iGfxGetMovedX()
  {
    return mi16Posx - mi16PosxOld;
  }

  int16 iGfxGetMovedY()
  {
    return mi16Posy - mi16PosyOld;
  }


  char* toString()
  {
    char mszValue[6];
    mszPos[0] = 0;
    cStrTools::uixItoan(mi16Posx, mszValue, 6, 10);
    cStrTools::szStrCat(mszPos, mszValue);
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::uixItoan(mi16Posy, mszValue, 6, 10);
    cStrTools::szStrCat(mszPos, mszValue);

    switch (mSmState)
    {
      case nSmStIdle:
        cStrTools::szStrCat(mszPos, ",Idle");
        break;
      case nSmStPressed:
        cStrTools::szStrCat(mszPos, ",Press");
        break;
      case nSmStMove:
        cStrTools::szStrCat(mszPos, ",Move");
        break;
      case nSmStDrag:
        cStrTools::szStrCat(mszPos, ",Drag");
        break;
      default:
        break;
    }

    return mszPos;
  }

  void vClone(cCursor *lcpCursor)
  {
    lcpCursor->mi16Posx = mi16Posx;
    lcpCursor->mi16Posy = mi16Posy;
    lcpCursor->mi16PosxOld = mi16PosxOld;
    lcpCursor->mi16PosyOld = mi16PosyOld;
    lcpCursor->mi16PosxPressed = mi16PosxPressed;
    lcpCursor->mi16PosyPressed = mi16PosyPressed;

    lcpCursor->mstStatus = mstStatus;
  }

  
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_CURSOR__H__

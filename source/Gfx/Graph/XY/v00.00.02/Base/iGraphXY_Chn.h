#ifndef IGRAPHXY_CHANNEL_H
#define IGRAPHXY_CHANNEL_H

#include "TypeDef.h"
#include "rfont.h"
#include "cStringTools.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define GRAPHXY_CHANNEL_DRAWMODE_DOTS  0
#define GRAPHXY_CHANNEL_DRAWMODE_LINES 1

#define GRAPHXY_CHANNEL_BUFMODE_UNLIMITED  0
#define GRAPHXY_CHANNEL_BUFMODE_LIMITED    1

class ciGraphXY_Channel
{
public: 
  stPxli16 *mpstP;       // Buffer

  uint16    mu16Size;     // Total size of buffer
  uint16    mu16Count;    // Current number of bytes in buffer
  uint16    mu16Inpos;    // current in and out position
  uint16    mu16Outpos;
  uint8     mui8BuffMode;

  //Skalierung
  int16     mi16SkalYCnt;
  int16     mi16SkalYMax;
  int16     mi16SkalYMin;
  GfxInt    miGfxSkalY_MaxStrWidth;

  //Beschriftung
  char8     mszLabelY[16];
  GfxInt    miGfxLabelY_MaxStrWidth;
  int16     mi16LabelYDiv;

  uint32    mui32Col;
  uint8     mui8DrawMode;

  cRFont    *mpstRFont;
  
  ciGraphXY_Channel* mcNext;

  ciGraphXY_Channel(cRFont *lpstRFont, stPxli16 *lpstP, uint16 lui16Size, ciGraphXY_Channel* lcNext)
  {
    mpstRFont    = lpstRFont;
    mu16Size     = lui16Size;
    mpstP        = lpstP;
    mui8BuffMode = GRAPHXY_CHANNEL_BUFMODE_UNLIMITED;

    mui32Col     = 0xFFFFFF;
    mui8DrawMode = GRAPHXY_CHANNEL_DRAWMODE_LINES;

    mcNext = lcNext;

    mi16SkalYCnt = 10;
    mi16LabelYDiv = 1;

    vSkalY_Value(0, 100);

    vSet_Y_Label((char *)"Y-Label");

    vClear();
  }

  void vClear()
  {
    mu16Count  = 0;
    mu16Inpos  = 0;
    mu16Outpos = 0;
  }

  void vSkalY_Value(int16 li16SkalYMin, int16 li16SkalYMax)
  {
    tstGfxPos2D stStrLen;
    char8  lszTemp[16];

    mi16SkalYMax = li16SkalYMax;
    mi16SkalYMin = li16SkalYMin;

    cStrTools::uixItoa(li16SkalYMax, lszTemp, 10);
    mpstRFont->mcText.From(lszTemp);
    mpstRFont->i8StringLen(&stStrLen);
    miGfxSkalY_MaxStrWidth = stStrLen.x;

    cStrTools::uixItoa(mi16SkalYMin, lszTemp, 10);
    mpstRFont->mcText.From(lszTemp);
    mpstRFont->i8StringLen(&stStrLen);

    if (stStrLen.x > miGfxSkalY_MaxStrWidth)
    {
      miGfxSkalY_MaxStrWidth = stStrLen.x;
    }
  }

  void vSet_Y_Label(const char8* lszText)
  {
    tstGfxPos2D stStrLen;

    cStrTools::szStrnCpy(mszLabelY, lszText, sizeof(mszLabelY));

    mpstRFont->mcText.From((char8*) lszText);
    mpstRFont->i8StringLen(&stStrLen);
    miGfxLabelY_MaxStrWidth = stStrLen.x;
  }

  void vSubX(i16 li16Value)
  {
    uint16   lui16Idx;
    uint16   lui16Pos;
 
    lui16Pos = mu16Outpos;

    for (lui16Idx = 0; lui16Idx < mu16Count; lui16Idx++)
    {
      mpstP[lui16Pos].x -= li16Value;
      lui16Pos++;
      if (lui16Pos == mu16Size)
      {
        lui16Pos = 0;
      }
    }
  }

  void vAdd(stPxli16 *pstP)
  {
    if (mui8BuffMode == GRAPHXY_CHANNEL_BUFMODE_UNLIMITED)
    {
      // store data
      mpstP[mu16Inpos].x = pstP->x;
      mpstP[mu16Inpos].y = pstP->y;

      mu16Inpos++; // increase in-position counter
      if (mu16Inpos == mu16Size) // if we have reached the end of queue, go to beginning
      {
        mu16Inpos = 0;
      }

      if (mu16Count < mu16Size)
      {
        mu16Count++; // number of bytes in buffer
      }
      else
      {
        mu16Outpos++;
        if (mu16Outpos == mu16Size)
        {
          mu16Outpos = 0;
        }
      }
    }
    else
    {
      // if fifo is full, check if logging is continued
      if (mu16Count < mu16Size)
      {
        // store data
        mpstP[mu16Inpos].x = pstP->x;
        mpstP[mu16Inpos].y = pstP->y;

        mu16Inpos++; // increase in-position counter
        if (mu16Inpos == mu16Size) // if we have reached the end of queue, go to beginning
        {
          mu16Inpos = 0;
        }

        if (mu16Count < mu16Size)
        {
          mu16Count++; // number of bytes in buffer
        }
      }
    }
  }

  void vAddXY(uint16 lui16x, uint16 lui16y)
  {
    stPxli16 lstP;

    lstP.x = lui16x;
    lstP.y = lui16y;

    vAdd(&lstP);
  }
};

#ifdef __cplusplus
}
#endif

#endif //IGRAPHXY_CHANNEL_H



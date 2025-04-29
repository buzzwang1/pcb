#ifndef IGRAPHY_CHANNEL_H
#define IGRAPHY_CHANNEL_H

#include "TypeDef.h"
#include "rfont.h"
#include "cStringTools.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define GRAPHY_CHANNEL_DRAWMODE_DOTS  0
#define GRAPHY_CHANNEL_DRAWMODE_LINES 1

#define GRAPH_CHANNEL_BUFMODE_UNLIMITED  0
#define GRAPHY_CHANNEL_BUFMODE_LIMITED    1

class ciGraphY_Channel
{
public: 
  i16      *mi16P;       // Buffer

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
  
  ciGraphY_Channel* mcNext;

  ciGraphY_Channel(cRFont *lpstRFont, i16 *li16P, uint16 lui16Size, ciGraphY_Channel* lcNext)
  {
    mpstRFont    = lpstRFont;
    mu16Size     = lui16Size;
    mi16P        = li16P;
    mui8BuffMode = GRAPH_CHANNEL_BUFMODE_UNLIMITED;

    mui32Col     = 0xFFFFFF;
    mui8DrawMode = GRAPHY_CHANNEL_DRAWMODE_LINES;

    mcNext = lcNext;

    mi16SkalYCnt = 10;
    mi16LabelYDiv = 1;

    vSkalY_Value(0, 100);

    vSet_Y_Label((char *)"Y-Values");

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

  void vAdd(i16 li16P)
  {
    if (mui8BuffMode == GRAPH_CHANNEL_BUFMODE_UNLIMITED)
    {
      // store data
      mi16P[mu16Inpos] = li16P;

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
        mi16P[mu16Inpos] = li16P;

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
};

#ifdef __cplusplus
}
#endif

#endif //IGRAPHY_CHANNEL_H



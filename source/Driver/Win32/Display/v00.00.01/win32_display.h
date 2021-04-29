#ifndef __WIN32_DISPLAY_H__
#define __WIN32_DISPLAY_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <windows.h>
#include "bitmap.h"

   class cWin32Display
   {
     public:
     HWND          GraphHWND;      // Handle des Fensters
     HDC           GraphHDC;       // Devicecontext
     BITMAPINFO   *mpstBitmapinfo;

     uint16        ui16Graph_x1,
                   ui16Graph_y1;

     uint8         ui8Zoom;

     cBitmap_Bpp32_8R8G8B  mcBm;

     cWin32Display(uint16 lui16X1, uint16 lui16Y1,
                   uint16 lui16Width, uint16 lui16Height,
                   uint8  lui8Zoom,
                   HWND   lhWnd)
       :mcBm(lui16Width, lui16Height)
     {

       GraphHWND = lhWnd;
       GraphHDC = GetDC(lhWnd);       // Device Context

       ui8Zoom = lui8Zoom;

       ui16Graph_x1 = lui16X1;
       ui16Graph_y1 = lui16Y1;

       Create_BitMapInfo();
     }

     void Create_BitMapInfo()
     {
       int bisize = sizeof(BITMAPINFOHEADER);

       mpstBitmapinfo = (BITMAPINFO *)malloc(bisize + 12);
       ZeroMemory(&mpstBitmapinfo->bmiHeader, bisize);

       // BitmapInfoHeader eines normalen 16 Bit Bitmaps 
       // wie wir es brauchen erzeugen
       mpstBitmapinfo->bmiHeader.biSize = bisize;
       mpstBitmapinfo->bmiHeader.biWidth = mcBm.miGfxWidth;
       mpstBitmapinfo->bmiHeader.biHeight = -mcBm.miGfxHeight;
       mpstBitmapinfo->bmiHeader.biPlanes = 1;
       mpstBitmapinfo->bmiHeader.biBitCount = mcBm.u8Bpp();
       mpstBitmapinfo->bmiHeader.biCompression = BI_BITFIELDS;
       // Farb-Felder des 32-Bit Bitmaps setzen.
       ((long*)&mpstBitmapinfo->bmiColors)[0] = 0xFF0000;
       ((long*)&mpstBitmapinfo->bmiColors)[1] = 0x00FF00;
       ((long*)&mpstBitmapinfo->bmiColors)[2] = 0x0000FF;
     }

     void Show()
     {
       if (!mcBm.bChkInit()) return;

       /*SetDIBitsToDevice(GraphHDC,
                         ui16Graph_x1,
                         ui16Graph_y1,
                         mcBm.miGfxWidth,
                         mcBm.miGfxHeight,
                         0, 0, 0,
                         mcBm.miGfxHeight,
                         mcBm.mpui8Data,
                         mpstBitmapinfo,
                         DIB_RGB_COLORS);*/

        StretchDIBits(GraphHDC,
                      ui16Graph_x1,
                      ui16Graph_y1,
                      mcBm.miGfxWidth * ui8Zoom,
                      mcBm.miGfxHeight * ui8Zoom,
                      0, 0,
                      mcBm.miGfxWidth,
                      mcBm.miGfxHeight,
                      mcBm.mpui8Data,
                      mpstBitmapinfo,
                      DIB_RGB_COLORS,
                      SRCCOPY);
     }

     ~cWin32Display()
     {
       free(mpstBitmapinfo);
       ReleaseDC(GraphHWND, GraphHDC);
       GraphHDC = 0;
     }
   };


#ifdef __cplusplus
}
#endif

#endif  //WIN32_DISPLAY

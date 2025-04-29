/****************************************************************************************/
/*! \file Sections.h
 *  \brief Einige Sections um z.B. Daten vom Code zu trennen. (Bringt Zeit beim Reflashen)
 *  \author Florian Kaes
 *  \version 2017_11_05
 *  \date    05.11.2017
 *  \section history History
 *
 *  \since 24.04.2013
 *  - Erstellt
 */
/****************************************************************************************/



#ifndef _SECTIONS_H
#define _SECTIONS_H

#define  GFX_SecCode  //__attribute__((section(".Rom_Gfx_Code")))
#define  GFX_SecConst //__attribute__((section(".Rom_Gfx_Data")))


#endif // _SECTIONS_H

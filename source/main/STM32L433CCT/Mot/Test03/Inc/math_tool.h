/*
 * math_tool.h
 *
 *  Created on: 4 nov. 2020
 *      Author: Patrick
 */

#ifndef INC_MATH_TOOL_H_
#define INC_MATH_TOOL_H_

#include "TypeDef.h"

class cMathTool
{
  public: 
  static int32_t constrain(int32_t x, int32_t min, int32_t max)
  {
      if(x<min)
          return min;
      else if(x>max)
          return max;
      else
          return x;
  }

  static float fconstrain(float x, float min, float max)
  {
      if(x<min)
          return min;
      else if(x>max)
          return max;
      else
          return x;
  }

  static uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
  {
      if(x<=in_min)
          return out_min;
      else if(x>=in_max)
          return out_max;
      else
          return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  static float fmap(float x, float in_min, float in_max, float out_min, float out_max)
  {
      if(x<=in_min)
          return out_min;
      else if(x>=in_max)
          return out_max;
      else
          return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
};

#endif /* INC_MATH_TOOL_H_ */

#ifndef __BOTNET_CONCEPTS_H__
#define __BOTNET_CONCEPTS_H__

#include "Typedef.h"
#include <concepts>

template <typename T>
concept cptLink = requires(T v, u16 lu16Adr, u16 lu16Time_ms, cComNode::tenEvent lenEvent, cComNode::tenError lenError, cComNode::tenState lenState)
{
  { v.vComStart(lenEvent) }           -> std::same_as<void>;
  { v.vComDone() }                    -> std::same_as<void>;
  { v.vComError(lenError, lenState) } -> std::same_as<void>;

  { v.bAddedToBn(lu16Adr) }          -> std::same_as<bool>;
  { v.vTick(lu16Time_ms) }           -> std::same_as<void>;
};

template <typename T>
concept cptComDriver = requires(T v, cComNode::tenEvent lenEvent)
{
  { v.enInitHw() }    -> std::same_as<cComNode::tenError>;
  { v.vResetCom() }   -> std::same_as<void>;
  { v.vComError() }   -> std::same_as<void>;
  { v.bCheckBusy() }  -> std::same_as<bool>;
  { v.vSm(lenEvent) } -> std::same_as<void>;
};

#endif // __BOTNET_CONCEPTS_H__

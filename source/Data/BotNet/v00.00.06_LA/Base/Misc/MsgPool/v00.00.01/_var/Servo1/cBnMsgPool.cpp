#include "cBnMsgPool.h"

#if cBotNet_MsgPool16B > 0
  cBnMsgPool::cMsgPool<(cBotNet_MsgSize / 4), cBnMsgPool::nCnt16B, 1>                                             cBnMsgPool::mcMsgPool16B = cBnMsgPool::cMsgPool<(cBotNet_MsgSize / 4), cBnMsgPool::nCnt16B, 1>();
#endif
#if cBotNet_MsgPool32B > 0
  cBnMsgPool::cMsgPool<(cBotNet_MsgSize / 2), cBnMsgPool::nCnt32B, 1 + cBnMsgPool::nCnt16B>                       cBnMsgPool::mcMsgPool32B = cBnMsgPool::cMsgPool<(cBotNet_MsgSize / 2), cBnMsgPool::nCnt32B, 1 + cBnMsgPool::nCnt16B>();
#endif
#if cBotNet_MsgPool64B > 0
  cBnMsgPool::cMsgPool<(cBotNet_MsgSize + 1), cBnMsgPool::nCnt64B, 1 + cBnMsgPool::nCnt16B + cBnMsgPool::nCnt32B> cBnMsgPool::mcMsgPool64B = cBnMsgPool::cMsgPool<(cBotNet_MsgSize + 1), cBnMsgPool::nCnt64B, 1 + cBnMsgPool::nCnt16B + cBnMsgPool::nCnt32B>();
#endif

cBnMsgPool::cMsgPoolBase* cBnMsgPool::mcMsgPool[1 + 3] =
  {
    null,
    #if cBotNet_MsgPool16B > 0
      &cBnMsgPool::mcMsgPool16B,
    #else
      null,
    #endif

    #if cBotNet_MsgPool32B > 0
      &cBnMsgPool::mcMsgPool32B,
    #else
      null,
    #endif

    #if cBotNet_MsgPool64B > 0
      &cBnMsgPool::mcMsgPool64B,
    #else
      null,
    #endif
  };

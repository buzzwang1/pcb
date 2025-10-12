#include "cBnMsgPool.h"

cBnMsgPool::Msg16B cBnMsgPool::mcMsg16B_Pool[16];
cBnMsgPool::Msg32B cBnMsgPool::mcMsg32B_Pool[8];
cBnMsgPool::Msg64B cBnMsgPool::mcMsg64B_Pool[8];

u8 cBnMsgPool::mcMsg16BBuf[16];
u8 cBnMsgPool::mcMsg32BBuf[8];
u8 cBnMsgPool::mcMsg64BBuf[8];

cRingBufT<uint8, uint16> cBnMsgPool::mcMsg16B = cRingBufT<uint8, uint16>(cBnMsgPool::mcMsg16BBuf, sizeof(cBnMsgPool::mcMsg16BBuf));
cRingBufT<uint8, uint16> cBnMsgPool::mcMsg32B = cRingBufT<uint8, uint16>(cBnMsgPool::mcMsg32BBuf, sizeof(cBnMsgPool::mcMsg32BBuf));
cRingBufT<uint8, uint16> cBnMsgPool::mcMsg64B = cRingBufT<uint8, uint16>(cBnMsgPool::mcMsg64BBuf, sizeof(cBnMsgPool::mcMsg64BBuf));


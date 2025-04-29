#include "cWufHandler.h"

tunWakeupSources* cWufHandler::munWakeupSources = (tunWakeupSources*)(&cBuRam::mBuRam->u32WuReason);


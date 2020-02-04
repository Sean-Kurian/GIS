#ifndef GLOBALDATA_H
#define GLOBALDATA_H

//==============================================================================
// File Description: Declares existance of our global data object as extern 
// which allows it to be used in any file which has #include "globalData.h"
// The object is actually constructed at the top of loadAndCloseMap.cpp
//==============================================================================

#include "MapData.h"

extern MapData gData;

#endif /* GLOBALDATA_H */


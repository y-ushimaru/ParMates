/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "../ObjManager.h"
#include "VehicleTram.h"

using namespace std;

//======================================================================
VehicleTram* ObjManager::createVehicleTram()
{
    VehicleTram* tmpTram;
    tmpTram = new VehicleTram();
    return tmpTram;
}

#endif //INCLUDE_TRAMS

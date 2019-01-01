/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "VehicleFamily.h"

//######################################################################
// 大カテゴリ（10の位）
VehicleType VehicleFamily::miniPassenger()
{
    return _miniPassenger*10;
}
bool VehicleFamily::isMiniPassenger(VehicleType type)
{
    return (_miniPassenger*10<=type && type<(_miniPassenger+1)*10);
}
//----------------------------------------------------------------------
VehicleType VehicleFamily::passenger()
{
    return _passenger*10;
}
bool VehicleFamily::isPassenger(VehicleType type){
    return (_passenger*10<=type && type<(_passenger+1)*10);
}
//----------------------------------------------------------------------
VehicleType VehicleFamily::bus()
{
    return _bus*10;
}
bool VehicleFamily::isBus(VehicleType type)
{
    return (_bus*10<=type && type<(_bus+1)*10);
}
//----------------------------------------------------------------------
VehicleType VehicleFamily::miniTruck()
{
    return _miniTruck*10;
}
bool VehicleFamily::isMiniTruck(VehicleType type)
{
    return (_miniTruck*10<=type && type<(_miniTruck+1)*10);
}
//----------------------------------------------------------------------
VehicleType VehicleFamily::truck()
{
    return _truck*10;
}
bool VehicleFamily::isTruck(VehicleType type)
{
    return (_truck*10<=type && type<(_truck+1)*10);
}
//----------------------------------------------------------------------
VehicleType VehicleFamily::tram()
{
    return _tram*10;
}
bool VehicleFamily::isTram(VehicleType type)
{
    return (_tram*10<=type && type<(_tram+1)*10);
}


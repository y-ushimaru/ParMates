/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "VehicleTram.h"
#include "TramLineManager.h"
#include "TramLine.h"
#include "../Intersection.h"
#include "../Route.h"
#include <vector>
#include <iostream>

using namespace std;

//======================================================================
VehicleTram::VehicleTram()
{
}

//======================================================================
VehicleTram::~VehicleTram()
{
}

//======================================================================
void VehicleTram::setRoute()
{
    // 起点・終点を含む通過すべき交差点
    const vector<const Intersection*>* gates = _route->gates();

    // 条件に合致する路面電車の路線を取得する
    TramLine* tramLine
        = TramLineManager::instance()
        .tramLineWithDesignatedGates(gates);

    if (!tramLine)
    {
        cerr << "ERROR: tramLine not found:" << endl;
        _route->print(cerr);
        return;
    }

    vector<const Intersection*> tramLineInters;
    tramLine->getIntersections(&tramLineInters);

    for (unsigned int i=0; i<tramLineInters.size(); i++)
    {
        _route->addIntersection(tramLineInters[i]);
    }
}

#endif //INCLUDE_TRAMS

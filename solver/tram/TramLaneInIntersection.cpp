/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "TramLaneInIntersection.h"
#include "../LaneBundle.h"
#include "../Connector.h"

using namespace std;

//======================================================================
TramLaneInIntersection::TramLaneInIntersection(const string& id,
                                               const Connector* ptBegin,
                                               const Connector* ptEnd,
                                               LaneBundle* parent)
    : LaneInIntersection(id, ptBegin, ptEnd, parent)
{
}

#endif //INCLUDE_TRAMS

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "TramLaneInSection.h"
#include "../LaneBundle.h"
#include "../Connector.h"

using namespace std;

//======================================================================
TramLaneInSection::TramLaneInSection(const string& id,
                                     const Connector* ptBegin,
                                     const Connector* ptEnd,
                                     LaneBundle* parent)
    : LaneInSection(id, ptBegin, ptEnd, parent)
{
}

#endif //INCLUDE_TRAMS

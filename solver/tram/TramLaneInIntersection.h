/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TRAM_LANE_IN_INTERSECTION_H__
#define __TRAM_LANE_IN_INTERSECTION_H__

#include "../LaneInIntersection.h"
#include <string>

class LaneBundle;
class Connector;

class TramLaneInIntersection : public LaneInIntersection
{
public:
    TramLaneInIntersection(const std::string& id,
                           const Connector* ptBegin,
                           const Connector* ptEnd,
                           LaneBundle* parent);
    virtual ~TramLaneInIntersection(){};
};

#endif //__TRAM_LANE_IN_INTERSECTION_H__

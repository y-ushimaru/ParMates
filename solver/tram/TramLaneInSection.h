/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TRAM_LANE_IN_SECTION_H__
#define __TRAM_LANE_IN_SECTION_H__

#include "../LaneInSection.h"
#include <string>

class LaneBundle;
class Connector;

class TramLaneInSection : public LaneInSection
{
public:
    TramLaneInSection(const std::string& id,
                      const Connector* ptBegin,
                      const Connector* ptEnd,
                      LaneBundle* parent);
    virtual ~TramLaneInSection(){};
};

#endif //__TRAM_LANE_IN_SECTION_H__

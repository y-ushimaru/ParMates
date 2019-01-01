/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "SectionTramExt.h"
#include "TramLaneInSection.h"
#include "BorderTram.h"
#include "../Section.h"
#include "../Intersection.h"
#include "../Border.h"
#include "../GVManager.h"
#include "../AmuConverter.h"

using namespace std;

//======================================================================
SectionTramExt::SectionTramExt(Section* section)
{
    _section = section;
}

//======================================================================
void SectionTramExt::getTramLanesFrom(vector<Lane*>* result_lanes,
                                      const Intersection* inter)
{
    BorderTram* borderTram
        = dynamic_cast<BorderTram*>
        (const_cast<Border*>(inter->border
                             (inter->direction(_section))));
   
    if (!borderTram)
    {
        return;
    }

    const Connector* connector = borderTram->outPointTram();
    if (!connector)
    {
        return;
    }

    vector<Lane*> lanes = _section->lanesFromConnector(connector);
    for (unsigned int i=0; i<lanes.size(); i++)
    {
        result_lanes->push_back(lanes[i]);
    }
}

//======================================================================
void SectionTramExt::getTramLanesTo(vector<Lane*>* result_lanes,
                                    const Intersection* inter)
{
    BorderTram* borderTram
        = dynamic_cast<BorderTram*>
        (const_cast<Border*>(inter->border
                             (inter->direction(_section))));
   
    if (!borderTram)
    {
        return;
    }

    const Connector* connector = borderTram->inPointTram();
    vector<Lane*> lanes = _section->lanesToConnector(connector);
    for (unsigned int i=0; i<lanes.size(); i++)
    {
        result_lanes->push_back(lanes[i]);
    }
}

//======================================================================
bool SectionTramExt::createTramLanesInSection()
{
    for (int i=0; i<2; i++)
    {
        Intersection* from = _section->intersection(i);
        Intersection* to   = _section->intersection((i+1)%2);

        const Border* borderBegin
            = from->border(from->direction(to));
        const Border* borderEnd
            = to->border(to->direction(from));

        BorderTram* borderTramBegin
            = dynamic_cast<BorderTram*>
            (const_cast<Border*>(borderBegin));
        BorderTram* borderTramEnd
            = dynamic_cast<BorderTram*>
            (const_cast<Border*>(borderEnd));

        if (!borderTramBegin
            || !borderTramEnd)
        {
            return true;
        }
        else if (!(borderTramBegin->outPointTram())
                 || !(borderTramEnd->inPointTram()))
        {
            continue;
        }

        int idIntBegin
            = i*100
            + borderBegin->numIn() + borderBegin->numOut();
        int idIntEnd
            = ((i+1)%2)*100
            + borderEnd->numIn() + borderEnd->numOut()
            + ((dynamic_cast<BorderTram*>
                (const_cast<Border*>(borderEnd)))->inPointTram()?1:0);
        int idInt
            = idIntBegin*10000 + idIntEnd;

        string id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);

        TramLaneInSection* lane
            = new TramLaneInSection(id,
                                    borderTramBegin->outPointTram(),
                                    borderTramEnd->inPointTram(),
                                    _section);
        lane->setSpeedLimit
            (GVManager::getNumeric("TRAM_SPEED_LIMIT"));

        _section->addLane(lane);
    }

    return true;
}

#endif //INCLUDE_TRAMS

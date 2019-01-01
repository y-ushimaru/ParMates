/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "ODNodeTramExt.h"
#include "SectionTramExt.h"
#include "VehicleTram.h"
#include "../RoadMap.h"
#include "../ODNode.h"
#include "../Section.h"
#include "../Lane.h"
#include "../RoadOccupant.h"
#include "../Vehicle.h"
#include "../ObjManager.h"
#include "../TimeManager.h"
#include "../VehicleIO.h"
#include <cassert>

using namespace std;

//======================================================================
ODNodeTramExt::ODNodeTramExt(ODNode* odNode)
{
    _odNode = odNode;
}

//======================================================================
ODNodeTramExt::~ODNodeTramExt()
{
}

//======================================================================
void ODNodeTramExt::putTram(RoadMap* roadMap,
                            deque<Vehicle*>* waitingVehicles,
                            vector<Vehicle*>* skipVehicles)
{
    Section* section = _odNode->nextSection(0);
    vector<Lane*> lanes;
    section->tramExt()->getTramLanesFrom(&lanes, _odNode);

    VehicleTram* tmpVehicle
        = dynamic_cast<VehicleTram*>((*waitingVehicles).front());
    (*waitingVehicles).pop_front();

    assert(tmpVehicle);

    if (lanes[0]->tailAgent()==NULL
        || lanes[0]->tailAgent()->length()
        - lanes[0]->tailAgent()->bodyLength()/2
        - tmpVehicle->bodyLength()/2
        > 1.0)
    {
        // 経路の指定
        tmpVehicle->setRoute();

        // 登場時刻の記録
        tmpVehicle->setStartTime(TimeManager::time());

        // 道路上に登場
        tmpVehicle->addToSection(roadMap,
                                 section,
                                 lanes[0],
                                 tmpVehicle->bodyLength()/2);

        bool result = ObjManager::addVehicleToReal(tmpVehicle);
        assert(result);

        // 作成した車両属性情報を出力する
        /*
         * addVehicleToRealの後でなければIDが決まらないため，
         * ここで出力する
         */
        VehicleIO::instance().writeVehicleStaticData(tmpVehicle);

        // 路面電車はGEN_COUNTERの出力対象としない
    }
    else
    {
        skipVehicles->push_back(tmpVehicle);
    }
}

#endif //INCLUDE_TRAMS

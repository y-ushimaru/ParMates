/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include "Simulator.h"
#include "RoutingManager.h"
#include "PFBase.h"
#include "PFAStar.h"
#include "PFAStarHierarchy.h"
#include "PFLogger.h"
#include "Random.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "GenerateVehicleController.h"
#include "Vehicle.h"
#include <autogl.h>
#include <cassert>
#include <cstring>

using namespace std;

//======================================================================
void Visualizer::searchVehicle()
{
    string vehicleId
        = AmuConverter::formatId(_infoVehicleId,
                                 NUM_FIGURE_FOR_VEHICLE);
    Vehicle* tmpVehicle = ObjManager::vehicle(vehicleId);
    if (!tmpVehicle)
    {
        cerr << "cannot find vehicle " << vehicleId << endl;
    }
    else
    {
        // ビューの中心を移動する
        AutoGL_SetViewCenter(tmpVehicle->x(),
                             tmpVehicle->y(),
                             tmpVehicle->z());
        clearTempFlag();
        AutoGL_DrawView();
    }
}

//======================================================================
void Visualizer::showVehicleInfo()
{
    string vehicleId = AmuConverter::formatId(_infoVehicleId,
                                              NUM_FIGURE_FOR_VEHICLE);
    Vehicle* tmpVehicle = ObjManager::vehicle(vehicleId);
    if (tmpVehicle==NULL)
    {
        cerr << "cannot find vehicle " << vehicleId << endl;
    }
    else
    {
        tmpVehicle->print();
    }
}

//======================================================================
void Visualizer::searchIntersection()
{
    string intersectionId
        = AmuConverter::formatId(_infoIntersectionId,
                                 NUM_FIGURE_FOR_INTERSECTION);
    Intersection* tmpInter
        = _sim->roadMap()->intersection(intersectionId);
    if (!tmpInter)
    {
        cerr << "cannot find intersection " << intersectionId << endl;
    }
    else
    {
        // ビューの中心を移動する
        AutoGL_SetViewCenter(tmpInter->center().x(),
                             tmpInter->center().y(),
                             tmpInter->center().z());
        clearTempFlag();
        AutoGL_DrawView();
    }
}

//======================================================================
void Visualizer::showIntersectionInfo()
{
    string intersectionId
        = AmuConverter::formatId(_infoIntersectionId,
                                 NUM_FIGURE_FOR_INTERSECTION);
    Intersection* tmpInter
        = _sim->roadMap()->intersection(intersectionId);
    if (!tmpInter)
    {
        cerr << "cannot find intersection " << intersectionId << endl;
    }
    else
    {
        tmpInter->printDetail(dynamic_cast<ODNode*>(tmpInter) != NULL);
    }
}

//======================================================================
void Visualizer::generateVehicleManual()
{
    // start,goalともODノードが実在するかどうかは
    // Simulator::generateVehicleManual()で判断する．

    // startIdの入力は必須
    if (strcmp(_genVehicleStart, "")==0)
    {
        cout << "input origin node ID." << endl;
        return;
    }
    string startId
        = AmuConverter::formatId(_genVehicleStart,
                                 NUM_FIGURE_FOR_INTERSECTION);

    // goalIdは入力されなければランダム
    string goalId;
    if (strcmp(_genVehicleGoal, "")==0)
    {
        goalId = "******";
    }
    else
    {
        goalId
            = AmuConverter::formatId(_genVehicleGoal,
                                     NUM_FIGURE_FOR_INTERSECTION);
    }

    // 経路選択パラメータの切り出し
    vector<double> params;
    params.resize(VEHICLE_ROUTING_PARAMETER_SIZE);
    params[0] = 1;
    for (unsigned int i=1; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        params[i] = 0;
    }
    string str;
    vector<string> tokens;
    str = _genVehicleParams;
    AmuStringOperator::getAdjustString(&str);
    AmuStringOperator::getTokens(&tokens, str, ',');
    if (str!="")
    {
        for (unsigned int i=0; i<tokens.size(); i++)
        {
            // isNumeric 
            params.push_back(AmuConverter::strtod(tokens[i]));
            if (i >= VEHICLE_ROUTING_PARAMETER_SIZE)
            {
                break;
            }
        }
        tokens.clear();
    }

    // 選好するネットワークランク
    int prefRank;
    if (strcmp(_genVehiclePrefRank, "")==0)
    {
        prefRank = INT_MAX;
    }
    else
    {
        prefRank = atoi(_genVehiclePrefRank);
    }
    
    // 経由地の切り出し
    vector<string> gateIds;
    gateIds.clear();
    gateIds.push_back(startId);

    str = _genVehicleStopPoints;
    AmuStringOperator::getAdjustString(&str);
    AmuStringOperator::getTokens(&tokens, str, ',');
    if (str!="")
    {
        for (unsigned int i=0; i<tokens.size(); i++)
        {
            gateIds.push_back
                (AmuConverter::formatId(tokens[i],
                                        NUM_FIGURE_FOR_INTERSECTION));
        }
    }
    gateIds.push_back(goalId);

    // 入力された情報から車両を生成
    _sim->generateVehicleController()
        ->generateVehicleManual(startId, goalId, &gateIds,
                                _genVehicleType, params, prefRank);

}

//======================================================================
void Visualizer::searchRoute()
{
    //------------------------------------------------------------------
    // 始点，終点を決定
    string fromId
        = AmuConverter::formatId(_routingFromIntersection,
                                 NUM_FIGURE_FOR_INTERSECTION);
    Intersection* from
        = _sim->roadMap()->intersection(fromId);
    if (!from)
    {
        cerr << "cannot find intersection " << fromId << endl;
        return;
    }
    
    string nextId
        = AmuConverter::formatId(_routingNextIntersection,
                                 NUM_FIGURE_FOR_INTERSECTION);
    Intersection* next
        = _sim->roadMap()->intersection(nextId);
    if (!next)
    {
        cerr << "cannot find intersection " << nextId << endl;
        return;
    }

    // from->nextが接続しているかどうか判別
    const Section* section = from->nextSection(next);
    if (!section
        || (section->lanesFrom(from)).size()==0)
    {
        cerr << "cannot find section " << fromId
             << "->" << nextId << endl;
        return;
    }

    string goalId
        = AmuConverter::formatId(_routingGoalIntersection,
                                 NUM_FIGURE_FOR_INTERSECTION);
    Intersection* goal
        = _sim->roadMap()->intersection(goalId);
    if (!goal)
    {
        cerr << "cannot find intersection " << goalId << endl;
        return;
    }

    //------------------------------------------------------------------
    // from，via，toをゲートとして登録
    Route aRoute;
    aRoute.addGate(from);

    string str = _routingStopPoints;
    AmuStringOperator::getAdjustString(&str);
    if (!str.empty())
    {
        vector<string> tokens;
        AmuStringOperator::getTokens(&tokens, str, ',');

        for (unsigned int i=0; i<tokens.size(); i++)
        {
            string gateId
                = AmuConverter::formatId(tokens[i],
                                         NUM_FIGURE_FOR_INTERSECTION);
            Intersection* gate
                = _sim->roadMap()->intersection(gateId);
            if (!gate)
            {
                cerr << "cannot find intersection " << gateId << endl;
            }
            else
            {
                aRoute.addGate(gate);
            }
        }
    }
    aRoute.addGate(goal);

    //------------------------------------------------------------------
    // 選好ネットワークランクを決定
    int prefRank = INT_MAX;
    stringstream ss;
    ss << _routingPrefRank;
    int rank = -1;
    ss >> rank;
    if (rank >= 0)
    {
        prefRank = rank;
    }

    //------------------------------------------------------------------
    PFBase* pathfinder = _sim->routingManager()->assignPathfinder();
    double w[VEHICLE_ROUTING_PARAMETER_SIZE];
    w[0] = 1.0;
    for (unsigned int i=1; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        w[i] = 0.0;
    }
    pathfinder->setWeights(w);
    if (dynamic_cast<PFAStarHierarchy*>(pathfinder))
    {
        dynamic_cast<PFAStarHierarchy*>(pathfinder)
            ->setPreferredNetworkRank(prefRank);
    }
    pathfinder->setRandomGenerator(_rnd);

    _pfLogger->reset();
    pathfinder->addLogger(_pfLogger);
    bool result
        = pathfinder->search(&aRoute, from, next);
    pathfinder->removeLogger();
    _pfLogger->printLog();

    if (!result)
    {
        cout << "route search failed." << endl;
        aRoute.print(cout);
    }
    else
    {
        cout << "search result:" << endl;
        aRoute.print(cout);
    }
    _sim->routingManager()->releasePathfinder(pathfinder);
}

//======================================================================
void Visualizer::incrementSearchRouteLog()
{
    if (_routingLogCount>=_pfLogger->logSize()-1)
    {
        return;
    }
    _routingLogCount++;
    showSearchRouteLog();
}

//======================================================================
void Visualizer::decrementSearchRouteLog()
{
    if (_routingLogCount<1)
    {
        return;
    }
    _routingLogCount--;
    showSearchRouteLog();
}

//======================================================================
void Visualizer::resetSearchRouteLog()
{
    _routingLogCount = 0;
    showSearchRouteLog();
}

//======================================================================
void Visualizer::showSearchRouteLog()
{
    _routingLogIsShown = 1;
    AutoGL_DrawView();
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoutingManager.h"
#include "RoutingNetworkBuilder.h"
#include "RoadMap.h"
#include "Section.h"
#include "Intersection.h"
#include "RelativeDirectionTable.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "RouteIO.h"
#include "PFBase.h"
#include "PFAStar.h"
#include "PFAStarHierarchy.h"
#include "PFDijkstra.h"
#include "RouteStorage.h"
#include "GVManager.h"
#include <cassert>

using namespace std;

//======================================================================
RoutingManager::RoutingManager()
{
    _routingNetwork.clear();
    _maxRank = 0;
    _pathfinders.clear();
    _usedRouteStorage.clear();
}

//======================================================================
RoutingManager::~RoutingManager()
{
    deleteAll();
}

//======================================================================
void RoutingManager::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

//======================================================================
RoutingNetwork* RoutingManager::routingNetwork()
{
    return _routingNetwork[0];
}

//======================================================================
RoutingNetwork* RoutingManager::routingNetwork(unsigned int rank)
{
    if (rank>=_routingNetwork.size())
    {
        return NULL;

    }
    else
    {
        return _routingNetwork[rank];
    }
}

//======================================================================
unsigned int RoutingManager::maxRank() const
{
    return _maxRank;
}

//======================================================================
bool RoutingManager::getReadyRoutingNetwork()
{
    cout << "*** Network for Routing ***" << endl;

    // RoutingNetworkの作成
    RoutingNetworkBuilder builder;
    builder.setRoadMap(_roadMap);
    builder.setRoutingNetworkContainer(&_routingNetwork);

    // 最下位レイヤのネットワーク生成
    /*
     * オリジナルの道路ネットワークに対し，
     * セクションの上り下りを区別したうえで
     * 線グラフ(Line Graph)を作成する．
     * これをランク0のネットワークとする．
     */
    builder.buildRoutingNetwork(0);
    _routingNetwork.push_back(builder.routingNetwork());
    _maxRank = _routingNetwork[0]->maxNodeRank();
    // maxRank = 3;

    // 上位レイヤのネットワーク作成
    for (unsigned int i=1; i<=_maxRank; i++)
    {
        builder.buildRoutingNetwork(i);
        _routingNetwork.push_back(builder.routingNetwork());
        _routingNetwork[i-1]->setUpperNetwork(_routingNetwork[i]);
        _routingNetwork[i]->setLowerNetwork(_routingNetwork[i-1]);
    }

    // レイヤ間リンクの生成
    for (unsigned int i=0; i<=_maxRank; i++)
    {
        builder.buildInterlayerLinks(i);
    }

    // RoutingNode, RoutingLinkの属性の付与
    for (unsigned int i=0; i<=_maxRank; i++)
    {
        builder.setNodeProperties(i);
        builder.setLinkProperties(i);
    }

    // 初期リンクコストの付与
    for (unsigned int i=0; i<=_maxRank; i++)
    {
        _routingNetwork[i]->setInitialCosts();
    }

    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        cout << endl;
        for (unsigned int i=0; i<=_maxRank; i++)
        {
            if (!_routingNetwork[i])
            {
                continue;
            }
            cout << "RoutingNetwork[" << i << "]" << endl;
            cout << "  Node: "
                 << _routingNetwork[i]->nodes()->size() << endl;
            cout << "  Link: "
                 << _routingNetwork[i]->links()->size() << endl << endl;
        }
    }

    /*
    for (unsigned int i=0; i<=_maxRank; i++)
    {
        _routingNetwork[i]->print();
    }
    */

    return true;
}

//======================================================================
bool RoutingManager::renewCosts()
{
    unsigned int maxRank = _routingNetwork[0]->maxNodeRank();
    for (unsigned int i=0; i<=maxRank; i++)
    {
        _routingNetwork[i]->renewCosts();
    }
    return true;
}

//======================================================================
void RoutingManager::addUsedRouteStorage(RouteStorage* routeStorage)
{
    _usedRouteStorage.push_back(routeStorage);
}

//======================================================================
void RoutingManager::renewRouteStorage()
{
    for (unsigned int i=0; i<_usedRouteStorage.size(); i++)
    {
        _usedRouteStorage[i]->renewRouteComponent();
    }
    _usedRouteStorage.clear();
}

//======================================================================
void RoutingManager::deleteAllRouteStorage()
{
    const RMAPI* inters = _roadMap->intersections();
    for (CITRMAPI citi = inters->begin();
         citi != inters->end();
         citi++)
    {
        (*citi).second->deleteRouteStorage();
    }
}

//======================================================================
PFBase* RoutingManager::assignPathfinder()
{
    PFBase* result = NULL;

#ifdef _OPENMP
#pragma omp critical (assignPathfinder)
    {
#endif //_OPENMP
        for (unsigned int i=0; i<_pathfinders.size(); i++)
        {
            // 使用中でないPathfinderを探す
            if (!(_pathfinders[i]->isInUse()))
            {
                result = _pathfinders[i];
                break;
            }
        }
        // 見つからなかった場合は新規作成
        if (!result)
        {
            // result = new PFAStar(_routingNetwork[0]);
            result = new PFAStarHierarchy(this,
                                          _routingNetwork[0]);
            _pathfinders.push_back(result);
        }
        result->setInUseOn();
#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//======================================================================
void RoutingManager::releasePathfinder(PFBase* pathfinder)
{
#ifdef _OPENMP
#pragma omp critical (releasePathfinder)
    {
#endif //_OPENMP

        pathfinder->setInUseOff();

#ifdef _OPENMP
    }
#endif //_OPENMP

}

//======================================================================
void RoutingManager::deleteAll()
{
    for (unsigned int i=0; i<_routingNetwork.size(); i++)
    {
        delete _routingNetwork[i];
    }
    _routingNetwork.clear();
    deleteAllPathfinders();
    // deleteAllRouteStorage();
}

//======================================================================
void RoutingManager::deleteAllPathfinders()
{
    for (unsigned int i=0; i<_pathfinders.size(); i++)
    {
        delete _pathfinders[i];
    }
    _pathfinders.clear();
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RouteIO.h"
#include "RoadMap.h"
#include "Intersection.h"
#include "RoutingManager.h"
#include "RouteStorage.h"
#include "PFBase.h"
#include "GVManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
#include <typeinfo>

using namespace std;

//======================================================================
RouteIO& RouteIO::instance()
{
    static RouteIO instance;
    return instance;
}

//======================================================================
void RouteIO::readRouteCache(RoadMap* roadMap,
                             RoutingManager* routingManager)
{
    string fRoute;
    GVManager::getVariable("CACHE_ROUTING_FILE", &fRoute);

    ifstream fin(fRoute.c_str(), ios::in);
    if (!fin.good())
    {
        cerr << "no route log file: " << endl
             << "  " << fRoute << endl;
        return;
    }

    // ファイルの読み込み
    if (GVManager::getFlag("FLAG_VERBOSE"))
    {
        cout << "*** Read route log ***" << endl;
    }
    string str;

    /*
     * 入力情報は経路探索器に依存する
     */
    PFBase* pathfinder = routingManager->assignPathfinder();

    // 1行目の処理
    getline(fin, str);
    AmuStringOperator::getAdjustString(&str);
    if (str!=typeid(*pathfinder).name())
    {
        cout << "WARNING: route log is invalid for current pathfinder."
             << endl;
        fin.close();
        routingManager->releasePathfinder(pathfinder);
        return;
    }

    // 2行目以降の処理
    /*
     * 行の解釈は経路探索器に依存するため，実質的には
     * PFBase::generateRouteComponentに任せる
     */
    int numFinishedLines = 1;
    while (fin.good())
    {
        getline(fin, str);
        numFinishedLines++;
        if (numFinishedLines%10000==0
            && GVManager::getFlag("FLAG_VERBOSE"))
        {
            cout << "... " << numFinishedLines
                 << " lines processed." << endl;
        }

        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;

            AmuStringOperator::getTokens(&tokens, str, ',');
            bool result
                = pathfinder
                ->generateRouteComponent(roadMap, tokens);

            if (!result)
            {
                cerr << "invalid line: " << str << endl;
            }
        }
    }

    routingManager->releasePathfinder(pathfinder);
    fin.close();
}

//======================================================================
void RouteIO::writeRouteCache(RoadMap* roadMap,
                              RoutingManager* routingManager)
{
    cout << "RouteIO::writeRouteCache" << endl;
    string fRoute;
    GVManager::getVariable("CACHE_ROUTING_FILE", &fRoute);

    // これまでの結果はメモリ上に持っているため内容を置換して構わない
    ofstream fout(fRoute.c_str(), ios::out|ios::trunc);
    if (!fout.good())
    {
        cerr << "no route log file: "
             << fRoute << endl;
        return;
    }

    /*
     * 出力情報は経路探索器に依存する
     */
    PFBase* pathfinder
        = routingManager->assignPathfinder();
    fout << typeid(*pathfinder).name() << endl;

    CITRMAPI iti = roadMap->intersections()->begin();
    while (iti!=roadMap->intersections()->end())
    {
        (*iti).second->routeStorage()->printStatic(fout);
        iti++;
    }

    routingManager->releasePathfinder(pathfinder);
    fout.close();
}

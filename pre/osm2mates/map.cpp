/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップ
//マップエディタの処理を元に作成

#include <Qt>
#include <vector>
#include <iostream>
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "road.h"

using namespace std;

#define ROAD_ID_MIN     0                           //単路ID最小／最大
#define ROAD_ID_MAX     999999999

//キーは演算可能でないと駄目、独自クラス困難、単路IDにして交差点側に交差点IDマップを入れる
//両方単路クラスへのポインタを持つがマップが主体、交差点はマップに合わせる

//新規 ID は空きでなく最大、仕様書指定、その方が後で扱いやすいか、作成時間順に並ぶ

//コンストラクタ
Map::Map()
{
}

//デストラクタ
Map::~Map()
{
    deleteAll();
}

//全削除
void Map::deleteAll()
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             intsec;
    MAP_ROADMAP_IT      rmi;
    Road*               road;

    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        intsec = imi->second;
        delete intsec;
    }
    _intsecMap.clear();
    for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
    {
        road = rmi->second;
        delete road;
    }
    _roadMap.clear();
}

//交差点作成取得、ID 指定なしなら最大 ID の次、既存 ID なら NULL
Intsec* Map::createIntsec(int id)
{
    MAP_INTSECMAP_IT	imi;
    Intsec*             newIntsec;

    if (id == MAP_NOID)
    {        
        id = MAP_INTSEC_ID_MIN;
        if (_intsecMap.size() != 0)
        {
            imi = _intsecMap.end();;
            imi--;
            id = imi->first + 1;
        }
//      if (id > MAP_INTSEC_ID_MAX)  //　ＩＤを振りなおすので、ここではチェックしない
//          return NULL;
    }
    else
    {
        if (_intsecMap.find(id) != _intsecMap.end())
            return NULL;
    }

//  qDebug("Map::createIntsec %d", id);
    newIntsec = new Intsec(id);
    _intsecMap.insert(pair<int, Intsec*>(id, newIntsec));

    return newIntsec;
}

//交差点検索
Intsec* Map::findIntsec(int id)
{
    MAP_INTSECMAP_IT        imi;

//  qDebug("Map::intsec %d", id);
    imi = _intsecMap.find(id);
    if (imi == _intsecMap.end())
        return NULL;
    return imi->second;
}

//次の交差点取得、first なら先頭から、なしなら NULL
Intsec* Map::nextIntsec(MAP_INTSECMAP_IT* imi, bool first)
{
    if (first)
        *imi = _intsecMap.begin();
    else
        (*imi)++;

    if (*imi == _intsecMap.end())
        return NULL;
    return (*imi)->second;
}

//交差点削除、接続単路も削除、なしなら無視
void Map::deleteIntsec(int id)
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             intsec;
    INTSEC_ROADMAP_IT   irmi;
    Road*               road;
    vector<int>         deleteRoadId;
    int                 i;

//  qDebug("Map::deleteIntsec %d", id);
    imi = _intsecMap.find(id);
    if (imi == _intsecMap.end())
        return;
    intsec = imi->second;

    //単路削除時に接続情報を消すので後でまとめて実行
    road = intsec->nextRoad(&irmi, true);
    while (road != NULL)
    {
        deleteRoadId.push_back(road->id());
        road = intsec->nextRoad(&irmi);
    }
    for (i=0; i < (int)deleteRoadId.size(); i++)
        deleteRoad(deleteRoadId[i]);

    delete intsec;
    _intsecMap.erase(imi);
}

//単路作成取得、既存なら既存単路を返す
Road* Map::createRoad(Intsec* intsec1, Intsec* intsec2, int defLane, bool* already)
{
    Road*           road;
    MAP_ROADMAP_IT  rmi;
    int             roadId;

//  qDebug("Map::createRoad %d-%d", pIntsec1->id(), pIntsec2->id());
    road = findRoad(intsec1->id(), intsec2->id());
    if (road != NULL)
    {
        if (already != NULL)
            *already = true;
        return road;
    }
    if (already != NULL)
        *already = false;
    roadId = ROAD_ID_MIN;
    if (_roadMap.size() != 0)
    {
        rmi = _roadMap.end();;
        rmi--;
        roadId = rmi->first + 1;
    }
    Q_ASSERT(roadId <= ROAD_ID_MAX);
//  qDebug("Map::createRoad %d", roadId);
    road = new Road(roadId, intsec1, intsec2, defLane);
    _roadMap.insert(pair<int, Road*>(roadId, road));
    intsec1->addRoad(intsec2->id(), road);
    intsec2->addRoad(intsec1->id(), road);

    return road;
}

//一括道路作成処理
void Map::createRoadPackage(Intsec* intsec1, Intsec* intsec2,
                            int laneForward, int laneBackward)
{
    //境界判定、既存道路ならレーンを既存に追加、これで重複道路を重ねてしまう
    if (!intsec1->border() || !intsec2->border())
    {
        Road* road = app.map()->createRoad(intsec1, intsec2, 0);
        road->addLanes(intsec1, laneBackward, laneForward,
                                laneForward,  laneBackward);
    }
}

//単路検索、なしなら NULL
Road* Map::findRoad(int intsecId1, int intsecId2)
{
    Intsec*     intsec;

//  qDebug("Map::road %d-%d", intsecId1, intsecId2);
    intsec = findIntsec(intsecId1);
    if (intsec != NULL)
        return intsec->road(intsecId2);

//  qDebug("Map::road null end %d-%d", intsecId1, intsecId2);
    return NULL;
}

//次の単路取得、first なら先頭から、なしなら NULL
Road* Map::nextRoad(MAP_ROADMAP_IT* rmi, bool first)
{
    if (first)
        *rmi = _roadMap.begin();
    else
        (*rmi)++;

    if (*rmi == _roadMap.end())
        return NULL;
    return (*rmi)->second;
}

//単路削除、なしなら無視
void Map::deleteRoad(int roadId)
{
    MAP_ROADMAP_IT      rmi;
    Road*               road;
    Intsec*             intsec1;
    Intsec*             intsec2;

    rmi = _roadMap.find(roadId);
    if (rmi != _roadMap.end())
    {
        road = rmi->second;
        intsec1 = road->intsec(ROAD_IT_SMALL);
        intsec2 = road->intsec(ROAD_IT_LARGE);
//      qDebug("Map::deleteRoad %d %d-%d 0x%x",
//             roadId, pIntsec1->id(), pIntsec2->id(), pRoad);
        intsec1->deleteRoad(intsec2->id());
        intsec2->deleteRoad(intsec1->id());
        delete road;
        _roadMap.erase(rmi);
    }
//  qDebug("Map::deleteRoad end %d", roadId);
}

//整形
void Map::refine()
{
    MAP_INTSECMAP_IT imi;
    INTSEC_ROADMAP_IT irmi;
    Intsec* intsec;
    Road* road;
    vector<int> ids;
    int idCon;

    //道路なしの交差点と、自分への道路を消す
    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        intsec = imi->second;
        if (intsec->roadNum() == 0)
            ids.push_back(intsec->id());
    }
    for (int i = 0; i < (int)ids.size(); i++)
    {
//      cout << "intsec " << ids[i] << endl;
        deleteIntsec(ids[i]);
    }
    ids.clear();
    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        intsec = imi->second;
        road = intsec->nextRoad(&irmi, true, &idCon);
        while (road != NULL)
        {
            if (intsec->id() == idCon)
                ids.push_back(road->id());
            road = intsec->nextRoad(&irmi, false, &idCon);
        }
    }
    for (int i = 0; i < (int)ids.size(); i++)
    {
//      cout << "road " << ids[i] << endl;
        deleteRoad(ids[i]);
    }
}

//強制１レーン
void Map::forceOneLane()
{
    MAP_ROADMAP_IT      rmi;
    Road*               road;
    int                 modForceOneLane;

    cout << "force one lane..." << endl;

    modForceOneLane = app.modForceOneLane();

    for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
    {
        road = rmi->second;
        for (int i = 0; i < ROAD_IT_NUM; i++)
        {
            for (int j = 0; j < ROAD_LT_NUM; j++)
            {
                if (road->lane(i, j) <= modForceOneLane)
                    road->setLane(i, j, 1);
            }
        }
    }
}

//交差点最終ＩＤ設定
void Map::setIntsecFinalId()
{
    MAP_INTSECMAP_IT imi;
    int finalId = 0;
    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
        _intsecFinalId.insert(pair<int, int>(imi->first, finalId++));
}

//交差点最終ＩＤ取得
int Map::getIntsecFinalId(int id)
{
    map<int, int>::iterator iti = _intsecFinalId.find(id);
    Q_ASSERT(iti!=_intsecFinalId.end());
    return iti->second;
}



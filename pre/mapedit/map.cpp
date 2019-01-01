/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップ

#include <vector>
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
    _pIntsecCommon = NULL;
}

//デストラクタ
Map::~Map()
{
    if (_pIntsecCommon!=NULL)
        delete _pIntsecCommon;
    deleteAll();
}

//初期処理
void Map::init()
{
    _pIntsecCommon = new IntsecCommon;
}

//中心座標設定
void Map::setCenter(double mapX, double mapY, double mapZ)
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;

    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        pIntsec = imi->second;
        pIntsec->setMapPos(pIntsec->getMapPosX() - mapX,
                           pIntsec->getMapPosY() - mapY,
                           pIntsec->getMapPosZ() - mapZ);
    }
}

//全削除
void Map::deleteAll()
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;

    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        pIntsec = imi->second;
        delete pIntsec;
    }
    _intsecMap.clear();
    for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
    {
        pRoad = rmi->second;
        delete pRoad;
    }
    _roadMap.clear();
}

//デバッグトレース
void Map::debugTrace()
{
    MAP_INTSECMAP_IT	imi;
    Intsec*             pIntsec;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;

    for (imi = _intsecMap.begin(); imi != _intsecMap.end(); imi++)
    {
        pIntsec = imi->second;
        pIntsec->debugTrace();
    }
    for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
    {
        pRoad = rmi->second;
        pRoad->debugTrace();
    }
}

//交差点作成取得、ID 指定なしなら最大 ID の次、ID が大きすぎか既存 ID なら NULL
Intsec* Map::createIntsec(int intsecId)
{
    MAP_INTSECMAP_IT	imi;
    Intsec*             pNewIntsec;

    if (intsecId == MAP_NOID)
    {        
        intsecId = MAP_INTSEC_ID_MIN;
        if (_intsecMap.size() != 0)
        {
            imi = _intsecMap.end();;
            imi--;
            intsecId = imi->first + 1;
        }
        if (intsecId > MAP_INTSEC_ID_MAX)
            return NULL;
    }
    else
    {
        if (_intsecMap.find(intsecId) != _intsecMap.end())
            return NULL;
    }

//  qDebug("Map::createIntsec %d", intsecId);
    pNewIntsec = new Intsec(intsecId);
    _intsecMap.insert(pair<int, Intsec*>(intsecId, pNewIntsec));

    return pNewIntsec;
}

//交差点取得
Intsec* Map::getIntsec(int intsecId)
{
    MAP_INTSECMAP_IT        imi;

//  qDebug("Map::getIntsec %d", intsecId);
    imi = _intsecMap.find(intsecId);
    if (imi == _intsecMap.end())
        return NULL;
    return imi->second;
}

//次の交差点取得、first なら先頭から、なしなら NULL
Intsec* Map::nextIntsec(MAP_INTSECMAP_IT* pimi, bool first)
{
    if (first)
        *pimi = _intsecMap.begin();
    else
        (*pimi)++;

    if (*pimi == _intsecMap.end())
        return NULL;
    return (*pimi)->second;
}

//交差点削除、接続単路も削除、なしなら無視
void Map::deleteIntsec(int intsecId)
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    INTSEC_ROADMAP_IT   irmi;
    Road*               pRoad;
    vector<int>         deleteRoadId;
    int                 i;

//  qDebug("Map::deleteIntsec %d", intsecId);
    imi = _intsecMap.find(intsecId);
    if (imi == _intsecMap.end())
        return;
    pIntsec = imi->second;

    //単路削除時に接続情報を消すので後でまとめて実行
    pRoad = pIntsec->nextRoad(&irmi, true);
    while (pRoad != NULL)
    {
        deleteRoadId.push_back(pRoad->getRoadId());
        pRoad = pIntsec->nextRoad(&irmi);
    }
    for (i=0; i < (int)deleteRoadId.size(); i++)
        deleteRoad(deleteRoadId[i]);

    delete pIntsec;
    _intsecMap.erase(imi);
}

//交差点ID変更、構造的変換なので注意、既存 ID 不可、交差点をもらって戻す、元は使えないので注意
Intsec* Map::changeIntsecId(Intsec* pIntsec, int newIntsecId)
{
    Intsec*             pIntsecNew;
    Intsec*             pIntsecCon;
    INTSEC_ROADMAP_IT   irmi;
    Road*               pRoad;
    int                 firstIntsecIdCon, intsecIdCon;

    firstIntsecIdCon = pIntsec->getFirstIntsecIdCon();
    pIntsecNew = createIntsec(newIntsecId);
    Q_ASSERT(pIntsecNew != NULL);
    pIntsecNew->copyMapPos(pIntsec);
    pRoad = pIntsec->nextRoad(&irmi, true, &intsecIdCon);
    while (pRoad != NULL)
    {
        pIntsecCon = getIntsec(intsecIdCon);
        Q_ASSERT(pIntsecCon != NULL);
        replaceRoad(pIntsec, pIntsecNew, pIntsecCon);
        pRoad = pIntsec->nextRoad(&irmi, false, &intsecIdCon);
    }
    if (firstIntsecIdCon != MAP_NOID)
        pIntsecNew->setFirstIntsecIdCon(firstIntsecIdCon);
    deleteIntsec(pIntsec->getIntsecId());
    return pIntsecNew;
}

//単路作成取得、既存なら既存単路を返す
Road* Map::createRoad(Intsec* pIntsec1, Intsec* pIntsec2, bool* pAlready)
{
    Road*           pRoad;
    MAP_ROADMAP_IT  rmi;
    int             roadId;

//  qDebug("Map::createRoad %d-%d", pIntsec1->getIntsecId(), pIntsec2->getIntsecId());
    pRoad = getRoad(pIntsec1->getIntsecId(), pIntsec2->getIntsecId());
    if (pRoad != NULL)
    {
        *pAlready = true;
        return pRoad;
    }
    *pAlready = false;
    roadId = ROAD_ID_MIN;
    if (_roadMap.size() != 0)
    {
        rmi = _roadMap.end();;
        rmi--;
        roadId = rmi->first + 1;
    }
    Q_ASSERT(roadId <= ROAD_ID_MAX);
//  qDebug("Map::createRoad %d", roadId);
    pRoad = new Road(roadId, pIntsec1, pIntsec2);
    _roadMap.insert(pair<int, Road*>(roadId, pRoad));
    pIntsec1->addRoad(pIntsec2->getIntsecId(), pRoad);
    pIntsec2->addRoad(pIntsec1->getIntsecId(), pRoad);

    return pRoad;
}

//単路取得、なしなら NULL
Road* Map::getRoad(int intsecId1, int intsecId2)
{
    Intsec*     pIntsec;

//  qDebug("Map::getRoad %d-%d", intsecId1, intsecId2);
    pIntsec = getIntsec(intsecId1);
    if (pIntsec != NULL)
        return pIntsec->getRoad(intsecId2);

//  qDebug("Map::getRoad null end %d-%d", intsecId1, intsecId2);
    return NULL;
}

//次の単路取得、first なら先頭から、なしなら NULL
Road* Map::nextRoad(MAP_ROADMAP_IT* prmi, bool first)
{
    if (first)
        *prmi = _roadMap.begin();
    else
        (*prmi)++;

    if (*prmi == _roadMap.end())
        return NULL;
    return (*prmi)->second;
}

//単路削除、なしなら無視
void Map::deleteRoad(int roadId)
{
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;
    Intsec*             pIntsec1;
    Intsec*             pIntsec2;

    rmi = _roadMap.find(roadId);
    if (rmi != _roadMap.end())
    {
        pRoad = rmi->second;
        pIntsec1 = pRoad->getIntsec(ROAD_IT_SMALL);
        pIntsec2 = pRoad->getIntsec(ROAD_IT_LARGE);
//      qDebug("Map::deleteRoad %d %d-%d 0x%x",
//             roadId, pIntsec1->getIntsecId(), pIntsec2->getIntsecId(), pRoad);
        pIntsec1->deleteRoad(pIntsec2->getIntsecId());
        pIntsec2->deleteRoad(pIntsec1->getIntsecId());
        delete pRoad;
        _roadMap.erase(rmi);
    }
//  qDebug("Map::deleteRoad end %d", roadId);
}

//単路付け換え、付け換え後単路返却、構造的変換なので注意、既存単路不可
Road* Map::replaceRoad(Intsec* pIntsec, Intsec* pIntsecNew, Intsec* pIntsecCon)
{
    bool            already;
    Road*           pRoad;
    Road*           pRoadNew;

    pRoad = pIntsec->getRoad(pIntsecCon->getIntsecId());
    Q_ASSERT(pRoad!=NULL);
    pRoadNew = createRoad(pIntsecCon, pIntsecNew, &already);
    Q_ASSERT(!already);
    pRoadNew->copyLane(pRoad);
    if (pIntsecCon->getFirstIntsecIdCon() == pIntsec->getIntsecId())
        pIntsecCon->setFirstIntsecIdCon(pIntsecNew->getIntsecId());
    deleteRoad(pRoad->getRoadId());
    return pRoadNew;
}



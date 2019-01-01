/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点
//マップエディタの処理を元に作成

#include <Qt>
#include <vector>
#include <iostream>
#include <math.h>
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "road.h"
#include <AmuVector.h>

//最大接続単路数、大きいと MATES 本体が動かない
#define ROAD_MAX                10

//エラー種類（ビット）
#define ERROR_NONE              0x0000          //エラーなし
#define ERROR_ROAD_NONE         0x0001          //単路なし
#define ERROR_ROAD_OVERMAX      0x0002          //最大接続単路数より大きい
#define ERROR_LANE_ROAD2        0x0004          //2 単路車線数不正
#define ERROR_LANE_STRAIGHT     0x0008          //直進車線数不正、車線数減少が 3　以上、交差点ファイル必要

//エラーID、app.h に合わせる事
#define ERROR_ID_ROAD_NONE          APP_ERROR_ID_INTSEC
#define ERROR_ID_ROAD_OVERMAX       (APP_ERROR_ID_INTSEC + 1)
#define ERROR_ID_LANE_ROAD2         (APP_ERROR_ID_INTSEC + 2)
#define ERROR_ID_LANE_STRAIGHT      (APP_ERROR_ID_INTSEC + 3)

//エラー文字列
static const char* staticErrorStr[] = {
    "no road",
    "too much road to execute",
    "2 road lane mismacth",
    "straight lane mismacth, need intersection file"};

//コンストラクタ
Intsec::Intsec(int id)
{
    _id = id;
    _mapX = _mapY = _mapZ = 0;
    _firstIdCon = MAP_NOID;
    _mergeIntsec = NULL;
    _border = false;
    _interOp = false;
    _error = ERROR_NONE;
}

//単路追加
void Intsec::addRoad(int idCon, Road* road)
{
    INTSEC_ROADMAP_IT   rmi;

    //重複IDならエラー、先頭接続交差点IDなしなら設定
    // qDebug("Intsec::addRoad %d-%d", _id, idCon);
    rmi = _roadMap.find(idCon);
    // Q_ASSERT(rmi==_roadMap.end());
    // tokyo で以前エラーになったが、直したら出なくなった
    _roadMap.insert(pair<int, Road*>(idCon, road));
    if (_firstIdCon == MAP_NOID)
        _firstIdCon = idCon;
}

//単路取得、なしなら NULL
Road* Intsec::road(int idCon)
{
    INTSEC_ROADMAP_IT   rmi;

//  qDebug("Intsec::road %d-%d", _id, idCon);
    rmi = _roadMap.find(idCon);
    if (rmi == _roadMap.end())
        return NULL;

//  qDebug("Intsec::road not null end %d-%d", _id, idCon);
    return rmi->second;
}

//次の単路取得、first なら先頭から、なしなら NULL
Road* Intsec::nextRoad(INTSEC_ROADMAP_IT* rmi, bool first, int* idCon)
{
    if (first)
        *rmi = _roadMap.begin();
    else
        (*rmi)++;

    if (*rmi == _roadMap.end())
    {
        if (idCon != NULL)
            *idCon = MAP_NOID;
        return NULL;
    }
    if (idCon != NULL)
        *idCon = (*rmi)->first;
    return (*rmi)->second;
}

//単路削除、なしなら無視
void Intsec::deleteRoad(int idCon)
{
    INTSEC_ROADMAP_IT   rmi;

    //自データのみ削除、オブジェクト削除はマップ、先頭接続交差点ID設定
//  qDebug("Intsec::deleteRoad %d-%d", _id, idCon);
    rmi = _roadMap.find(idCon);
    if (rmi != _roadMap.end())
    {
        _roadMap.erase(rmi);
        if (idCon == _firstIdCon)
        {
            rmi = _roadMap.begin();
            if (rmi != _roadMap.end())
                _firstIdCon = rmi->first;
            else
                _firstIdCon = MAP_NOID;
        }
    }
}

//単路角度取得、反時計回りまたは絶対値、Z座標は無視
double Intsec::roadAngle(int idSource, int idDest, bool absolute)
{
    Map*        map = app.map();
    Intsec*     intsecSource;
    Intsec*     intsecDest;
    AmuVector   vectorSource, vectorDest;
    double      angle;

    intsecSource = map->findIntsec(idSource);
    intsecDest   = map->findIntsec(idDest);
    if (intsecSource == NULL || intsecDest == NULL)
        cout << "error" << endl;
    Q_ASSERT(intsecSource != NULL && intsecDest != NULL);
    vectorSource.setXYZ(intsecSource->mapPosX() - _mapX,
                        intsecSource->mapPosY() - _mapY, 0);
    vectorDest.setXYZ(intsecDest->mapPosX() - _mapX,
                      intsecDest->mapPosY() - _mapY, 0);
    angle = -vectorSource.calcAngle(vectorDest);        //時計回り→反時計回り
    if (angle < 0)
    {
        if (!absolute)
            angle = 2 * M_PI + angle;
        else
            angle = -angle;
    }
    return angle;
}

//角度による次の単路取得、first なら先頭から、なしなら NULL
//  先頭から反時計回り、first で角度計算後すぐに呼ぶ事、先頭以外で同一角度なら ID 昇順
Road* Intsec::nextRoadByAngle(INTSEC_ANGLEMAP_IT* ami, bool first, int* idCon)
{
    INTSEC_ROADMAP_IT   rmi;

    if (first)
    {
        _angleMap.clear();
        if (_firstIdCon != MAP_NOID)
        {
            _angleMap.insert(pair<double, int>(0, _firstIdCon));
            for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
            {
                if (rmi->first != _firstIdCon)
                    _angleMap.insert(pair<double, int>(roadAngle(_firstIdCon, rmi->first),
                                                       rmi->first));
            }
        }
        *ami = _angleMap.begin();
    }
    else
        (*ami)++;

    if (*ami == _angleMap.end())
    {
        if (idCon != NULL)
            *idCon = MAP_NOID;
        return NULL;
    }
    if (idCon != NULL)
        *idCon = (*ami)->second;
    rmi = _roadMap.find((*ami)->second);
    Q_ASSERT(rmi != _roadMap.end());    
//  qDebug("Intsec::nextRoadByAngle %d %d %f", _id, (*pami)->second, (*pami)->first);
    return rmi->second;
}

//エラーチェック、エラーなら true
bool Intsec::checkError()
{
    INTSEC_ANGLEMAP_IT  ami;
    Road*               road;
    int                 intsecType, intsecTypeCon;
    vector<Road*>       roads;
    vector<int>         intsecTypes;
    vector<int>         intsecIdCons;
    int                 i, j, k, l;
    vector<double>      angleRoads;
    vector<int>         straightRoads;
    double              angleSum, angleStraight, angleStraightMin;

    _error = ERROR_NONE;

    if      (_roadMap.size() > ROAD_MAX)
        _error = _error | ERROR_ROAD_OVERMAX;
    else if (_roadMap.size() == 0)
        _error = _error | ERROR_ROAD_NONE;

//  qDebug("road map size %d", _roadMap.size());
    if (_roadMap.size() >= 2)
    {
        road = nextRoadByAngle(&ami, true);
        while (road != NULL)
        {
            intsecType = road->intsecType(_id);
            Q_ASSERT(intsecType != ROAD_IT_NONE);
            if (intsecType == ROAD_IT_SMALL)
                intsecTypeCon = ROAD_IT_LARGE;
            else
                intsecTypeCon = ROAD_IT_SMALL;
            roads.push_back(road);
            intsecTypes.push_back(intsecType);
            intsecIdCons.push_back(road->intsec(intsecTypeCon)->id());
//          qDebug("Intsec::checkError road %d-%d", _intsecId, pRoad->getIntsec(intsecTypeCon)->getIntsecId());
            road = nextRoadByAngle(&ami);
        }
        if (_roadMap.size() == 2)
        {
            if (check2LaneError(roads[0], roads[1], intsecTypes[0], intsecTypes[1]) ||
                check2LaneError(roads[1], roads[0], intsecTypes[1], intsecTypes[0]))
                _error = _error | ERROR_LANE_ROAD2;
        }
        else
        {
//          qDebug("Intsec::checkError many roads");
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                j = (i + (int)intsecIdCons.size() + 1) % (int)intsecIdCons.size();
                angleRoads.push_back(roadAngle(intsecIdCons[i], intsecIdCons[j], true));
                straightRoads.push_back(-1);
            }
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                angleSum = 0;
                angleStraightMin = 1000;
                for (j = 1; j < (int)intsecIdCons.size(); j++)
                {
                    k = (i + j - 1 + (int)intsecIdCons.size()) % (int)intsecIdCons.size();
                    l = (i + j     + (int)intsecIdCons.size()) % (int)intsecIdCons.size();
                    angleSum += angleRoads[k];
                    angleStraight = fabs(angleSum - M_PI);
                    if (angleStraightMin > angleStraight)
                    {
                        angleStraightMin = angleStraight;
                        straightRoads[i] = l;
                    }
                }
            }
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                j = straightRoads[i];
                Q_ASSERT(j >= 0);
                if (straightRoads[j] == i)
                {
                    //逆は後でやるので一度呼べばいい
                    if (checkStraightError(roads[i], roads[j], intsecTypes[i], intsecTypes[j]))
                        _error = _error | ERROR_LANE_STRAIGHT;
                }
            }
        }
    }

//  qDebug("Intsec::checkError error 0x%x", _error);
    return (_error != ERROR_NONE);
}

//2 単路車線エラーチェック、エラーなら true
bool Intsec::check2LaneError(Road* road1, Road* road2, int intsecType1, int intsecType2)
{
    if (road1->lane(intsecType1, ROAD_LT_OUT) > road2->lane(intsecType2, ROAD_LT_IN))
        return true;
    else if (road1->lane(intsecType1, ROAD_LT_OUT) == 0 &&
             road2->lane(intsecType2, ROAD_LT_IN)  >= 1)
        return true;
    return false;
}

//直線車線エラーチェック、エラーなら true
bool Intsec::checkStraightError(Road* road1, Road* road2, int intsecType1, int intsecType2)
{
    if (road1->lane(intsecType1, ROAD_LT_OUT) > road2->lane(intsecType2, ROAD_LT_IN) + 2)
        return true;
    return false;
}

//エラーメッセージ取得、複数行
void Intsec::getErrorMessages(string *message, vector<int>* errorCnt, vector<string>* errorStr)
{
    (*message) = "";
    if (_error & ERROR_ROAD_NONE)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_ROAD_NONE);
    if (_error & ERROR_ROAD_OVERMAX)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_ROAD_OVERMAX);
    if (_error & ERROR_LANE_ROAD2)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_LANE_ROAD2);
    if (_error & ERROR_LANE_STRAIGHT)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_LANE_STRAIGHT);
}

//エラーメッセージ取得、１行
void Intsec::getErrorMessage(string *message, vector<int>* errorCnt, vector<string>* errorStr,
                             int id)
{
    (*message) += staticErrorStr[id - APP_ERROR_ID_INTSEC];
    (*message) += "\n";
    (errorCnt->at(id))++;
    if (errorStr->at(id) == "")
        errorStr->at(id) = staticErrorStr[id - APP_ERROR_ID_INTSEC];
}


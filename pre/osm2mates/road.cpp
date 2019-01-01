/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//単路
//マップエディタの処理を元に作成

#include <iostream>
#include <math.h>
#include <string.h>
#include "app.h"
#include "road.h"
#include "intsec.h"

#define	MIN_DISTANCE            30                  //最小距離

//エラー種類（ビット）
#define ERROR_NONE              0x0000              //エラーなし
#define ERROR_DIST_SHORT        0x0001              //最小距離未満
#define ERROR_LANE_NONE         0x0002              //車線なし
#define ERROR_LANE_ONESIDE_NONE 0x0004              //片方車線なし
#define ERROR_LANE_DIVIDE       0x0008              //車線分岐過不足

//エラーID、app.h に合わせる事
#define ERROR_ID_DIST_SHORT        APP_ERROR_ID_ROAD
#define ERROR_ID_LANE_NONE         (APP_ERROR_ID_ROAD + 1)
#define ERROR_ID_LANE_ONESIDE_NONE (APP_ERROR_ID_ROAD + 2)
#define ERROR_ID_LANE_DIVIDE       (APP_ERROR_ID_ROAD + 3)

//エラー文字列
static const char* staticErrorStr[] = {
    "too short distance",
    "no lane",
    "no oneside lane",
    "lack of lane to divide"};

//コンストラクタ
Road::Road(int id, Intsec* intsec1, Intsec* intsec2, int laneDefault)
{
    _id = id;
    if (intsec1->id() < intsec2->id())
    {
        _intsec[ROAD_IT_SMALL] = intsec1;
        _intsec[ROAD_IT_LARGE] = intsec2;
    }
    else
    {
        _intsec[ROAD_IT_SMALL] = intsec2;
        _intsec[ROAD_IT_LARGE] = intsec1;
    }
    _lane[ROAD_IT_SMALL][ROAD_LT_OUT] = _lane[ROAD_IT_SMALL][ROAD_LT_IN] =
    _lane[ROAD_IT_LARGE][ROAD_LT_OUT] = _lane[ROAD_IT_LARGE][ROAD_LT_IN] = laneDefault;
    _laneFlag[ROAD_IT_SMALL][ROAD_LT_OUT] = _laneFlag[ROAD_IT_SMALL][ROAD_LT_IN] =
    _laneFlag[ROAD_IT_LARGE][ROAD_LT_OUT] = _laneFlag[ROAD_IT_LARGE][ROAD_LT_IN] = false;
    _error = ERROR_NONE;
}

//交差点種別取得、なしなら ROAD_IT_NONE
int Road::intsecType(int id)
{
    if      (id == _intsec[ROAD_IT_SMALL]->id())
        return ROAD_IT_SMALL;
    else if (id == _intsec[ROAD_IT_LARGE]->id())
        return ROAD_IT_LARGE;
    else
        return ROAD_IT_NONE;
}

//車線数設定、laneチェックなし
void Road::setLane(int intsecType, int laneType, int lane)
{
    Q_ASSERT(0 <= intsecType && intsecType < ROAD_IT_NUM);
    Q_ASSERT(0 <= laneType && laneType < ROAD_LT_NUM);
    if (lane > 9)
        lane = 9;  //警告は不要か、そもそも実行不能
    _lane[intsecType][laneType] = lane;
    _laneFlag[intsecType][laneType] = true;

// cout << _intsec[ROAD_IT_SMALL]->id() << " "
//      << _intsec[ROAD_IT_LARGE]->id() << " "
//      << intsecType << " " << laneType << " "
//      << _lane[intsecType][laneType] << endl;
}

//車線数まとめて追加
void Road::addLanes(Intsec* intsec1, int laneOut1, int laneIn1,
                                     int laneOut2, int laneIn2)
{
    int intsecType1 = intsecType(intsec1->id());
    Q_ASSERT(intsecType1 != ROAD_IT_NONE);
    int intsecType2 = ROAD_IT_REVERSE(intsecType1);

    addLane(intsecType1, ROAD_LT_OUT, laneOut1);
    addLane(intsecType1, ROAD_LT_IN,  laneIn1);
    addLane(intsecType2, ROAD_LT_OUT, laneOut2);
    addLane(intsecType2, ROAD_LT_IN,  laneIn2);
}

//エラーチェック、エラーなら true
bool Road::checkError()
{
    double  dx, dy, dz, distance;

    _error = ERROR_NONE;
    dx = _intsec[ROAD_IT_SMALL]->mapPosX() - _intsec[ROAD_IT_LARGE]->mapPosX();
    dy = _intsec[ROAD_IT_SMALL]->mapPosY() - _intsec[ROAD_IT_LARGE]->mapPosY();
    dz = _intsec[ROAD_IT_SMALL]->mapPosZ() - _intsec[ROAD_IT_LARGE]->mapPosZ();
    distance = sqrt(dx * dx + dy * dy + dz * dz);
    if (distance < MIN_DISTANCE)
        _error = _error | ERROR_DIST_SHORT;
    if (_lane[ROAD_IT_SMALL][ROAD_LT_OUT] == 0 && _lane[ROAD_IT_SMALL][ROAD_LT_IN] == 0 &&
        _lane[ROAD_IT_LARGE][ROAD_LT_OUT] == 0 && _lane[ROAD_IT_LARGE][ROAD_LT_IN] == 0)
        _error = _error | ERROR_LANE_NONE;
    if ((_lane[ROAD_IT_SMALL][ROAD_LT_OUT] == 0 && _lane[ROAD_IT_LARGE][ROAD_LT_IN]  != 0) ||
        (_lane[ROAD_IT_SMALL][ROAD_LT_IN]  == 0 && _lane[ROAD_IT_LARGE][ROAD_LT_OUT] != 0) ||
        (_lane[ROAD_IT_LARGE][ROAD_LT_OUT] == 0 && _lane[ROAD_IT_SMALL][ROAD_LT_IN]  != 0) ||
        (_lane[ROAD_IT_LARGE][ROAD_LT_IN]  == 0 && _lane[ROAD_IT_SMALL][ROAD_LT_OUT] != 0))
        _error = _error | ERROR_LANE_ONESIDE_NONE;
    if ((_lane[ROAD_IT_SMALL][ROAD_LT_OUT] != _lane[ROAD_IT_LARGE][ROAD_LT_IN] &&
         _lane[ROAD_IT_SMALL][ROAD_LT_OUT] != _lane[ROAD_IT_LARGE][ROAD_LT_IN] + 1) ||
        (_lane[ROAD_IT_LARGE][ROAD_LT_OUT] != _lane[ROAD_IT_SMALL][ROAD_LT_IN] &&
         _lane[ROAD_IT_LARGE][ROAD_LT_OUT] != _lane[ROAD_IT_SMALL][ROAD_LT_IN] + 1))
        _error = _error | ERROR_LANE_DIVIDE;
    return (_error != ERROR_NONE);
}

//エラーメッセージ取得、複数行
void Road::getErrorMessages(string *message, vector<int>* errorCnt, vector<string>* errorStr)
{
    (*message) = "";
    if (_error & ERROR_DIST_SHORT)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_DIST_SHORT);
    if (_error & ERROR_LANE_NONE)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_LANE_NONE);
    if (_error & ERROR_LANE_ONESIDE_NONE)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_LANE_ONESIDE_NONE);
    if (_error & ERROR_LANE_DIVIDE)
        getErrorMessage(message, errorCnt, errorStr, ERROR_ID_LANE_DIVIDE);
}

//エラーメッセージ取得、１行
void Road::getErrorMessage(string *message, vector<int>* errorCnt, vector<string>* errorStr,
                           int id)
{
    (*message) += staticErrorStr[id - APP_ERROR_ID_ROAD];
    (*message) += "\n";
    (errorCnt->at(id))++;
    if (errorStr->at(id) == "")
        errorStr->at(id) += staticErrorStr[id - APP_ERROR_ID_ROAD];
}





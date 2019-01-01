/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//単路

#include <math.h>
#include <string.h>
#include "road.h"
#include "intsec.h"

#define	LANE_DEFAULT            1                   //デフォルト車線数
#define	MIN_DISTANCE            30                  //最小距離

//エラー種類（ビット）
#define ERROR_NONE              0x0000             //エラーなし
#define ERROR_DIST_SHORT        0x0001             //最小距離未満
#define ERROR_LANE_NONE         0x0002             //車線なし
#define ERROR_LANE_ONESIDE_NONE 0x0004             //片方車線なし
#define ERROR_LANE_DIVIDE       0x0008             //車線分岐過不足

//コンストラクタ
Road::Road(int roadId, Intsec* pIntsec1, Intsec* pIntsec2)
{
//  qDebug("Road::Road %d %d-%d", roadId, pIntsec1->getIntsecId(), pIntsec2->getIntsecId());
    _roadId = roadId;
    if (pIntsec1->getIntsecId() < pIntsec2->getIntsecId())
    {
        _pIntsec[ROAD_IT_SMALL] = pIntsec1;
        _pIntsec[ROAD_IT_LARGE] = pIntsec2;
    }
    else
    {
        _pIntsec[ROAD_IT_SMALL] = pIntsec2;
        _pIntsec[ROAD_IT_LARGE] = pIntsec1;
    }
    _lane[ROAD_IT_SMALL][ROAD_LT_OUT] = _lane[ROAD_IT_SMALL][ROAD_LT_IN] =
    _lane[ROAD_IT_LARGE][ROAD_LT_OUT] = _lane[ROAD_IT_LARGE][ROAD_LT_IN] = LANE_DEFAULT;
}

//交差点種別取得、なしなら ROAD_IT_NONE
int Road::getIntsecType(int intsecId)
{
    if      (intsecId == _pIntsec[ROAD_IT_SMALL]->getIntsecId())
        return ROAD_IT_SMALL;
    else if (intsecId == _pIntsec[ROAD_IT_LARGE]->getIntsecId())
        return ROAD_IT_LARGE;
    else
        return ROAD_IT_NONE;
}

//距離取得
double Road::getDistance()
{
    double  dx, dy, dz;
    dx = _pIntsec[ROAD_IT_SMALL]->getMapPosX() - _pIntsec[ROAD_IT_LARGE]->getMapPosX();
    dy = _pIntsec[ROAD_IT_SMALL]->getMapPosY() - _pIntsec[ROAD_IT_LARGE]->getMapPosY();
    dz = _pIntsec[ROAD_IT_SMALL]->getMapPosZ() - _pIntsec[ROAD_IT_LARGE]->getMapPosZ();
    return sqrt(dx * dx + dy * dy + dz * dz);
}

//デバッグトレース
void Road::debugTrace()
{
    qDebug("Road::debugTrace id=%d, lane=(%d(%d)<-%d(%d), %d(%d)->%d(%d))",
           _roadId, _pIntsec[ROAD_IT_SMALL]->getIntsecId(), _lane[ROAD_IT_SMALL][ROAD_LT_OUT],
                    _pIntsec[ROAD_IT_LARGE]->getIntsecId(), _lane[ROAD_IT_LARGE][ROAD_LT_IN],
                    _pIntsec[ROAD_IT_SMALL]->getIntsecId(), _lane[ROAD_IT_SMALL][ROAD_LT_IN],
                    _pIntsec[ROAD_IT_LARGE]->getIntsecId(), _lane[ROAD_IT_LARGE][ROAD_LT_OUT] );
}

//エラーチェック、エラーなら true
bool Road::checkError()
{
    double   distance;

    _error = ERROR_NONE;

    distance = getDistance();
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

//  qDebug("Road::checkError id=%d, lane=(%d(%d)<-%d(%d), %d(%d)->%d(%d)) distance=%f error=0x%x",
//         _roadId, _pIntsec[ROAD_IT_SMALL]->getIntsecId(), _lane[ROAD_IT_SMALL][ROAD_LT_OUT],
//                  _pIntsec[ROAD_IT_LARGE]->getIntsecId(), _lane[ROAD_IT_LARGE][ROAD_LT_IN],
//                  _pIntsec[ROAD_IT_SMALL]->getIntsecId(), _lane[ROAD_IT_SMALL][ROAD_LT_IN],
//                  _pIntsec[ROAD_IT_LARGE]->getIntsecId(), _lane[ROAD_IT_LARGE][ROAD_LT_OUT],
//                  distance, _error);
    return (_error != ERROR_NONE);
}

//エラー判定（チェックなし）
bool Road::isError()
{
    return (_error != ERROR_NONE);
}

//エラーメッセージ取得（複数行）
void Road::getErrorMessage(string *pMessage)
{
    (*pMessage) = "";
    if (_error & ERROR_DIST_SHORT)
        (*pMessage) += "Distance is too short.\n";
    if (_error & ERROR_LANE_NONE)
        (*pMessage) += "There are no lanes.\n";
    if (_error & ERROR_LANE_ONESIDE_NONE)
        (*pMessage) += "Either inflow or outflow lane is not enough.\n";
    if (_error & ERROR_LANE_DIVIDE)
        (*pMessage) += "Lane is not enough to devide.\n";
}

//車線コピー
void Road::copyLane(Road* pRoadSource)
{
    memcpy(_lane, pRoadSource->_lane, sizeof(_lane));
}


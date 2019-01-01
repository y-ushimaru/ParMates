/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//単路

#ifndef ROAD_H
#define ROAD_H

#include <QtGui>
#include <string>

#define QT_FATAL_ASSERT

using namespace std;

class Intsec;

//交差点種別（ID大）
#define ROAD_IT_SMALL   0                                   //ID小
#define ROAD_IT_LARGE   1                                   //ID大
#define ROAD_IT_NUM     2                                   //種別数
#define ROAD_IT_NONE    -1                                  //種別なし
#define ROAD_IT_REVERSE(it) (1 - (it))                      //逆転

//車線種別
#define ROAD_LT_OUT     0                                   //単路から出る
#define ROAD_LT_IN      1                                   //単路へ入る
#define ROAD_LT_NUM     2                                   //車線種別

//単路
class Road
{
    int         _roadId;                                            //単路ID
    Intsec*     _pIntsec[ROAD_IT_NUM];                              //交差点
    int         _lane[ROAD_IT_NUM][ROAD_LT_NUM];                    //車線数
    int         _error;                                             //エラー種別

public:
    Road(int roadId, Intsec* pIntsec1, Intsec* pIntsec2);           //コンストラクタ
    void        debugTrace();                                       //デバッグトレース
    int         getRoadId()                                         //単路ID取得
    {
        return _roadId;
    }
    Intsec*     getIntsec(int intsecType)                           //交差点取得
    {
        Q_ASSERT(0 <= intsecType && intsecType < ROAD_IT_NUM);
        return _pIntsec[intsecType];
    }
    int getIntsecType(int intsecId);                                //交差点種別取得、なしなら ROAD_IT_NONE
    double getDistance();                                           //距離取得
    bool checkError();                                              //エラーチェック、エラーなら true
    bool isError();                                                 //エラー判定（チェックなし）
    void getErrorMessage(string *pMessage);                         //エラーメッセージ取得（複数行）

    void setLane(int intsecType, int laneType, int lane)            //車線設定、laneチェックなし
    {
        Q_ASSERT(0 <= intsecType && intsecType < ROAD_IT_NUM);
        Q_ASSERT(0 <= laneType && laneType < ROAD_LT_NUM);
        _lane[intsecType][laneType] = lane;
    }
    int  getLane(int intsecType, int laneType)                      //車線取得
    {
        Q_ASSERT(0 <= intsecType && intsecType < ROAD_IT_NUM);
        Q_ASSERT(0 <= laneType && laneType < ROAD_LT_NUM);
        return _lane[intsecType][laneType];
    }
    void setLane2()                                                //往復2車線設定
    {
        _lane[ROAD_IT_SMALL][ROAD_LT_OUT] = _lane[ROAD_IT_SMALL][ROAD_LT_IN] =
        _lane[ROAD_IT_LARGE][ROAD_LT_OUT] = _lane[ROAD_IT_LARGE][ROAD_LT_IN] = 1;
    }
    void setLane4()                                                //往復4車線設定
    {
        _lane[ROAD_IT_SMALL][ROAD_LT_OUT] = _lane[ROAD_IT_SMALL][ROAD_LT_IN] =
        _lane[ROAD_IT_LARGE][ROAD_LT_OUT] = _lane[ROAD_IT_LARGE][ROAD_LT_IN] = 2;
    }
    bool isLane2()                                                  //往復2車線確認
    {
        if (_lane[ROAD_IT_SMALL][ROAD_LT_OUT] == 1 &&
            _lane[ROAD_IT_SMALL][ROAD_LT_IN]  == 1 &&
            _lane[ROAD_IT_LARGE][ROAD_LT_OUT] == 1 &&
            _lane[ROAD_IT_LARGE][ROAD_LT_IN]  == 1)
            return true;
        else
            return false;
    }
    bool isLane4()                                                  //往復4車線確認
    {
        if (_lane[ROAD_IT_SMALL][ROAD_LT_OUT] == 2 &&
            _lane[ROAD_IT_SMALL][ROAD_LT_IN]  == 2 &&
            _lane[ROAD_IT_LARGE][ROAD_LT_OUT] == 2 &&
            _lane[ROAD_IT_LARGE][ROAD_LT_IN]  == 2)
            return true;
        else
            return false;
    }
    bool isLaneOver4()                                              //往復4車線以上確認
    {
        if (_lane[ROAD_IT_SMALL][ROAD_LT_OUT] + _lane[ROAD_IT_SMALL][ROAD_LT_IN] >= 4 &&
            _lane[ROAD_IT_LARGE][ROAD_LT_OUT] + _lane[ROAD_IT_LARGE][ROAD_LT_IN] >= 4)
            return true;
        else
            return false;
    }
    void        copyLane(Road* pRoadSource);                        //車線コピー
};

#endif // ROAD_H

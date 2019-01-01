/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TRAM_LINE_MANAGER_H__
#define __TRAM_LINE_MANAGER_H__

#include <vector>

class RoadMap;
class Intersection;
class TramLine;

/// 路上における軌道の設置箇所
/**
 * 進行方向に向かって左，中央，右を指定する．
 * 中央とは上り車線と下り車線の間を意味する．
 */
typedef enum
{
    UNDEFINED,
    NONE,
    LEFT,
    CENTER,
    RIGHT
} TramLineSide;

/**
 * @addtogroup Tram
 * @brief 路面電車の環境と挙動を定義するモジュール
 */

/// 路面電車の路線情報を管理するためのクラス
/**
 * @ingroup Tram
 */
class TramLineManager
{
private:
    TramLineManager();
    ~TramLineManager();

public:
    /// 唯一のインスタンスを返す
    static TramLineManager& instance();

    /// 地図をセットする
    void setRoadMap(RoadMap* roadMap);

    /// 路面電車の路線定義ファイルを読み込む
    void readTramLineFile();

    /// 交差点@p interの@p dir方向の路面電車コネクタの数を得る
    void getNumTramConnectors
    (Intersection* inter, int dir,
     int* result_numInLeft,   int* result_numOutLeft,
     int* result_numInCenter, int* result_numOutCenter,
     int* result_numInRight,  int* result_numOutRight);
 
    /// 路線は交差点@p interにおいて@p from方向から@p to方向に接続するか
    bool isTramLineConnected(Intersection* inter,
                             int from, int to);

    /// 指定された交差点をその順で通過する路線を返す
    TramLine* tramLineWithDesignatedGates
    (const std::vector<const Intersection*>* gates);

    /// 情報を表示する
    void print() const;

private:
    /// 地図
    RoadMap* _roadMap;

    /// 路面電車路線
    std::vector<TramLine*> _tramLines;
};

#endif //__TRAM_LINE_MANAGER_H__

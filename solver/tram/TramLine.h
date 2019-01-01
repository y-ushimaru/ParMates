/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TRAM_LINE_H__
#define __TRAM_LINE_H__

#include <vector>
#include "TramLineManager.h"

class Intersection;

/// 路面電車の1つの路線情報を保持するためのクラス
/**
 * 上り下りで2路線とカウントする．
 *
 * @ingroup Tram
 */
class TramLine
{
public:
    TramLine();
    ~TramLine();

    /// 路線上の交差点の情報を保持する内部構造体
    struct TramLineInter
    {
        Intersection* inter;
        Intersection* prev;  //!< 路線上の手前の交差点
        int prevDir;         
        TramLineSide prevSide;   
        Intersection* next;  //!< 路線上の次の交差点
        int nextDir;
        TramLineSide nextSide;
    };

    /// 交差点情報を追加する
    bool createTramLineInter(Intersection* inter,
                             Intersection* prev,
                             TramLineSide prevSide,
                             Intersection* next,
                             TramLineSide nextSide);

    /// 交差点の集合を返す
    void getIntersections
    (std::vector<const Intersection*>* result_inters);

    /// 交差点@p interの@p dir方向の路面電車コネクタの数を得る
    void getNumTramConnectors
    (Intersection* inter, int dir,
     int* result_numInLeft,   int* result_numOutLeft,
     int* result_numInCenter, int* result_numOutCenter,
     int* result_numInRight,  int* result_numOutRight);

    /// 路線は交差点@p interにおいて@p from方向から@p to方向に接続するか
    bool isTramLineConnected(Intersection* inter,
                             int from, int to);

    /// 情報を表示する
    void print() const;

private:
    /// 路線を構成する交差点
    std::vector<TramLineInter*> _inters;

};

#endif //__TRAM_LINE_H__

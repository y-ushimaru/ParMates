/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __INTERSECTION_TRAM_EXT_H__
#define __INTERSECTION_TRAM_EXT_H__

#include <vector>
#include "../AmuLineSegment.h"
#include "../LaneBundle.h"

class Intersection;
class Connector;

/// 交差点の路面電車機能拡張クラス
/**
 * @ingroup Tram
 */
class IntersectionTramExt
{
public:
    IntersectionTramExt(){};
    IntersectionTramExt(Intersection* inter);
    ~IntersectionTramExt(){};

    /// 路面電車軌道用の流入・流出点の設定
    void setNumTramConnectors();

    /// 方向@p dirの交差点に向かって道路左端の路面電車流入点数を返す
    int numInTramLeft(int dir) const;

    /// 方向@p dirの交差点に向かって道路左端の路面電車流出点数を返す
    int numOutTramLeft(int dir) const;

    /// 方向@p dirの交差点に向かって道路左端の路面電車流入流出点数を返す
    int numTotalTramLeft(int dir) const;
 
    /// 方向@p dirの道路中央の路面電車流入点数を返す
    int numInTramCenter(int dir) const;

    /// 方向@p dirの道路中央の路面電車流出点数を返す
    int numOutTramCenter(int dir) const;

    /// 方向@p dirの道路中央の路面電車流入流出点数を返す
    int numTotalTramCenter(int dir) const;
 
    /// 方向@p dirの交差点に向かって道路右端の路面電車流入点数を返す
    int numInTramRight(int dir) const;

    /// 方向@p dirの交差点に向かって道路右端の路面電車流出点数を返す
    int numOutTramRight(int dir) const;

    /// 方向@p dirの交差点に向かって道路左端の路面電車流入流出点数を返す
    int numTotalTramRight(int dir) const;

    /// 方向@p dirの路面電車レーンの総数を返す
    int numTotalTramLanes(int dir) const; 

    /// 路面電車軌道用コネクタ@p connectorを持つ境界番号を返す
    int tramDirection(const Connector* connector) const;

    /// 端コネクタ
    const Connector* edgeTramConnector(int idInt);

    /// サブセクション境界上のコネクタを生成する
    void createConnectorsOnLine(AmuLineSegment line,
                                int numIn, int numOut,
                                int dir);

    /// 交差点内の路面電車レーンを生成する
    bool createTramLanesInIntersection();

    /// ODノード内の路面電車レーンを生成する
    bool createTramLanesInODNode();

    /// 交差点内の1つの路面電車レーンを生成する
    void createTramLane(int idInt,
                        const Connector* from, const Connector* to,
                        bool setLaneBegEnd);


private:
    /// 対応する交差点
    Intersection* _inter;

    /// 交差点に向かって道路左端の路面電車流入点数
    std::vector<int> _numInTramLeft;

    /// 交差点に向かって道路左端の路面電車流出点数
    std::vector<int> _numOutTramLeft;

    /// 道路中央の路面電車流入点数
    std::vector<int> _numInTramCenter;

    /// 道路中央の路面電車流出点数
    std::vector<int> _numOutTramCenter;

    /// 交差点に向かって道路右端の路面電車流入点数
    std::vector<int> _numInTramRight;

    /// 交差点に向かって道路右端の路面電車流出点数
    std::vector<int> _numOutTramRight;

    /// 路面電車用内部コネクタ
    RMAPCON _innerTramConnectors;
};

#endif //__INTERSECTION_TRAM_EXT_H__

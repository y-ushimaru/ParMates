/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LANE_IN_INTERSECTION_H__
#define __LANE_IN_INTERSECTION_H__

#include "Lane.h"
#include <map>

/// 交差点内の仮想走行レーンを表すクラス
/**
 * Laneを継承する
 *
 * @ingroup RoadNetwork
 */
class LaneInIntersection : public Lane
{
public:
    LaneInIntersection(const std::string& id,
                       const Connector* ptBegin, const Connector* ptEnd,
                       LaneBundle* parent);
    virtual ~LaneInIntersection(){};

    //======================================================================
    /** @name レーンの所属と接続に関する関数 */
    //@{

    /// レーンの交差関係を構築する
    virtual bool setCollision();

    /// 交差点内交錯レーンを返す
    virtual std::vector<Lane*>* collisionLanesInIntersection();

    /// 交差点上流単路部内交錯レーンを返す
    virtual std::vector<Lane*>* collisionLanesInSection();

    //@}

public:

    //====================================================================
    /** @name その他 */
    //@{

    /// 情報を表示する
    virtual void print() const;

    //@}

protected:
};

#endif //__LANE_IN_SECTION_H__

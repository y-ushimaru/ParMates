/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __LANE_IN_SECTION_H__
#define __LANE_IN_SECTION_H__

#include "Lane.h"

/// 単路内の仮想走行レーンを表すクラス
/**
 * Laneを継承する
 *
 * @ingroup RoadNetwork
 */
class LaneInSection : public Lane
{
public:
    LaneInSection(const std::string& id,
                  const Connector* ptBegin, const Connector* ptEnd,
                  LaneBundle* parent);
    virtual ~LaneInSection(){};

    //======================================================================
    /** @name レーンの所属と接続に関する関数 */
    //@{

    /// レーンの接続関係を構築する
    /**
     * _nextLanes, _previousLanesを作成する
     */
    virtual bool setConnection();

    /// 横のレーンにおける位置を返す
    /**
     * @param originLength 自分のレーンの始点からの距離
     * @param isLeft 左レーンを探索するかどうか
     * @param[out] result_sideLane 横のレーン
     * @param[out] result_length 横のレーンに投影した時の始点からの距離
     */
    virtual void getSideLaneLength(double originLength,
                                   bool isLeft,
                                   Lane** result_sideLane,
                                   double* result_length) const;

    //@}

protected:
    //====================================================================
    /** @name レーンの所属と接続に関するprotected関数 */
    //@{

    // 横のレーンを返す
    /**
     * @p isLeftがtrueなら左の，falseなら右の最も近いレーンを返す
     */
    virtual Lane* _sideLane(bool isLeft) const;

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

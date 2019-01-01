/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROAD_OCCUPANT_H__
#define __ROAD_OCCUPANT_H__

#include "AmuVector.h"
#include <string>

/**
 * @addtogroup Agent
 * @brief エージェントの挙動を定義するモジュール
 */

/// Laneに配置される"モノ"の純粋抽象クラス
/**
 * @note Vehicleに継承させる
 * @ingroup Agent
 */
class RoadOccupant
{
public:
    virtual ~RoadOccupant(){};

    /// 配置物の識別番号を返す
    virtual const std::string& id() const = 0;

    /// 配置物の幅を返す
    virtual double bodyWidth() const = 0;
    /// 配置物の長さを返す
    virtual double bodyLength() const = 0;
    /// 配置物の高さを返す
    virtual double bodyHeight() const = 0;

    /// レーンの始点からの距離
    virtual double length() const = 0;
    /// 単路の始点からの距離
    virtual double totalLength() const = 0;
    /// 速度
    virtual double velocity() const = 0;
    /// 加速度
    virtual double accel() const = 0;

    /// 方向ベクトル
    virtual const AmuVector directionVector() const = 0;
};

#endif //__ROAD_OCCUPANT_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLE_TRAM_H__
#define __VEHICLE_TRAM_H__

#include "../Vehicle.h"

/// 路面電車クラス
/**
 * @ingroup Tram
 */
class VehicleTram : public Vehicle
{
public:
    VehicleTram();
    virtual ~VehicleTram();

    /// 経路を設定する
    /**
     * 経路探索は必要なく，規定された路面電車の路線から選択する
     */
    void setRoute();
};

#endif //__VEHICLE_TRAM_H__

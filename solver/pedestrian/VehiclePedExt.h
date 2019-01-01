/**************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLE_PED_EXT_H__
#define __VEHICLE_PED_EXT_H__

class Vehicle;
class Pedestrian;
class Intersection;
class Zebra;
class AmuPoint;
class AmuVector;

/// 自動車の歩行者認知機能拡張クラス
class VehiclePedExt
{
public:
    VehiclePedExt(){};
    VehiclePedExt(Vehicle* vehicle);
    ~VehiclePedExt(){};

    /// 歩行者を認知する
    bool recognizePedestrian(Intersection* nextInter);


    /// 歩行者との交差判定を行う
    bool isCrossingWith(const Pedestrian* pds,
                        double time) const;

    bool isDistance(const Pedestrian *pds) const;   

private:
    /// 交差点の横断歩道@p zebraでの歩行者との交差を調べる
    bool _isStoppedByCrossWithPedestrian(Zebra* zebra,
                                         double lengthToNext);

    /// @p pを通る方向@p dirの直線に対し，@p pointがある側を返す
    int _detectSide(const AmuPoint* p, const AmuVector* dir,
                    const AmuPoint* point) const;

private:
    /// 対応するVehicle
    Vehicle* _vehicle;
};

#endif //__VEHICLE_PED_EXT_H__

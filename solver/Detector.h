/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include <vector>
#include <string>

class Section;
class Lane;
class Vehicle;

/// 車両感知器クラス
/**
 * 車両感知器ユニットを構成するオブジェクト．
 * 1つのレーンにつき1つの車両感知器オブジェクトが割り当てられる．
 *
 * @ingroup RoadsideUnit Monitoring
 */
class Detector
{
public:
    Detector();
    ~Detector();

    /// 観測結果を格納する構造体
    struct PassedVehicleData
    {
        Lane* lane;
        Vehicle* vehicle;
    };

    /// 観測結果を初期化する
    void clearPassedVehicleData(PassedVehicleData* pvd);

    /// 感知器にレーン@p laneと観測位置@p lengthをセットする
    /**
     * 観測位置はレーン始点からの距離で表す．
     */
    void setLane(Section* section, Lane* lane, double length);

    /// レーンを観測し，直前にポイントを通過した車両のベクトルを返す
    bool getPassedVehicleData(std::vector<PassedVehicleData>* result);

    /// 設置されているレーンを返す
    Lane* lane() const;

    /// 設置されたレーンの始点からの距離を返す
    double length() const;

    /// 設置されている位置のx座標を返す
    double x() const;

    /// 設置されている位置のy座標を返す
    double y() const;

    /// 設置されている位置のz座標を返す
    double z() const;

private:
    /// 設置された単路
    Section* _section;

    /// 設置されたレーン
    Lane* _lane;

    /// レーン始点からの距離
    double _length;

    /// 現在のタイムステップでの観測結果
    std::vector<PassedVehicleData> _detectedVehicles;
};

#endif //__DETECTOR_H__

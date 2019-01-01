/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __DETECTOR_UNIT_H__
#define __DETECTOR_UNIT_H__

#include <vector>
#include <string>
#include "TimeManager.h"
#include "RoadsideUnit.h"
#include "Detector.h"

class Section;

/// 車両感知器群クラス
/**
 * 一つの単路に設置される車両感知器の「グループ」に相当する．
 * 各レーンを担当する個々の感知器を管理する．
 * 出力ファイルはユニット単位で作成する．
 *
 * @ingroup RoadsideUnit Monitoring
 */
class DetectorUnit : public RoadsideUnit
{
public:
    DetectorUnit(const std::string& id);
    ~DetectorUnit();

    /** @name アクセッサ */
    //@{

    /// 識別番号を返す
    const std::string& id() const;

    /// 設置されたsectionを返す
    Section* section() const;

    /// 設置された方向（上り/下り）を返す
    bool isUp() const;

    /// 設置された位置（Section始点からの距離）を返す
    double length() const;

    /// 醜形間隔を返す
    ulint interval() const;

    /// ユニットを構成する感知器の集合を返す
    const std::vector<Detector*>* detectors() const;

    //@}

    /// ユニットを配置し，個々の感知器を作成する
    void setPosition(Section* section, bool isUp, double length, ulint interval);

    /// 単路を観測する
    void monitorLanes();

    /// 観測データを返す
    bool getPassedVehicleData
    (std::vector<Detector::PassedVehicleData>* result) const;

    /// 観測データをクリアする
    void clearPassedVehicleData();

    /// 統計データを格納する構造体
    struct StatVehicleData
    {
        ulint beginTime;
        int totalAllPassengers;
        int totalAllTrucks;
        int sumPassengers;
        int sumTrucks;
        std::vector<std::string> laneIds;
        std::vector<int> numPassengers;
        std::vector<int> numTrucks;
    };

    /// 統計データを初期化する
    void clearStatVehicleData();

    /// 統計データを返す
    StatVehicleData statVehicleData();

    /// 情報を表示する
    void print() const;

private:
    /// 識別番号
    std::string _id;

    /// 設置されている単路
    Section* _section;

    /// 単路の上り方向かどうか
    bool _isUp;

    /// 始点交差点からの距離
    double _length;

    /// 統計データを出力する時間間隔
    ulint _interval;

    /// ユニットを構成する感知器
    std::vector<Detector*> _detectors;

    /// 観測データ
    std::vector<Detector::PassedVehicleData> _unitPvd;

    /// 統計データ
    StatVehicleData _unitSvd;
};

#endif //__DETECTOR_UNIT_H__

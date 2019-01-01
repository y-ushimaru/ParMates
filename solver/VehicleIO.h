/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLEIO_H__
#define __VEHICLEIO_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "TimeManager.h"
#include "VirtualLeader.h"

#ifdef USE_ZLIB
#include <zlib.h>
#else // USE_ZLIB
#include <cstdio>
#endif // USE_ZLIB   

class RoadMap;
class Vehicle;

/// 車両エージェントの読み書きを行うクラス
/**
 * Singleton
 */
class VehicleIO
{
private:
    VehicleIO();
    ~VehicleIO(){};

public:
    /// 唯一のインスタンスを返す
    static VehicleIO& instance();

    /** @name 出力に関するもの*/
    /// @{

    /// 車両の動的データ（タイムステップごとの位置速度など）を出力する
    bool writeVehiclesDynamicData(const ulint time,
                                  std::vector<Vehicle*>* vehicles);
private:
    /// 動的データ出力の本体
    bool _writeVehicleDynamicData(Vehicle* vehicle);

    /// ディレクトリを階層的に作成する
    /**
     * @p pathsに格納された名前のディレクトリを順に作る
     * ./paths[0]/paths[1]/paths[2]/...
     */
    bool _makeDirectories(std::vector<std::string> paths) const;

public:
    /// 車両の静的データ（車種や形状など）を出力する
    bool writeVehicleStaticData(Vehicle* vehicle);

    /// 車両の走行距離データを出力する
    bool writeVehicleDistanceData(Vehicle* vehicle);

    /// 走行中の全車両の走行距離データを出力する
    bool writeAllVehiclesDistanceData();

    /// @}

private:
    /// 動的データファイルのPrefix
    static const std::string _timePrefix;

    /// 動的データの出力ファイル
    /*
     * zlibを使用する場合はgzFile,
     * 使用しない場合はFile*に置き換わる
     */
#ifdef USE_ZLIB
    gzFile _dynamicOut;
#else // USE_ZLIB
    FILE* _dynamicOut;
#endif // USE_ZLIB   

    /// 車両属性データファイル名
    std::string _attributeOutFileName;

    /// 車両属性データの出力ストリーム
    std::ofstream _attributeOut;

    /// 走行距離データファイル名
    std::string _tripOutFileName;

    /// 走行距離データの出力ストリーム
    std::ofstream _tripOut;

    /// 追加情報出力、0:なし、1:ウィンカ、2:先行者ID
    int _extension;
};

#endif //__VEHICLEIO_H__

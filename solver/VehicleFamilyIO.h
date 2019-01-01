/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLEFAMILY_IO_H__
#define __VEHICLEFAMILY_IO_H__

/// 車種の設定をファイルから読み込むクラス
/**
 * このクラスはインスタンスを生成しない
 *
 * @ingroup Vehicle Initialization IO
 */
class VehicleFamilyIO
{
private:
    VehicleFamilyIO(){};
    ~VehicleFamilyIO(){};

public:
    /// ファイルから車種情報を読み込み，VehicleFamilyManagerにセットする
    static void getReadyVehicleFamily();

    /// 情報を画面に出力する
    static void print();
};

#endif //__VEHICLEFAMILY_IO_H__

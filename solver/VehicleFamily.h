/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLEFAMILY_H__
#define __VEHICLEFAMILY_H__

typedef int VehicleType;

/// 車種を集中管理するクラス
/**
 * 必要十分な車種がどれくらいであるのかは不明．
 * ナンバリングは今後何度も修正する必要があろう．
 *
 * とりあえずJCAPIIのカテゴリを10の位にしておく．
 * 1の位は描画タイプによって変えることもできると思う．
 *
 * バスの経路動作等はBusクラス等で管理していて種別とは別
 * サイズや表示色のみ種別で管理
 *
 * このクラスはインスタンスを生成しない
 *
 * @ingroup Vehicle
 */
class VehicleFamily
{
public:
    // 大カテゴリ(10の位)
    static VehicleType miniPassenger();
    static bool isMiniPassenger(VehicleType type);

    static VehicleType passenger();
    static bool isPassenger(VehicleType type);

    static VehicleType bus();
    static bool isBus(VehicleType type);

    static VehicleType miniTruck();
    static bool isMiniTruck(VehicleType type);

    static VehicleType truck();
    static bool isTruck(VehicleType type);

    static VehicleType tram();
    static bool isTram(VehicleType type);

    // 小カテゴリ（1の位）

protected:
    // 大カテゴリ(10の位)
    static const VehicleType _miniPassenger  =    1;
    static const VehicleType _passenger      =    2;
    static const VehicleType _bus            =    3;
    static const VehicleType _miniTruck      =    4;
    static const VehicleType _truck          =    5;

    static const VehicleType _tram           = 9000;

    // 小カテゴリ（1の位）
};

#endif // __VEHICLEFAMILY_H__

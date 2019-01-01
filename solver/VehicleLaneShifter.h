/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLE_LANESHIFTER_H__
#define __VEHICLE_LANESHIFTER_H__

class Lane;
class Vehicle;
class RoadOccupant;

/// 車線変更関連機能を提供するクラス
/**
 * 車線変更機能のみでなく車線変更されている側の車の速度制御も担当する
 *
 * @note Vehicleのフレンドクラスとする
 *
 * @bug
 * 車線変更を行っている最中に交差点に入ると実行時エラーが起こる。
 * 現在は交差点から一定以上の距離がない場合は車線変更を始めないようにして
 * 対処している。
 *
 * @note
 * 車線変更先の車の取得が beginShift() 時のみだけなので
 * 車線変更中にレーンから出て行かれると見えなくなる？ by Y.Kato
 *
 * @ingroup Vehicle
 */
class VehicleLaneShifter
{
public:
    VehicleLaneShifter();
    ~VehicleLaneShifter(){};

    //==================================================================
    /** @name アクセッサ */
    //@{

    /// 担当する車両をセットする
    void setVehicle(Vehicle* vehicle);

    /// 車線変更が可能かどうか
    /**
     * @attention 先にcheckLaneSpaceを行っておく必要がある
     */
    bool canShift() const;

    /// 現在車線変更を行っているか
    bool isActive() const;

    /// 車両が向かう車線を返す
    Lane* laneTo() const;

    /// 車両が向かう車線上の位置を返す
    double lengthTo() const;

    //@}

    //==================================================================
    /** @name recognize内で実行する関数 */
    //@{

    /// 車線変更を行うために十分な空きがあるか
    void checkLaneSpace();

    /// 車線変更を行う側の処理
    /**
     * 車線変更先の先行車をVirtualLeaderに登録する
     */
    void searchNextLeader();

    /// 車線変更を受ける側の処理
    /**
     * 車線変更中の車をVirtualLeaderに登録する
     */
    void searchInterruption() const;

    //@}

    //==================================================================
    /** @name run内で実行する関数 */
    //@{

    /// 車線変更動作を始める
    void beginShift();

    /// 車線変更動作を進める
    /**
     * @return 動作が終了したか
     */
    void proceedShift();

    /// 車線変更動作を終える
    void endShift();

    /// 車線変更を中断する
    /**
     * 一瞬で元のレーンの中心に戻る．
     * 車線変更を行うと問題がある部分を切り抜けるために作成．
     */
    void abortShift();

    /// 車線変更を行う側のerror方向の速度を返す
    double activeErrorVelocity() const;

    //@}

private:
    /// 車線変更の方向
    bool _isLeft;

    /// 変更先の車線
    /**
     * Vehicle::runで毎回更新され（車線変更中にレーンを渡れるようにするため），
     * Vehicle::recognizeで参照される
     */
    Lane*  _laneTo;

    /// 変更先の車線での位置
    /**
     * Vehicle::runで毎回更新され（車線変更中にレーンを渡れるようにするため），
     * Vehicle::recognizeで参照される
     */
    double _lengthTo;

    double _errorTo;

    /// 担当するVehicle
    Vehicle* _vehicle;

    /// 現在車線変更を行っているかどうか
    bool _isActive;

    /// 車線変更可能かどうか
    bool _canShift;
};

#endif //__VEHICLELANESHIFTER_H__

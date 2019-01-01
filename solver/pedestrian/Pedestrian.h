/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PEDESTRIAN_H__
#define __PEDESTRIAN_H__

#include "../AmuConverter.h"
#include "../Intersection.h"
#include "../AmuVector.h"
#include "../AmuPoint.h"

class Zebra;
class Vehicle;
class RandomGenerator;

/// RecognitionStatus: 歩行速度を変更すべきかどうかの認知/判断結果
#define RecogStatus_NoChange        0   // 速度変更の必要なし
#define RecogStatus_VehicleFound    1   // 接近する車あり
#define RecogStatus_PedestrianFound 2   // 接近する歩行者あり

class Pedestrian
{
public:
    Pedestrian();
    ~Pedestrian();

    /// 乱数生成器へのポインタを返す
    /**
     * 外部からこの車両が持つ乱数生成器にアクセスする場合に利用する
     */
    RandomGenerator* randomGenerator();

    //======================================================================
    /** @name 属性に関する関数 */
    /// @{

    /// 識別番号の取得
    const std::string& id() const;
    void setId (const std::string& id);
    bool hasId () const;

    void setStartTime (ulint startTime);
    ulint startTime() const;

    /// 人体半径の取得
    double radius() const; 
    /// パーソナルスペースの取得
    double psRadius() const;

    /// 視界の長さの取得
    double viewLength() const;
    /// 視界の角度の取得
    double viewAngle() const;
  
    /// @}

    //======================================================================
    /** @name 位置に関する関数 */
    /// @{

    /// 横断歩道上にいるか
    bool isOnZebra() const;
    void setOnZebra();

    /// x座標を取得する
    double x() const;
    /// y座標を取得する
    double y() const;
    /// z座標を取得する
    double z() const;


    /// 座標を取得する
    const AmuPoint* point () const;

    /// 自分がいる交差点をセットする
    void setIntersection(const Intersection* inter);

    /// 交差点を返す
    const Intersection* intersection () const;

    /// 自分がいる横断歩道をセットする
    void setZebra (Zebra* zebra);

    /// 今いる横断歩道を返す
    Zebra* zebra () const;

    /// 位置を設定する
    void setPoint (double x, double y, double z);

    /// 位置を設定する
    void setPoint (const AmuPoint* p);

    /// @}

    //======================================================================
    /** @name 挙動に関する関数 */
    /// @{

    /// 周囲の状況を認識する
    void recognize ();
 
    /// recognize()の結果に従って、進行方向や速度を変更する
    void determineToChangeVelocity();

    /// 移動する
    void walk();

    /// 歩行速度ベクトルを設定する
    void setVelocity (const AmuVector &velocity);

    /// 速度の取得
    const AmuVector* velocity() const;

    // /方向を設定する
    void setDirection (int direction);

    /// 方向の取得
    int direction () const;

    /// 前回のrecognize()の結果を返す
    int recognitionStatus () const;

    /// 止まっているかどうか(=速度が一定以下にあるか)返す
    bool isAlmostStopped () const;

    /// 初期方向ベクトルを返す
    const AmuVector* directionVector() const;

    ///@}

    //======================================================================
protected:
    /// 乱数生成器
    RandomGenerator* _rnd;

    //======================================================================
    /** @name 属性に関する変数、関数 */
    /// @{

    /// 識別番号。ObjManager内で付与される。
    std::string _id;

    bool _hasId;

    /// 人体の半径
    double _radius;
    /// パーソナルスペース
    double _personal;
    /// 視野の長さ
    double _viewLength;
    /// 視野の角度
    double _viewAngle;

    ulint _startTime;

    /// @}

    //======================================================================
    /** @name 位置に関する変数、関数 */
    /// @{

    /// 自分がいる交差点
    const Intersection* _intersection;
    /// 自分がいる横断歩道
    Zebra* _zebra;

    /// 横断歩道にいるかどうか
    bool _onZebra;

    /// 位置。
    AmuPoint _point;

    /// 速度ベクトル
    AmuVector _velocity;

    /// 初期方向ベクトル。
    AmuVector _directionVector;

    /// 方向。これがないと速度変更時に向きが逆になったことに気が付かない。
    int _direction;

    /// @}

    //======================================================================
    /** @name 挙動に関する変数、関数 */
    /// @{

    /// 速度が0に近くなっていたら、初期方向ベクトルに再初期化する
    void _recoverDirection();

    /// 進行方向に自動車がいた場合に速度ベクトルを変更する
    void changeVelocityFor(const Vehicle* foundVehicle);

    /// 進行方向に歩行者がいた場合に速度ベクトルを変更する
    void changeVelocityFor(const Pedestrian* foundPedestrian);

    /// 最大速度を取得する
    double maxVelocity();

    /// 進行方向を基準として、特定の歩行者との左右の間隔を計算する。
    /*
     * 対象となる歩行者が向かって右に見えている場合正の値、
     * 左に見えている場合は負の値を返す。
     */
    double calcSideDistance (Pedestrian* pds);
    double calcFrontDistance (Pedestrian* pds);

    const AmuPoint calcNextPoint(AmuPoint* line0, AmuPoint* line1,
                                 bool* result_isReflected) const;

    ///位置と速度ベクトルを更新する。
    /**
     * もし横断歩道の左右を超えてはみ出す場合は、最も近い横断歩道の
     * 内側に位置を修正する。その時は速度ベクトルもはみ出ないように
     * 修正する
     */
    const AmuPoint updatePointAndVelocity(std::vector<AmuPoint*>*,
                                          bool*) const;

    /// 加速時の係数
    double _accel;

    /// 速度補正係数
    double _cV;

    /// recognize()の結果
    int _recogStatus;

    /// _recogStatusの付属データ
    /*
     * RecogStatus_VehicleFoundの時の見つかった自動車
     */
    Vehicle* _foundVehicle;

    /// _recogStatusの付属データ
    /*
     * RecogStatus_PedestrianFoundの時の見つかった歩行者
     */
    Pedestrian* _foundPedestrian;

    /// 前方に自動車がいるかどうか
    Vehicle* searchVehicle();

    /// 前方に他の歩行者がいるかどうか
    Pedestrian* searchPedestrian();

    //int detectSide (const AmuPoint* p, const AmuVector* dir);

    /// 与えられた自動車と交差するかどうか
    bool isCrossingWithVehicle(Vehicle* vehicle) const;
};

#endif //__PEDESTRIAN_H__

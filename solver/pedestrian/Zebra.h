/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ZEBRA_H__
#define __ZEBRA_H__

#include "../SubIntersection.h"
#include "../AmuPoint.h"
#include "../AmuLineSegment.h"

class Pedestrian;
class Vehicle;

/// 横断歩道クラス
/**
 * 交差点内の横断歩道を表現するためのクラス
 * データ構造上，交差点を分割するクラスSubIntersectionのサブクラスとしている
 */
class Zebra : public SubIntersection
{
public:
    Zebra(const std::string& id);
    virtual ~Zebra();

    /// 交差点内の方向IDを取得する。
    int getDirectionInIntersection();
    
    /// 歩行者を置く
    bool putPedestrian(Pedestrian* pds);

    /// 歩行者の列を返す
    std::vector<Pedestrian*>* pedestrians();

    /// 歩行者列の更新
    void renewPedestrianLine();

    /// 歩行者列の消去
    void clearPedestrian();

    /// 歩行者を_watingPedestriansに入れる
    void putWaitingPds(Pedestrian* pds, int i);

    Pedestrian* popPdsFromWaiting(int i);

    /// pdsの最も近い歩行者を探す
    Pedestrian* searchNearestPedestrian(Pedestrian* pds) const;

    /// pds1,pds2の距離を返す
    double distanceBetween(Pedestrian* pds1, Pedestrian* pds2) const;

    //  (x,y)にいるvehicleが横断歩道上を通行可能か判定する。
    /*
     *  @param x      vehicleの中心のx座標(グローバル座標)
     *  @param y      vehicleの中心のy座標(グローバル座標)
     *  @param length vehicleの長さ
     *  @param width  vehicleの幅
     *  @param time   時刻
     */
    /*
     * bool canPass
     * (double x, double y,
     *  const AmuVector* dir, double length, double width, double time);
     */
    bool canPass(Vehicle* vehicle) const;

    /// 渡り初めの位置を返す
    /**
     * @param dir               渡る方向。
     * @param baselinePosition  スタートライン上の位置(左端が0.0, 右端が1.0)。
     */
    AmuPoint initialPositionOf(int dir, double baselinePosition);

    ///渡り初めの速度を返す。
    /**
     * @param dir 渡る方向
     * @param speedRatio 規定の最大速さに対する設定速さの比率
     */
    AmuVector initialVelocityOf(int dir, double speedRatio);

    /// 計算用データを予め作成しておく。
    void initialize(int dirInIntersection);

    /// 歩行者の次のステップの位置を計算する。
    /**
     * 横断歩道の左右の端からはみ出しそうな場合は、最近傍の位置に修正する。
     */
    const AmuPoint nextPedestrianPoint(const Pedestrian* pds,
                                       bool* result_isReflected) const;

    /// 歩行者の発生率を返す
    /**
     * 交差点境界の反時計回りを上り方向とする
     */
    double pedGenProbability(bool isUp) const;

    /// 歩行者の発生率を返す
    void setPedGenProbability(bool isUp, double p);

private:
    /// 左右の境界の直線の方程式のパラメタを計算する
    void _generateSideLineFactors ();

    /// 最初にローカル座標系への変換用行列を作っておく
    void _generateTransposeMatrix ();

private:
    /// 交差点内の方向(direction)ID
    int _dirInIntersection;

    /// 歩行者発生率
    /**
     * 0(false): 時計回り(下り)，1(true): 反時計回り(上り)
     */
    double _pedGenProbability[2];

    /// この横断歩道にいる歩行者の集合
    std::vector<Pedestrian*> _tmpPedestrians;
    std::vector<Pedestrian*> _pedestrians;

    /// 交差点に登場する前の歩行者のコンテナ
    std::vector<Pedestrian*> _waitingPedestrians[2];

    double _transMat[2][2];
    double _A01, _B01, _C01;
    double _A23, _B23, _C23;
};

#endif //__ZEBRA_H__

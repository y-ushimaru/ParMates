/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_LINESEGMENT_H__
#define __AMU_LINESEGMENT_H__

#include "AmuPoint.h"
#include "AmuVector.h"
#include <iostream>

/// 二次元空間上の線分クラス
/**
 * 方向ベクトルを持つため，始点と終点を反転させると別の線分になる
 *
 * @ingroup Geometry
 */
class AmuLineSegment
{
private:
    /// 始点
    AmuPoint _pointBegin;
    /// 終点
    AmuPoint _pointEnd;
    /// 方向ベクトル
    AmuVector _mvec;

    /// 指定された2点@p pt0, @p pt1から直線を作る
    /**
     * 指定された2点から方向ベクトルを作成する
     *
     * @param pt0 始点
     * @param pt1 終点
     */
    void _setVectorFromPoints(const AmuPoint pt0, const AmuPoint pt1);

    /// 方向ベクトルをセットする
    /**
     * @attention 始点，終点と整合するかチェックを行っていないので注意．
     *
     * @param mvec 設定する方向ベクトル
     */
    void _setVector(const AmuVector& mvec);

public:
    AmuLineSegment();

    /// 既にある線分に始点、終点を設定する
    /**
     * @param beginPoint 始点
     * @param endPoint 終点
     */
    void setPoints(const AmuPoint beginPoint, const AmuPoint endPoint);

    /// 始点、終点を指定して線分を作る
    /**
     * @param beginPoint 始点
     * @param endPoint 終点
     */
    AmuLineSegment(const AmuPoint beginPoint, const AmuPoint endPoint);
    ~AmuLineSegment();

    /// 始点を返す
    const AmuPoint pointBegin() const;

    /// 終点を返す
    const AmuPoint pointEnd() const;
  
    /// 線分の長さを返す
    double length() const;

    /// 方向ベクトルを返す
    const AmuVector directionVector() const;

    /// 逆向きの線分を作成し返す
    /**
     * @return 新たに作成された逆向きの線分
     */
    const AmuLineSegment reversal() const;

    /// 点が線分の左側にあるかを返す
    /**
     * 余弦の正負で判定する
     *
     * @param point 調査したい点
     * @return 調査したい点が線分の左側にあるかどうか
     */
    bool isLeftSide(const AmuPoint& point) const;

    /// 点との距離を返す
    /**
     * @see calcNearestPoint()
     */
    double distance(const AmuPoint& point) const;
  
    /// 線分を(@p d0 : @p d1)に内分した点を生成して返す
    /**
     * @return 生成された内分点
     */
    AmuPoint createInteriorPoint(double d0,double d1) const;

    /// 他の AmuLineSegment との交点を生成する
    /**
     * @param crSLine 交じわる線
     * @param[in,out] result_point 交点（存在しない場合は何もしない）
     * @return 交点が存在するか
     */
    bool createIntersectionPoint
    (const AmuLineSegment& crSLine, AmuPoint * result_point) const;
  
    /// 垂線の足の位置を計算する
    /**
     * 想定される用途にあわせ，線分の外の点であっても返す
     *
     * @param point 垂線が通る点
     * @return 新たに生成された垂線の足
     */
    const AmuPoint calcFootOfPerpendicular(const AmuPoint& point) const;
  
    /// 垂線の足の位置を計算する
    /**
     * 返り値が負値であれば始点の外側，
     * length() より長ければ終点の外側にある．
     *
     * @param point 垂線が通る点
     * @return 始点から垂線の足までの長さ
     */
    double calcFootOfPerpendicularLength(const AmuPoint& point) const;
  
    /// 指定された点から最も近い線分上の点を計算する
    /**
     * 垂線の足が線分上にあれば垂線の足，
     * そうでなければ始点あるいは終点を返す
     *
     * @param point 基準となる点
     * @return 指定された点から最も近い点
     */
    const AmuPoint calcNearestPoint(const AmuPoint& point) const;

    /// 指定された点から最も近い線分上の点までの距離を計算する
    /**
     * @param point 基準となる点
     * @return 指定された点から最も近い点の始点からの距離
     */
    double calcNearestLength(const AmuPoint& point) const;
 
    /// z = 0 平面上の直線を返す
    /** 
     * @attention 平面上で処理するための一時的な処置
     */
    const AmuLineSegment z0() const;

    /// z = 0 平面上で線分の交差をチェックし交点を取得する
    /**
     * 平面に投影した線分同士で交差判定を行う．交点が存在する場合，
     * それを自分の直線上に射影した交差点を@p crossPointに代入する．
     *
     * @param crSLine 調査する線分
     * @param[in,out] crossPoint 交点（存在しない場合は何もしない）
     * @return 交点が存在するか
     */
    bool checkIntersectionZ0(const AmuLineSegment& crSLine,
                             AmuPoint *crossPoint);
};

#endif // __AMU_LINESEGMENT_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_VECTOR_H__
#define __AMU_VECTOR_H__

class AmuPoint;

/**
 * @addtogroup Geometry
 * @brief 幾何演算に関連するクラス群
 * @ingroup Utility
 */

/// 3次元のベクトルを表すクラス
/**
 * @ingroup Geometry
 */
class AmuVector
{
private:
    /// x成分
    double _x;

    /// y成分
    double _y;

    /// z成分
    double _z;

public:
    AmuVector();

    /// 2点からベクトルを作る
    /**
     * @param beginPoint 始点
     * @param endPoint 終点
     */
    AmuVector(const AmuPoint& beginPoint,
              const AmuPoint& endPoint);

    /// 3成分を指定してベクトルを作る。
    AmuVector(double x, double y, double z);

    ~AmuVector();

    /** @name アクセッサ */
    //@{

    /// x成分を設定する
    void setX(double x);

    /// y成分を設定する
    void setY(double y);

    /// z成分を設定する
    void setZ(double z);

    /// 3成分を設定する
    void setXYZ(double x, double y, double z);

    /// 始点と終点を指定する
    void setPoints(const AmuPoint& beginPoint,
                   const AmuPoint& endPoint);
  
    /// x成分を取得する
    double x() const;

    /// y成分を取得する
    double y() const;

    /// z成分を取得する
    double z() const;

    //@}

    /// xy平面に平行に@p thetaだけ回転させる
    void revoltXY(double theta);

    // yz平面に平行に@p thetaだけ回転させる
    // void revoltYZ(double theta);

    // zx平面に平行に@p thetaだけ回転させる
    // void revoltZX(double theta);

    // @p vecに平行な軸まわりに@p thetaだけ回転させる
    // void revolt(double theta, const AmuVector& crV);

    /// 他のベクトルとの内積を返す
    /**
     * @param crV 内積を求めるベクトル
     * @return 計算された内積の値
     */
    double calcScalar(const AmuVector& crV) const;

    /// 他のベクトルとなす角を返す(-pi[rad]からpi[rad])
    /**
     * 相手のベクトルから見たベクトルの回転角を返す
     *
     * @param crV 角度を求める基準となるベクトル
     * @return ベクトルのなす角（単位:[rad]）
     */
    double calcAngle(const AmuVector& crV) const;

    /// ベクトルの長さを返す
    double size() const;

    /// ベクトルを長さ1に正規化する
    void normalize();

    /// 他のベクトルを軸に反転する
    /**
     * @param crV 軸となるベクトル
     */
    void invert(const AmuVector& crV);
 
    // 極座標系に変換
    // void toPolar(double* result_r, double* result_angle) const;

    /** @name AmuVector演算子 */
    //@{
    AmuVector& operator+=(const AmuVector& rhs);
    AmuVector& operator-=(const AmuVector& rhs);
    AmuVector& operator*=(double k);
    AmuVector& operator/=(double d);
    AmuVector& operator=(const AmuVector& rhs);
    const AmuVector operator-() const;
    //@}
};

/**
 * @name AmuVector演算子
 * @relates AmuVector
 */
//@{

/// ベクトルの加算
inline AmuVector operator+(const AmuVector& lhs, const AmuVector& rhs)
{
    return (AmuVector(lhs) += rhs);
}

/// ベクトルの減算
inline AmuVector operator-(const AmuVector& lhs, const AmuVector& rhs)
{
    return (AmuVector(lhs) -= rhs);
}

/// ベクトルの定数倍
inline AmuVector operator*(const AmuVector& lhs, double k)
{
    return (AmuVector(lhs) *= k);
}

/// ベクトルの定数倍
inline AmuVector operator*(double k, const AmuVector& rhs)
{
    return (AmuVector(rhs) *= k);
}

/// ベクトルの定数による除算
inline AmuVector operator/(const AmuVector& lhs, double k)
{
    return (AmuVector(lhs) /= k);
}

//@}

#endif //__AMU_VECTOR_H__

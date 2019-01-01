/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_POINT_H__
#define __AMU_POINT_H__

#include <cmath>

class AmuVector;

/// 3次元空間における点を表すクラス
/**
 *  識別番号を持たずユニークであることを保証しない
 *
 *  @ingroup Geometry
 */
class AmuPoint
{
private:
    /// x座標
    double _x;

    /// y座標
    double _y;

    /// z座標
    double _z;

    /// 正しい地点であるかどうかのフラグ
    /**
     * デフォルトはtrue
     */
    bool   _flag;
  
public:
    /// 識別番号なしでx座標，y座標，z座標からPointを作成
    AmuPoint(double x=0, double y=0, double z=0);

    /// x,y,z座標とともにフラグをセットする
    AmuPoint(double x, double y, double z, bool f);

    /** @name アクセッサ */
    /// @{

    /// x座標を設定する
    void setX(double x);

    /// y座標を設定する
    void setY(double y);

    /// z座標を設定する
    void setZ(double z);

    /// 3座標を設定する
    void setXYZ(double x, double y, double z);

    /// 正しい地点かどうかのフラグを設定する
    void setFlag(bool f);
  
    /// x座標を取得する
    double x() const;

    /// y座標を取得する
    double y() const;

    /// z座標を取得する
    double z() const;

    /// 正しい地点かどうかのフラグを取得する
    bool flag() const;

    /// @}
  
    /// 2点間の距離を計算して返す
    double distance(const AmuPoint anotherPoint) const;

    /// z = 0 平面上の点を返す
    /** 
     * 歩行者処理用、平面上で処理するための一時的な処置
     */
    const AmuPoint z0() const;

    /** @name AmuPoint演算子 */
    //@{
    AmuPoint& operator+=(const AmuVector& vec);
    AmuPoint& operator-=(const AmuVector& vec);
    //@}
};

/**
 * @name AmuPoint演算子
 * @relates AmuPoint
 */
//@{

/// 点の一致
inline bool operator==(const AmuPoint& rhs, const AmuPoint& lhs)
{
    return (std::abs(rhs.x() - lhs.x()) < 10E-6
            && std::abs(rhs.y() - lhs.y()) < 10E-6
            && std::abs(rhs.z() - lhs.z()) < 10E-6);
}

/// 点の加算
inline const AmuPoint operator+(const AmuPoint& rhs,
                                const AmuPoint& lhs)
{
    AmuPoint retval(rhs.x()+lhs.x(),
                    rhs.y()+lhs.y(),
                    rhs.z()+lhs.z());
    return retval;
}

/// 点の加算
inline const AmuPoint operator+(const AmuPoint& rhs,
                                const AmuVector& lhs)
{
    AmuPoint retval(rhs);
    return retval += lhs;
}

/// 点の減算
inline const AmuPoint operator-(const AmuPoint& rhs,
                                const AmuPoint& lhs)
{
    AmuPoint retval(rhs.x()-lhs.x(),
                    rhs.y()-lhs.y(),
                    rhs.z()-lhs.z());
    return retval;
}

/// 点の減算
inline const AmuPoint operator-(const AmuPoint& rhs,
                                const AmuVector& lhs)
{
    AmuPoint retval(rhs);
    return retval -= lhs;
}

/// 乗算
inline const AmuPoint operator*(const AmuPoint& rhs, double factor)
{
    AmuPoint retval(rhs.x()*factor,
                    rhs.y()*factor,
                    rhs.z()*factor);
    return retval;
}

inline const AmuPoint operator*(double factor, const AmuPoint& rhs)
{
    return rhs * factor;
}

//@}

#endif //__AMU_POINT_H__

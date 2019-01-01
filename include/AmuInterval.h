/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_INTERVAL_H__
#define __AMU_INTERVAL_H__

/// 数直線上の区間を表すクラス
/**
 * @ingroup Geometry
 */
class AmuInterval
{
public:
    AmuInterval(){};

    /// 下限@p lower，上限@p upperを指定して区間を作る
    AmuInterval(double lower, double upper);

    ~AmuInterval(){};

    /** @name アクセッサ */
    //@{

    /// 下限を返す
    double lower() const;

    /// 上限を返す
    double upper() const;

    //@}

    /// @p numが区間に含まれるかどうか
    bool isIncluded(double num) const;

    /** @name 演算子の定義 */
    //@{

    /// 左辺の区間が右辺と重複なく小さければ真を返す
    bool operator<(const AmuInterval& rhs) const;

    /// 左辺の区間が右辺と重複なく大きければ真を返す
    bool operator>(const AmuInterval& rhs) const;

    /// 左辺の区間が右辺に含まれれば真を返す
    bool operator==(const AmuInterval& rhs) const;

    //@}

private:
    /// 下限
    double _lower;

    /// 上限
    double _upper;
};

#endif //__AMU_INTERVAL_H__

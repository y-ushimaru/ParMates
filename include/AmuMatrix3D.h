/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_MATRIX3D_H__
#define __AMU_MATRIX3D_H__

#include "AmuMatrix.h"

/// 3x3行列を表すクラス
/**
 *  @ingroup Geometry
 */
class AmuMatrix3D : public AmuMatrix
{
public:
    /// コンストラクタ
    /**
     * xijはi行、j列の要素
     * */
    AmuMatrix3D(double x00,double x01,double x02,
                double x10,double x11,double x12,
                double x20,double x21,double x22);
    ~AmuMatrix3D();

    /// 行列式を返す
    double delta() const;

    /// 逆行列に変換する
    /**
     * 変換できない場合は（delta()==0）何もしない
     *
     * @return 逆行列の作成に成功したかどうか
     */
    bool inverse();

    /// (@p i, @p j)成分を返す
    double getItem(int i, int j) const;

private:
    /// 成分
    double _matrix[3][3];
};

#endif //__AMU_MATRIX3D_H__


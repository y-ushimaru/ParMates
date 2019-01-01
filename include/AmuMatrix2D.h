/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_MATRIX2D_H__
#define __AMU_MATRIX2D_H__

#include "AmuMatrix.h"

/// 2x2行列を表すクラス
/**
 *  @ingroup Geometry
 */
class AmuMatrix2D : public AmuMatrix
{
public:
    /// コンストラクタ
    /**
     * xijはi行、j列の要素
     */
    AmuMatrix2D(double x00,double x01,
               double x10,double x11);
    ~AmuMatrix2D();

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
    double _matrix[2][2];
};

#endif //__AMU_MATRIX2D_H__

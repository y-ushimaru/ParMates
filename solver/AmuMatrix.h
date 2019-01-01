/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_MATRIX_H__
#define __AMU_MATRIX_H__

/// 行列を表す抽象基底クラス
/**
 * @ingroup Geometry
 */
class AmuMatrix
{
public:
    virtual ~AmuMatrix(){};

    /// 行列式を返す
    virtual double delta() const = 0;

    /// 逆行列に変換する
    /**
     * @return 逆行列の作成に成功したかどうか
     */
    virtual bool inverse() = 0;

    /// (@p i, @p j)成分を返す
    virtual double getItem(int i,int j) const = 0;
};

#endif

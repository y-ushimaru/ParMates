/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <string>
#include "AmuPoint.h"

/// コネクタを表すクラス
/**
 * レーン間を繋ぐ役割を持つ．
 *
 * 座標のみを扱い、ユニークであることを保証しないPointに対し
 * 識別番号を持ちユニークであることを保証する．
 *
 * 現在，識別番号および delete の権利は ObjManager が管理している．
 *
 * @ingroup RoadNetwork
 */
class Connector
{
private:
    /// 識別番号
    int _idGlobal;

    /// 位置の座標
    AmuPoint _point;

public:
    /// コンストラクタ
    /**
     * @param id 識別番号
     *  @param point 位置
     */
    Connector(int id, const AmuPoint point);

    /// デストラクタ
    ~Connector();

    /// 識別番号を設定する
    void setIdGlobal(int id);
    /// 識別番号を取得する
    int idGlobal() const;

    /// 位置を設定する
    void setPoint(const AmuPoint point);
    /// 位置を取得する
    const AmuPoint point() const;
    /// 位置の x 座標を取得する
    double x() const;
    /// 位置の y 座標を取得する
    double y() const;
    /// 位置の z 座標を取得する
    double z() const;
};

#endif //__CONNECTOR_H__

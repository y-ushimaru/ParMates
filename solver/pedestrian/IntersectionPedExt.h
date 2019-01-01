/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __INTERSECTION_PED_EXT_H__
#define __INTERSECTION_PED_EXT_H__

#include "Zebra.h"

typedef std::map<std::string,
                 Zebra*,
                 std::less<std::string> > RMAPZBR;
typedef std::map<std::string,
                 Zebra*,
                 std::less<std::string> >::iterator ITRMAPZBR;
typedef std::map<std::string,
                 Zebra*,
                 std::less<std::string> >::const_iterator CITRMAPZBR;

class Intersection;

/// 交差点の歩行者機能拡張クラス
/**
 * @ingroup Tram
 */
class IntersectionPedExt
{
public:
    IntersectionPedExt(){};
    IntersectionPedExt(Intersection* inter);
    ~IntersectionPedExt(){};

    /// 横断歩道をセットする
    void setZebra(Zebra* zebra);

    /// 横断歩道の集合を返す
    const RMAPZBR* zebras() const;

    /// 方向 @p dir対応する横断歩道を返す
    Zebra* zebra(int dir) const;

private:
    /// 対応する交差点
    Intersection* _inter;

    /// この交差点内の横断歩道
    /**
     * ZebraはSubIntersectionのサブクラスであるため
     * _inter(LaneBundleのサブクラス)->_entitiesと重複するが
     * 横断歩道特有の処理のためコンテナを保持する
     */
    RMAPZBR _zebras;
};

#endif //__INTERSECTION_PED_EXT_H__

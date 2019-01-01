/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SUBINTERSECTION_H__
#define __SUBINTERSECTION_H__

#include <string>
#include "RoadEntity.h"

/// 交差点サブセクションクラス
/**
 * 交差点を細分化するためのクラス。RoadEntityを継承。
 *
 * @ingroup roadNetwork
 */
class SubIntersection : public RoadEntity
{
public:
    SubIntersection(const std::string& id, SubroadFactory type);
    ~SubIntersection();

protected:
};

#endif //__SUBINTERSECTION_H__

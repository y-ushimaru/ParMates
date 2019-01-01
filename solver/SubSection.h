/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SUBSECTION_H__
#define __SUBSECTION_H__

#include <string>
#include "RoadEntity.h"

/// 単路サブセクションクラス
/**
 * 単路を細分化するためのクラス
 *
 * @ingroup roadNetwork
 */
class SubSection : public RoadEntity
{
public:
    SubSection(const std::string& id, SubroadFactory type);
    ~SubSection();

protected:
};

#endif //__SUBSECTION_H__

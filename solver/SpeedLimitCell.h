/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SPEEDLIMITCELL_H__
#define __SPEEDLIMITCELL_H__
#include "TimeManager.h"

class Section;

/// 制限速度の設定情報を保持するクラス
class SpeedLimitCell
{
public:
    SpeedLimitCell(ulint time, Section* section, bool isUp, double speedLimit);
    ~SpeedLimitCell();

    /// 適用開始時刻を返す
    ulint time() const;

    /// 対象となる単路のポインタを返す
    const Section* section() const;

    /// 上りかどうかを返す
    bool isUp() const;

    /// 制限速度を返す
    double speedLimit() const;

private:
    /// 適用開始時刻
    const ulint _time;

    /// 対象となる単路
    const Section* _section;

    /// 上りか下りか
    const bool _isUp;

    /// 制限速度
    const double _speedLimit;
};

#endif //__SPEEDLIMITCELL_H__

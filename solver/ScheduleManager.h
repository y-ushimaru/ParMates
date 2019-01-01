/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SCHEDULEMANAGER_H__
#define __SCHEDULEMANAGER_H__
#include <map>
#include "TimeManager.h"

class SpeedLimitCell;

/// 時間管理されたイベントを管理するクラス
/**
 * @note Managerは静的クラスとする
 * @ingroup Manager
 */
class ScheduleManager
{
public:
    /// 保持する情報を全てdeleteする 
    static void deleteAll();

    /// 制限速度情報をテーブルに格納する
    static void addSpeedLimitCell(ulint time, const SpeedLimitCell* cell);

    /// 制限速度を更新する
    static void renewSpeedLimit();

    /// 保持する情報を画面表示する
    static void print();

private:
    /// 制限速度情報を格納するテーブル
    /**
     * キーが適用開始時刻, 値がセル内容
     */
    static std::multimap<ulint, const SpeedLimitCell*> _speedLimitList;
};

#endif //__SCHEDULEMANAGER_H__

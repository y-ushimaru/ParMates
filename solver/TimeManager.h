/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__
#include <map>
#include <unordered_map>
#include <string>
#include "Conf.h"

class Clocker;

typedef std::unordered_map<std::string,
                           Clocker*> MAPCLK;
typedef std::unordered_map<std::string,
                           Clocker*>::iterator ITRMAPCLK;
typedef std::unordered_map<std::string,
                           Clocker*>::const_iterator CITRMAPCLK;

/**
 * @addtogroup Manager
 * @brief シミュレータ全般から参照される情報を管理するモジュール
 */

/// 時刻を管理するクラス
/**
 * 時間の単位は[msec]
 *
 * @note Managerは静的クラスとする
 * @ingroup Manager
 */
class TimeManager
{
public:
    /** @name シミュレータの時刻に関する関数 */
    //@{

    /// 現在時刻を返す
    /**
     * @return 現在時刻[msec]
     */
    static ulint time();

    /// 現在時刻を設定する
    /**
     * @param presentTime 現在時刻[msec]
     */
    static void setTime(ulint presentTime);

    /// 1タイムステップの時間刻み幅（delta_t）を返す
    /**
     * @return 時間刻み幅[msec]
     */
    static ulint unit();

    /// 1タイムステップの時間刻み幅（delta_t）を設定する
    /**
     * @param stepTime 時間刻み幅[msec]
     */
    static void setUnit(ulint stepTime);

    /// 現在のステップ数を得る
    /**
     * @return 現在のステップ数
     */
    static ulint step();

    /// 現在のステップ数を設定する
    /**
     * @param step 現在のステップ数
     */
    static void setStep(ulint step);

    /// 1ステップ進む
    static void increment();

    //@}

    /** @name 処理時間に関する関数 */
    //@{

    /// 計時を開始する
    /**
     * @return 正常に計時を開始できたか
     */
    static bool startClock(const std::string clockName);

    /// 計時を終了する
    /**
     * @return 正常に計時を終了できたか
     */
    static bool stopClock(const std::string clockName);

    /// すべての時計の計時結果を表示する
    static void printAllClockers();

    /// 時計を消去する
    static void deleteAllClockers();

    /// parallel 計時結果を返す
    static double outputClocker(const std::string clockName);

    //@}

private:
    TimeManager(){};
    ~TimeManager(){};

private:

    /** @name シミュレータの時刻に関する変数 */
    //@{

    /// 現在時刻[msec]
    static ulint _time;

    /// 1タイムステップの時間刻み幅（delta_t）[msec]
    /**
     * @note 標準ではdelta_t = 100[msec]
     */
    static ulint _unit;

    /// 現在のステップ数
    static ulint _step;

    //@}

    /** @name 処理時間に関する変数 */
    //@{

    /// タイマーのメインコンテナ
    static MAPCLK _clockers;

    /// parallel 計測開始時間
    static ulint _startTime;

    /// parallel 計測終了時間
    static ulint _endTime;

    //@}
};

#endif //__TIME_MANAGER_H__

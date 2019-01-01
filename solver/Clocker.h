/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __CLOCKER_H__
#define __CLOCKER_H__
#include <time.h>

/// シミュレーションに要する時間の計測に用いるクラス
/**
 * @ingroup utility
 */
class Clocker
{
public:
    Clocker();
    ~Clocker(){};

    /// 計時を開始する
    /**
     * @return 正常に計時を開始できたか
     */
    bool startClock();

    /// 計時を終了する
    /**
     * @return 正常に計時を終了できたか
     */
    bool stopClock();

    /// CPU時間の合計を返す
    double totalProcessorTime() const;

    /// 経過時間の合計を返す
    double totalTime() const;

    /// 計時結果を表示する
    void print() const;

private:
    /// その回の計時を開始した時刻(clock関数で計測)
    time_t _startTimeClock;

    /// その回の計時を開始した時刻(getrusage関数で計測)
    double _startTimeRu;

    /// その回の計時を開始した時刻(gettimeofday関数で計測)
    double _startTimeTod;

    /// CPU時間の合計
    /**
     * @note
     * マルチスレッドの場合，スレッドが使用したCPU時間の合計
     */
    double _totalProcessorTime;
    /// 経過時間の合計
    double _totalTime;

    /// 現在計時中かどうか
    bool _isClocking;
};

#endif //__CLOCKER_H__

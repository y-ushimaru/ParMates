/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <vector>
#include <string>
#include <algorithm>
#include "SignalColor.h"
#include "SignalAspect.h"
#include "SignalControlDataSet.h"
#include "Conf.h"
#include "TimeManager.h"

const int NUM_MAX_SPLIT_INT = NUM_MAX_SPLIT;
const unsigned int NUM_MAX_SPLIT_ULINT = NUM_MAX_SPLIT;

/**
 * @addtogroup Signal
 * @brief 信号
 * @ingroup RoadEnvironment
 */

/// 信号機クラス
/**
 * @sa SignalColor(色と値の関係を定義している)
 * @ingroup Signal
 */
class Signal
{
public:
    /// コンストラクタ
    Signal();
    Signal(const std::string& id);

    /// デストラクタ
    ~Signal();
  
    /// 識別番号を返す
    const std::string& id() const;

    /// 識別番号を設定する
    void setId(const std::string&);

    /// 有効なdirectionの数を返す
    int numDirections() const;

    /// サイクル（周期）を返す
    int cycle() const;

    /// 現示パターンをセットする
    void setStateSet(const std::vector<SignalAspect>& stateSet);

    /// 信号制御情報をセットする
    void setSignalControlDataSet(const SignalControlDataSet& controlData);

    /// 信号が準備できたかどうか
    bool isReady();

    /// 信号の色を返す
    SignalMainState mainColor(const int direction) const;

    /// 信号の矢印を返す
    SignalSubState subColor(const int direction) const;

    /// 歩行者の現示を返す
    SignalWalkerState walkerColor(const int direction) const;

    /// 現在の現示の一つ前の状態を返す
    SignalMainState prevMainColor(const int direction) const;
    SignalSubState prevSubColor(const int direction) const;
    SignalWalkerState prevWalkerColor(const int direction) const;

    /// 信号の進入許可の列挙
    enum SignalPermission
    {
        PROHIBITION, //< 許可しない
        PERMISSION,  //< 許可する
        CRAWLING,    //< 徐行制限
        PAUSING      //< 一旦停止
    };

    /// 信号が境界@p directionからの侵入を許可するか
    /**
     * @return 0 許可しない
     * @return 1 許可する
     * @return 2 許可する（徐行制限）
     * @return 3 許可する（一旦停止）
     */
    SignalPermission permission(const int direction) const;

    /// デバッグ用出力
    void crt() const;

protected:
    /// 識別番号
    std::string _id;

    /// 現示パターン
    std::vector<SignalAspect> _stateSet;

    /// 信号制御情報
    SignalControlDataSet _controlData;

    /// 信号の準備状況
    /**
     * 1:コンストラクタが呼ばれた直後
     * 2:StateかControlDataSetのどちらか一方が読み込まれた
     * 3:準備完了（StateとControlDataSetの両方が読み込まれた）
     */
    int _isReady;
};

#endif //__SIGNAL_H__

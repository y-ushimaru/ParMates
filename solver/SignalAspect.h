/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNAL_ASPECT_H__
#define __SIGNAL_ASPECT_H__

#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include "SignalColor.h"

/// 信号の状態を格納する構造体
/**
 * @ingroup Signal
 */
struct Triplet
{
    SignalMainState   main;
    SignalSubState    sub;
    SignalWalkerState walker;
};

/// 信号現示を表すクラス
/**
 * @sa Signal
 * @ingroup Signal
 */
class SignalAspect
{
public:
    SignalAspect();
    SignalAspect(std::vector<Triplet> state);
    ~SignalAspect();

    bool setSignalState(std::vector<Triplet> state);
    bool isReady();

    /// 有効なdirectionの数を返す
    int numDirections() const; 

    /// 信号の色を返す
    SignalMainState mainColor(const int direction) const;

    /// 信号の矢印を返す
    SignalSubState subColor(const int direction) const;

    /// 歩行者用現示を返す
    SignalWalkerState walkerColor(const int direction) const;
  
    /// 色と矢印のペアを返す
    Triplet colors(const int direction) const;
    
    /// デバッグ用出力
    void crt() const;

protected:

    std::vector<Triplet> _state;

    /// 初期化の際タイプに合った要求かどうか
    bool _numSignalIsValid(const int numSignalElement);

    /// 色の要求の際にタイプに合った要求かどうか
    bool _isValidRequest(const int direction) const;
};

#endif

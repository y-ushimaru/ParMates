/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNALCOLOR_H__
#define __SIGNALCOLOR_H__

typedef int SignalMainState;
typedef int SignalSubState;
typedef int SignalWalkerState;

/// 信号の色を一括して管理する。
/** 
 * 静的クラスである。
 *
 * @ingroup Signal
 */
class SignalColor
{
public:

    /** @name main color 信号の色 */
    //@{
    /// 青色の灯火
    static SignalMainState blue();
    /// 赤色の灯火
    static SignalMainState red();
    /// 黄色の灯火
    static SignalMainState yellow();
    /// 赤色の点滅
    static SignalMainState redblink();
    /// 黄色の点滅
    static SignalMainState yellowblink();
    //@}
  
    /** @name sub color 信号の矢印 */
    //@{
    /// 矢印なし
    static SignalSubState none();
    /// 全方向
    static SignalSubState all();
    /// 直進
    static SignalSubState straight();
    /// 左折
    static SignalSubState left();
    /// 右折
    static SignalSubState right();
    /// 直進と左折
    static SignalSubState straightLeft();
    /// 直進と右折
    static SignalSubState straightRight();
    /// 左折と右折
    static SignalSubState leftRight();
    //@}

private:
    // main color
    static SignalMainState _blue;
    static SignalMainState _red;
    static SignalMainState _yellow;
    static SignalMainState _redblink;
    static SignalMainState _yellowblink;

    // sub color,信号の矢印を表現
    static SignalSubState _none;//矢印なし
    static SignalSubState _all;//全方向
    static SignalSubState _straight;//直進
    static SignalSubState _left;//左折
    static SignalSubState _right;//右折
    static SignalSubState _straightLeft;//直進と左折
    static SignalSubState _straightRight;//直進と右折
    static SignalSubState _leftRight;//左折と右折

private:
    SignalColor();
    ~SignalColor();
};

#endif // __SIGNALCOLORDEFINITION_H__

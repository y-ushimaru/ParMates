/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __BLINKER_H__
#define __BLINKER_H__

#include "RelativeDirection.h"

/// 車のウィンカーを表すクラス
/**
 * @ingroup Vehicle
 */
class Blinker
{
public:
    Blinker();
    ~Blinker();

    /// 何もついてない
    bool isNone() const;

    /// 左方向指示
    bool isLeft() const;

    /// 右向指示
    bool isRight() const;

    /// ハザード
    bool isHazard() const;

    /// 状態
    int state() const;

    /// ウィンカーが示す交差点進行方向
    RelativeDirection turning() const;

    bool operator==(Blinker& another) const;

    /// 消す
    void setNone();

    /// 左合図
    void setLeft();

    /// 右合図
    void setRight();

    /// ハザードをつける
    void setHazard();

private:
    /// 状態
    /**
     * 内部的には 0:None, 1:Left, 2:Right 3:Hazard
     *
     * @attention 同時につくことは考えていない
     */
    int _state;

};
#endif //__BLINKER_H__

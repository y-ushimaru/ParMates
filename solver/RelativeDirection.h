/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __RELATIVE_DIRECTION_H__
#define __RELATIVE_DIRECTION_H__

/// 相対方向を表す列挙型．
/** 
 * ビット演算を利用しフラグとして使えるように値を割り当ててある。<br>
 * RelativeDirection rd = (RD_RIGHT | RD_STRAIGHT) として複数を同時に設定したり
 * 特定のフラグが立っているかどうかを (rd & RD_RIGHT) のようにテストできる<br><br>
 * 
 * 拡張のため、五叉路などで特殊な”方向”を設定できるように
 * 交差点固有矢印を設定できるようにしてある。<br>
 * だが，これだけで十分かはわからない．
 *
 *  @ingroup roadNetwork
 */
enum RelativeDirection {
    RD_NONE     = 0x0000, //!< 0000 0000: 値なし

    RD_BACK     = 0x0001, //!< 0000 0001: 転回
    RD_RIGHT    = 0x0002, //!< 0000 0010: 右折
    RD_STRAIGHT = 0x0004, //!< 0000 0100: 直進
    RD_LEFT     = 0x0008, //!< 0000 1000: 左折

    RD_EXT1     = 0x0010, //!< 0001 0000: 交差点固有拡張方向(1)
    RD_EXT2     = 0x0020, //!< 0010 0000: 交差点固有拡張方向(2)
    RD_EXT3     = 0x0040, //!< 0100 0000: 交差点固有拡張方向(3)
    RD_EXT4     = 0x0080, //!< 1000 0000: 交差点固有拡張方向(4) 以下必要に応じて増やす

    RD_MAX,               //!< データの最大値。

    RD_ANY      = 0xffffffff //!< すべてのビットが立っている
};

#endif //__RELATIVE_DIRECTION_H__

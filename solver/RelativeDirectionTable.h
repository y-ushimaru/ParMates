/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __RELATIVE_DIRECTION_TABLE_H__
#define __RELATIVE_DIRECTION_TABLE_H__

#include "RelativeDirection.h"
#include <vector>

/// 相対方向テーブルクラス
/**
 * 抽象基底クラス
 *
 * 境界@c i から境界@c j に向かう方向を返す二次元テーブル。<br>
 * RelativeDirectionTable rdt に対し rdt(i,j)でアクセスする。<br>
 * 例えば i==j のとき rdt(i,j) は RD_BACK を返す。
 */
class RelativeDirectionTable
{
public:
    virtual ~RelativeDirectionTable() = 0;
    virtual int size() const = 0;
    virtual RelativeDirection operator()(int i, int j) const = 0;
};

//######################################################################
/// 端点専用相対方向テーブル
class RelativeDirectionTableTerminal : public RelativeDirectionTable
 {
public:
    virtual int size() const { return 1; }
    virtual RelativeDirection operator()(int i, int j) const;
};

//######################################################################
/// 二方向路専用相対方向テーブル
class RelativeDirectionTable2Way : public RelativeDirectionTable
{
public:
    virtual int size() const { return 2; }
    virtual RelativeDirection operator()(int i, int j) const;
};

//######################################################################
/// T字路専用相対方向テーブル
class RelativeDirectionTableTJunction : public RelativeDirectionTable
{
public:
    explicit RelativeDirectionTableTJunction(const int start);
    virtual int size() const { return 3; }
    virtual RelativeDirection operator()(int i, int j) const;
private:
    /// 始点。T字の左肩の部分が入る
    const int _start;
    /// 共通データ
    static const RelativeDirection _table[3][3];
};

//######################################################################
/// 十字路専用相対方向テーブル
class RelativeDirectionTableCross : public RelativeDirectionTable
{
public:
    virtual int size() const { return 4; }
    virtual RelativeDirection operator()(int i, int j) const;
};

//######################################################################
/// カスタム相対方向テーブル
/**
 * 交差点独自の相対方向テーブルを設定するためのクラス。<br>
 * setItem(i,j,RD_RIGHT)のような形で任意の値を設定できる。
 */
class RelativeDirectionTableCustom : public RelativeDirectionTable
{
private:
    const int _size;
    std::vector<RelativeDirection> _table;
public:
    explicit RelativeDirectionTableCustom(int size);
    virtual int size() const { return _size; }
    virtual RelativeDirection operator()(int i, int j) const;
    virtual void setItem(int i, int j, RelativeDirection rd);
};

#endif //__RELATIVE_DIRECTION_TABLE_H__

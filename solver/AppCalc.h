/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __APP_CALC_H__
#define __APP_CALC_H__

#include "AppMates.h"
#include <getopt.h>

/// advmates-calcアプリケーションクラス
/**
 * 可視化せず，計算のみを行う
 *
 * @ingroup Initialization
 */
class AppCalc : public AppMates
{
public:
    AppCalc() : AppMates(DEFAULT_MAX_TIME){};
    virtual ~AppCalc(){};

    /// 初期化とコマンドライン引数の処理
    /**
     * @param argc コマンドライン引数の数
     * @param argv コマンドライン引数
     * @param output 結果出力フラグを設定するかどうか
     */
    void init(int argc, char** argv, bool output);

    /// 計算と結果出力
    int batchRun();

protected:
    /// コマンドライン引数を処理する
    virtual void parseArgument(int argc, char** argv);

    /// 説明を出力する
    virtual void printUsage();

protected:
    /// デフォルト計算時間
    static const unsigned long DEFAULT_MAX_TIME = 3600ul * 1000ul; // 1 hour

};

#endif //__APP_CALC_H__

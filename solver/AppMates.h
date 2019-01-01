/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __APPMATES_H__
#define __APPMATES_H__

#include <string>
#include <getopt.h>
#include "Simulator.h"

/**
 * @addtogroup Procedure
 * @brief シミュレーション手順を定義するモジュール
 */

/**
 * @addtogroup Initialization
 * @brief シミュレーションの初期化と開始
 * @ingroup Procedure
 */

/// アプリケーションの基底クラス
/**
 * データファイルや乱数の種などを管理する
 *
 * @ingroup Initialization
 */
class AppMates
{
public:
    AppMates(){};
    AppMates(unsigned long maxTime);
    virtual ~AppMates();

    /// 初期化とコマンドライン引数の処理
    virtual void init(int argc, char** argv, bool output);

    /// シミュレータを返す
    Simulator* simulator();

protected:
    /// コマンドライン引数を処理する
    virtual void parseArgument(int argc, char** argv);

    /// 説明を出力する
    virtual void printUsage();

    /// シミュレータの初期設定を行う
    bool getReadySimulator();

private:
    /// データディレクトリへのパスを指定する
    /**
     * @param arg ディレクトリ名
     */
    bool _setDataPath(int argc, char** argv);

    /// 乱数の種を指定する
    /**
     * @param arg 指定する種(文字列で与える)
     */
    bool _setRandomSeed(std::string arg);

protected:
    Simulator* _simulator;

    /// データディレクトリへのパス
    std::string _dataPath; 

    /// 乱数の種
    unsigned int _key;

    /// 計算時間
    unsigned long _maxTime;

    /** @name コマンドラインオプションの解析に用いる変数 **/
    //@{
    static int optionIndex;
    static std::string shortOptions;
    static struct option longOptions[];
    //@}
};

#endif //__APPMATES_H__

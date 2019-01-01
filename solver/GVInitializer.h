/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GV_INITIALIZER_H__
#define __GV_INITIALIZER_H__

#include <string>

/// シミュレーションに用いる定数を初期化するクラス
/**
 * 入出力に用いるファイルをGVManagerに登録する。
 * このクラスはインスタンスを作成しない。
 *
 * @ingroup Initialization
 */
class GVInitializer
{
private:
    GVInitializer(){};
    ~GVInitializer(){};

public:
    /// シミュレーションに用いる定数を初期化する
    /*
     * @param dataPath データディレクトリのパス
     */
    static void init(const std::string& dataPath);
};

#endif //__GV_INITIALIZER_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <memory>
#include <cstdlib>
#include "AppCalc.h"

using namespace std;

/// アプリケーションオブジェクト
unique_ptr<AppCalc> app;

/// exit時に実行する関数
/**
 * appのデストラクタを明示的に呼び出す
 *
 * @sa App
 */
void exitFunc()
{
    app.reset();
}

/// advmates-calcのmain関数
/**
 * AppMatesを作成して初期化を行い，シミュレーションを開始する．
 * 計算のみ行い，結果の可視化（アニメーション）を行わない．
 */
int main(int argc, char* argv[])
{
    /*
     * auto_ptrにより自動的にデストラクタが呼び出されるはずだが
     * 環境によりメモリリークが発生するため，
     * exit前に明示的に呼び出すことで回避する．
     */
    atexit(exitFunc);

    app.reset(new AppCalc());
    app->init(argc, argv, true);

    return app->batchRun();
}

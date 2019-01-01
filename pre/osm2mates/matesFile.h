/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
// MATES ファイルアクセス

#ifndef MATESFILE_H
#define MATESFILE_H

#include <Qt>
#include <string>
#include <sstream>

#define QT_FATAL_ASSERT

using namespace std;

// MATES ファイルアクセス
class MatesFile
{
    string      _mapPosFile;                            //マップ位置ファイル
    string      _networkFile;                           //ネットワークファイル
    string      _errorFile;                             //エラーファイル
    string      _intsecDir;                             //交差点ディレクトリ
public:
    //コンストラクタ
    MatesFile();
    //読み込み前処理
    void        prevRead();
    //書き込み前処理
    void        prevWrite();
    //読み込み
    bool        read();
    //書き込み
    bool        write();
private:
    //ファイルチェック
    bool        checkFile(const char* file);
    //マップ位置ファイル読み込み
    bool        readMapPos();
    //ネットワークファイル読み込み
    bool        readNetwork();
    //交差点ID取得とチェック、エラーなら MAP_NOID
    int         intsecId(const char* id);
    //位置取得とチェック、エラーなら Ok が false、データを補正
    double      pos(const char* posStr, bool* ok);
    //車線数種得、エラーなら -1
    int         lane(const char lane);
    //マップ位置ファイル書き込み
    bool        writeMapPos();
    //ネットワークファイル書き込み
    bool        writeNetwork();
    //エラーファイル書き込み 
    bool        writeError();
};

#endif // FILE_H

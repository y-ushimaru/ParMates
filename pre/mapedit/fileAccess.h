/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//ファイルアクセス

#ifndef FILEACCESS_H
#define FILEACCESS_H

#include <QtGui>
#include <string>
#include <sstream>

#define QT_FATAL_ASSERT

using namespace std;

//ファイルアクセス
class FileAccess
{
    QWidget*    _pParent;                               //親画面
    string      _setupFile;                             //設定ファイル
    string      _mapPosFile;                            //マップ位置ファイル
    string		_networkFile;                           //ネットワークファイル
    string		_errorFile;                             //エラーファイル
public:
    FileAccess();                                       //コンストラクタ
    void        init(QWidget* pParent, const char* pSetupFile);
                                                        //初期処理
    bool        read(bool* pCacncel, bool noSetupFile); //読み込み（キャンセルならデータ変更なし）
    bool        write();                                //書き込み
private:
    bool        checkFile(const char* pFile);           //ファイルチェック
    bool        readMapPos();                           //マップ位置ファイル読み込み
    bool        readNetwork();                          //ネットワークファイル読み込み
    int         getIntsecId(const char* pIntsecId);     //交差点ID取得とチェック、エラーなら MAP_NOID
    double      getPos(const char* pPos, bool* pOk);    //位置取得とチェック、エラーなら Ok が false、データを補正
    int         getLane(const char lane);               //車線数種得、エラーなら NOLANE
    bool        writeMapPos();                          //マップ位置ファイル書き込み
    bool        writeNetwork();                         //ネットワークファイル書き込み
    bool        checkError();                           //エラーチェック、エラーなら true
    bool        writeError();                           //エラーファイル書き込み
    bool        getQuoteString(const char* pSource, int startPos, string* pQuote);
                                                        //"..."から文字列抽出、日本語不可、エラーなら false
    bool        errorMessage(const char* pMessage);     //エラーメッセージ、常に false
};

#endif // FILE_H

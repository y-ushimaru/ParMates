/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//アプリケーション

#ifndef APP_H
#define APP_H

#include <QtGui>
#include <stdio.h>
#include <string>
#include "shMem.h"

#define QT_FATAL_ASSERT

using namespace std;

#define APP_DEFAULTFILE     "mapEditor.mie"                     //ファイル名
#define APP_DEFAULTFILE_ALL "*.mie"
#define APP_MAPPOSFILE      "mapPosition.txt"
#define APP_NETWORKFILE     "network.txt"
#define APP_ERRORFILE       "mapEditorError.txt"
#define APP_MAGNI_MAX       100                                 //最大／最小倍率
#define APP_MAGNI_MIN       0.0001
#define APP_POS_MAX         1000000000                          //最大位置
#define APP_POS_MIN         -1000000000                         //最小位置

class MainWindow;
class MapFrame;
class Map;

//アプリケーション
class App
{
    MainWindow* _pMainWindow;                               //中核オブジェクト
    MapFrame*   _pMapFrame;
    Map*        _pMap;
    string      _setupFile;                                 //設定ファイル
    double      _viewMapX, _viewMapY;                       //表示マップ位置（左下、m）
    double      _viewMagni;                                 //表示倍率（表示／マップ、m／pixel）
    bool        _clearView;                                 //透過表示
    bool        _showId;                                   	//ID表示
    bool        _backImage;                                 //背景画像
    string      _backFile;                                  //背景画像ファイル
    double      _backMapX, _backMapY;                       //背景マップ位置（左下、m）
    double      _backMagni;                                 //背景倍率（マップ／背景、pixel/m）
    ShMem       _shMem;                                     //共有メモリ

public:

    App()                                                   //コンストラクタ
    {
        _pMainWindow = NULL;
        _pMapFrame = NULL;
        _pMap = NULL;
        setDefault();
    }
    void init(MainWindow* pmw, MapFrame* pmf, Map* pm)      //初期処理
    {
        _pMainWindow = pmw;
        _pMapFrame = pmf;
        _pMap = pm;
    }
    void setDefault()                                        //既定値設定
    {
        _setupFile.clear();
        _viewMapX = _viewMapY = 0.0;
        _viewMagni = 1.0;
        _clearView = false;
        _showId = true;
        _backImage = false;
        _backFile.clear();
        _backMapX = _backMapY = 0.0;
        _backMagni = 1.0;
    }

    MainWindow* getMainWindow()                               //中核オブジェクト取得
    {
        return _pMainWindow;
    }
    MapFrame* getMapFrame()
    {
        return _pMapFrame;
    }
    Map* getMap()
    {
        return _pMap;
    }
    void setSetupFile(const char* pSetupFile)                 //共通情報設定／取得
    {
        _setupFile = pSetupFile;
    }
    void getSetupFile(string* pSetupFile)
    {
        *pSetupFile = _setupFile;
    }
    void setViewMap(double x, double y)
    {
        _viewMapX = x;
        _viewMapY = y;
    }
    double getViewMapX()
    {
        return _viewMapX;
    }
    double getViewMapY()
    {
        return _viewMapY;
    }
    void setViewMagni(double magni)
    {
        _viewMagni = magni;
    }
    double getViewMagni()
    {
        return _viewMagni;
    }
    void setClearVivew(bool cv)
    {
        _clearView = cv;
    }
    bool getClearView()
    {
        return _clearView;
    }
    void setShowId(bool si)
    {
        _showId = si;
    }
    bool getShowId()
    {
        return _showId;
    }
    void setBackImage(bool bi)
    {
        _backImage = bi;
    }
    bool getBackImage()
    {
        return _backImage;
    }
    void setBackFile(const char* pBackFile)
    {
        _backFile = pBackFile;
    }
    void getBackFile(string* pBackFile)
    {
        *pBackFile = _backFile;
    }
    void setBackMap(double x, double y)
    {
        _backMapX = x;
        _backMapY = y;
    }
    double getBackMapX()
    {
        return _backMapX;
    }
    double getBackMapY()
    {
        return _backMapY;
    }
    void setBackMagni(double magni)
    {
        _backMagni = magni;
    }
    double getBackMagni()
    {
        return _backMagni;
    }
    ShMem& shMem()
    {
        return _shMem;
    }

    bool backFileDialog(QWidget* pParent, string* pBackFile);   //背景画像ファイルダイアログ
                                                                //背景画像ファイル名を指定し戻す
    bool measureLinePoint(int lineX1, int lineY1, int lineX2, int lineY2,
                          int pointX, int pointY, int minDistance, double* pDistance);
                                                                //直線と点の計測、最小以内なら true

    void configEditor(int intsecId);                            //設定エディタ
};

#ifdef MAIN_CPP
App app;
#else
extern App app;
#endif

#endif // APP_H



/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//アプリケーション

#ifndef APP_H
#define APP_H

#include <Qt>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

#define QT_FATAL_ASSERT

using namespace std;

// 緯度経度最大
#define APP_LATLON_MAX           10000

// OSM 道路種別
#define APP_OSM_RT_MOTORWAY      0
#define APP_OSM_RT_MOTORWAY_LINK 1
#define APP_OSM_RT_TRUNK         2
#define APP_OSM_RT_TRUNK_LINK    3
#define APP_OSM_RT_PRIMARY       4
#define APP_OSM_RT_PRIMARY_LINK  5
#define APP_OSM_RT_SECONDARY     6
#define APP_OSM_RT_TERTIARY      7
#define APP_OSM_RT_UNCLASSIFIED  8
#define APP_OSM_RT_RESIDENTIAL   9
#define APP_OSM_RT_NUM           10

// エラーID、intsec.cpp と road.cpp に合わせること
#define APP_ERROR_ID_INTSEC      0
#define APP_ERROR_ID_ROAD        10
#define APP_ERROR_ID_NUM         20

class Map;

//アプリケーション
class App
{
    Map*            _map;                       // MATES 地図
    string          _setupFile;                 // 設定ファイル

    bool            _osm;                       // OSM 変換
    bool            _modify;                    // 修正
    string          _inputOSM_File;             // OSM入力ファイル
    string          _inputMatesDir;             // Mates入力ディレクトリ
    string          _outputDir;                 // 出力ディレクトリ
    bool            _noConfirm;                 // 確認なし
    int             _reportIntv;                // レポート間隔（ms）
    double          _latMin, _latMax;           // 緯度経度制限
    double          _lonMin, _lonMax;
    bool            _latLonJapan;               // 日本測地系
    vector<int>     _osmRT_Lane;                // OSM 道路種別ごとのレーン数 0 なら道路なし
    vector<string>  _osmRT_SetupKey;            // OSM 道路種別ごと設定ファイルキー
    vector<string>  _osmRT_Key;                 // OSM 道路種別ごと OSM ファイルキー
    double          _modMergeLen;               // 統合距離、-1 なら統合なし
    int             _modMergeFind;              // 統合検索回数
    int             _modForceOneLane;           // 強制１レーン、指定値以下のレーンが対象、-1 ならなし

    bool            _setLatLonMid;              // 緯度経度変換設定
    double          _latMid, _lonMid;           // 中間
    double          _latToY, _lonToX;           // 変換係数
    double          _latBorder, _lonBorder;     // 境界

public:
    // コンストラクタ
    App();

    // 初期処理、設定ファイル読み込み
    bool init(Map* map, const char* setupFile);

    // 情報取得
    Map* map()
    {
        return _map;
    }
    string* setupFile()
    {
        return &_setupFile;
    }
    bool osm()
    {
        return _osm;
    }
    bool modify()
    {
        return _modify;
    }
    string* inputOSM_File()
    {
        return &_inputOSM_File;
    }
    string* inputMatesDir()
    {
        return &_inputMatesDir;
    }
    string* outputDir()
    {
        return &_outputDir;
    }
    bool noConfirm()
    {
        return _noConfirm;
    }
    int reportIntv()
    {
        return _reportIntv;
    }

    int osmRT_Lane(int osmRT)
    {
        return _osmRT_Lane[osmRT];
    }
    string* osmRT_Key(int osmRT)
    {
        return &_osmRT_Key[osmRT];
    }
    double modMergeLen()
    {
        return _modMergeLen;
    }
    int modMergeFind()
    {
        return _modMergeFind;
    }
    int modForceOneLane()
    {
        return _modForceOneLane;
    }

    // 緯度経度範囲内
    bool inLatLonMinMax(double lat, double lon, bool* border);

    bool inLatLonMinMaxArea(double latMin, double latMax,
                            double lonMin, double lonMax);
    // 緯度経度変換
    void setLatLonMid(double latMid, double lonMid); 
    double latToY(double lat);
    double lonToX(double lon);
    void latLonJapanToWorld(double* lat, double* lon);

private:
    bool getSetupBoolData(string* key, stringstream* line, bool* boolData);

    // 設定ファイルデータ読み込み、日本語不可
    bool getSetupData(string* key, stringstream* line, int* intData=NULL,
                      double* doubleData=NULL, string* stringData=NULL);

};

#ifdef APP_CPP
App app;
#else
extern App app;
#endif

#endif // APP_H

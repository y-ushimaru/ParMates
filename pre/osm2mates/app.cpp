/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//アプリケーション
//マップエディタの処理を元に作成

#include <Qt>
#include <QFileInfo>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <math.h>
#define APP_CPP
#include "app.h"

using namespace std;

//設定ファイル、OSM 道路種別は App コンストラクタ内
#define SETUP_KEY_FILETYPE              "OSM2MATES_SETUP" //設定ファイルキー
#define SETUP_KEY_OSM                   "OSM"
#define SETUP_KEY_MODIFY                "MODIFY"
#define SETUP_KEY_INPUT_OSM_FILE        "INPUT_OSM_FILE"
#define SETUP_KEY_INPUT_MATES_DIR       "INPUT_MATES_DIR"
#define SETUP_KEY_OUTPUT_DIR            "OUTPUT_DIR"
#define SETUP_KEY_NO_CONFIRM            "NO_CONFIRM"
#define SETUP_KEY_REPORT_INTV           "REPORT_INTV"
#define SETUP_KEY_LAT_MIN               "LAT_MIN"
#define SETUP_KEY_LAT_MAX               "LAT_MAX"
#define SETUP_KEY_LON_MIN               "LON_MIN"
#define SETUP_KEY_LON_MAX               "LON_MAX"
#define SETUP_KEY_LATLON_JAPAN          "LATLON_JAPAN"
#define SETUP_KEY_MOD_MERGE_LEN         "MOD_MERGE_LEN"
#define SETUP_KEY_MOD_MERGE_FIND        "MOD_MERGE_FIND"
#define SETUP_KEY_MOD_FORCE_ONE_LANE    "MOD_FORCE_ONE_LANE"
#define SETUP_VAL_FILETYPE              "1.1" //設定ファイル値
#define SETUP_VAL_TRUE                  "true"
#define SETUP_VAL_FALSE                 "false"

App::App()
{
    _map = NULL;
    _osm = _modify = false;
    _noConfirm = false;
    _reportIntv = 1000;
    _latLonJapan = false;
    _latMin = _lonMin = -APP_LATLON_MAX;
    _latMax = _lonMax =  APP_LATLON_MAX;

    for (int i = 0; i < APP_OSM_RT_NUM; i++)
    {
        _osmRT_Lane.push_back(1);
        _osmRT_SetupKey.push_back("");
        _osmRT_Key.push_back("");
    }
    _modMergeLen = 30.0;
    _modMergeFind = 15;
    _modForceOneLane = -1;

    //レーン数 0 なら道路自体なし
    _osmRT_SetupKey[APP_OSM_RT_MOTORWAY]      = "OSM_LANE_MOTORWAY";
    _osmRT_SetupKey[APP_OSM_RT_MOTORWAY_LINK] = "OSM_LANE_MOTORWAY_LINK";
    _osmRT_SetupKey[APP_OSM_RT_TRUNK]         = "OSM_LANE_TRUNK";
    _osmRT_SetupKey[APP_OSM_RT_TRUNK_LINK]    = "OSM_LANE_TRUNK_LINK";
    _osmRT_SetupKey[APP_OSM_RT_PRIMARY]       = "OSM_LANE_PRIMARY";
    _osmRT_SetupKey[APP_OSM_RT_PRIMARY_LINK]  = "OSM_LANE_PRIMARY_LINK";
    _osmRT_SetupKey[APP_OSM_RT_SECONDARY]     = "OSM_LANE_SECONDARY";
    _osmRT_SetupKey[APP_OSM_RT_TERTIARY]      = "OSM_LANE_TERTIARY";
    _osmRT_SetupKey[APP_OSM_RT_UNCLASSIFIED]  = "OSM_LANE_UNCLASSIFIED";
    _osmRT_SetupKey[APP_OSM_RT_RESIDENTIAL]   = "OSM_LANE_RESIDENTIAL";

    _osmRT_Key[APP_OSM_RT_MOTORWAY]      = "motorway";
    _osmRT_Key[APP_OSM_RT_MOTORWAY_LINK] = "motorway_link";
    _osmRT_Key[APP_OSM_RT_TRUNK]         = "trunk";
    _osmRT_Key[APP_OSM_RT_TRUNK_LINK]    = "trunk_link";
    _osmRT_Key[APP_OSM_RT_PRIMARY]       = "primary";
    _osmRT_Key[APP_OSM_RT_PRIMARY_LINK]  = "primary_link";
    _osmRT_Key[APP_OSM_RT_SECONDARY]     = "secondary";
    _osmRT_Key[APP_OSM_RT_TERTIARY]      = "tertiary";
    _osmRT_Key[APP_OSM_RT_UNCLASSIFIED]  = "unclassified";
    _osmRT_Key[APP_OSM_RT_RESIDENTIAL]   = "residential";

    _setLatLonMid = false;
    _latMid = _lonMid = _latBorder = _lonBorder = APP_LATLON_MAX;
    _latToY = _lonToX = 0;
}

//初期処理、設定ファイル読み込み
bool App::init(Map* map, const char* setupFile)
{
    ifstream ifs;
    char readBuff[1024];
    string fileType;
    stringstream line;
    double lat, lon;

    _map = map;
    lat = lon = APP_LATLON_MAX;

    //設定ファイル読み込み、位置は制限なし、不正なら補正
    if (setupFile != NULL)
    {

        cout << "read setup file \"" << setupFile << "\"" << endl;

        _setupFile = setupFile;

        string key;
        ifs.open(_setupFile.c_str(), ios::in);
        if (!ifs)
        {
            cerr << "setup file open error" << endl;
            return false;
        }
        while(ifs.getline(readBuff, sizeof(readBuff)))
        {
            line.str("");
            line.clear();
            line << readBuff;
            key = "";
            line >> key;
            if (key.size() == 0 || key.at(0) == '#')  //空白かコメント
                 continue;
//          qDebug("read '%s' key '%s'", line.str().c_str(), key.c_str());
            if (key == SETUP_KEY_FILETYPE)
            {
                if (!getSetupData(&key, &line, NULL, NULL, &fileType))
                    return false;
            }
            else if (key == SETUP_KEY_OSM)
            {
                if (!getSetupBoolData(&key, &line, &_osm))
                    return false;
            }
            else if (key == SETUP_KEY_MODIFY)
            {
                if (!getSetupBoolData(&key, &line, &_modify))
                    return false;
            }
            else if (key == SETUP_KEY_INPUT_OSM_FILE)
            {
                if (!getSetupData(&key, &line, NULL, NULL, &_inputOSM_File))
                    return false;
            }
            else if (key == SETUP_KEY_INPUT_MATES_DIR)
            {
                if (!getSetupData(&key, &line, NULL, NULL, &_inputMatesDir))
                    return false;
            }
            else if (key == SETUP_KEY_OUTPUT_DIR)
            {
                if (!getSetupData(&key, &line, NULL, NULL, &_outputDir))
                    return false;
            }
            else if (key == SETUP_KEY_NO_CONFIRM)
            {
                if (!getSetupBoolData(&key, &line, &_noConfirm))
                    return false;
            }
            else if (key == SETUP_KEY_REPORT_INTV)
            {
                if (!getSetupData(&key, &line, &_reportIntv))
                    return false;
            }
            else if (key == SETUP_KEY_LAT_MIN)
            {
                if (!getSetupData(&key, &line, NULL, &_latMin))
                    return false;
                lat = _latMin;
            }
            else if (key == SETUP_KEY_LAT_MAX)
            {
                if (!getSetupData(&key, &line, NULL, &_latMax))
                    return false;
                lat = _latMax;
            }
            else if (key == SETUP_KEY_LON_MIN)
            {
                if (!getSetupData(&key, &line, NULL, &_lonMin))
                    return false;
                lon = _lonMin;
            }
            else if (key == SETUP_KEY_LON_MAX)
            {
                if (!getSetupData(&key, &line, NULL, &_lonMax))
                    return false;
                lon = _lonMax;
            }
            else if (key == SETUP_KEY_LATLON_JAPAN)
            {
                if (!getSetupBoolData(&key, &line, &_latLonJapan))
                    return false;
            }
            else if (key == SETUP_KEY_MOD_MERGE_LEN)
            {
                if (!getSetupData(&key, &line, NULL, &_modMergeLen))
                    return false;
            }
            else if (key == SETUP_KEY_MOD_MERGE_FIND)
            {
                if (!getSetupData(&key, &line, &_modMergeFind))
                    return false;
                if (_modMergeFind < 1)
                {
                    cerr << "invalid setup file " << key
                         << " data " << _modMergeFind << endl;
                    return false;
                }
            }
            else if (key == SETUP_KEY_MOD_FORCE_ONE_LANE)
            {
                if (!getSetupData(&key, &line, &_modForceOneLane))
                    return false;
            }
            else
            {
                int i;
                for (i = 0; i < APP_OSM_RT_NUM; i++)
                {
                    if (key == _osmRT_SetupKey[i])
                    {
                        if (!getSetupData(&key, &line, &_osmRT_Lane[i]))
                            return false;
                        if (_osmRT_Lane[i] < 0)
                        {
                            cerr << "invalid setup file " << key
                                 << " data " << _osmRT_Lane[i] << endl;
                            return false;
                        }
                        break;
                    }
                }
                if (i == APP_OSM_RT_NUM)
                {
                    cerr << "invalid setup file key " << key << endl;
                    return false;
                }
            }
        }
        ifs.close();
    }

    if (fileType != SETUP_VAL_FILETYPE)
    {
        cerr << "invalid setup file format key " << SETUP_KEY_FILETYPE << endl;
        return false;
    }
    if (!_osm && !_modify)
    {
        cerr << "no setup file function "
             << SETUP_KEY_OSM << " " << SETUP_KEY_MODIFY << endl;
        return false;
    }
    if (_inputOSM_File == "" && _osm)
    {
        cerr << "no setup file key " << SETUP_KEY_INPUT_OSM_FILE << endl;
        return false;
    }
    if (_inputMatesDir == "" && !_osm)
    {
        cerr << "no setup file key " << SETUP_KEY_INPUT_MATES_DIR << endl;
        return false;
    }
    if (_outputDir == "")
    {
        cerr << "no setup file key " << SETUP_KEY_OUTPUT_DIR << endl;
        return false;
    }
    if (_inputMatesDir == _outputDir)
    {
        cerr << "same input output error \"" << _inputMatesDir << "\"" << endl;
        return false;
    }

    //日本測地系から世界測地系へ変換、内部は常に世界
    if (_latLonJapan)
    {
        if (lat == APP_LATLON_MAX || lon == APP_LATLON_MAX)
        {
            cerr << "japan geodetic conversion error, need latitude and longitude" << endl;
            return false;
        }

        double latTemp = lat;
        double lonTemp = lon;
        if (_latMin != -APP_LATLON_MAX)
            latLonJapanToWorld(&_latMin, &lon);
        if (_latMax !=  APP_LATLON_MAX)
            latLonJapanToWorld(&_latMax, &lonTemp);
        if (_lonMin != -APP_LATLON_MAX)
            latLonJapanToWorld(&lat,     &_lonMin);
        if (_lonMax !=  APP_LATLON_MAX)
            latLonJapanToWorld(&latTemp, &_lonMax);
//      printf("%.6f %.6f %.6f %.6f\n", _latMin, _lonMin, _latMax, _lonMax);
    }

    cout << endl;
    return true;
}

bool App::inLatLonMinMax(double lat, double lon, bool* border)
{
    if (!_setLatLonMid)  //最初だけ設定、後で正式の値
        setLatLonMid(lat, lon);

    if (lat >= _latMin - _latBorder && lon >= _lonMin - _lonBorder &&
        lat <= _latMax + _latBorder && lon <= _lonMax + _lonBorder)
    {
        if (lat < _latMin || lon < _lonMin ||
            lat > _latMax || lon > _lonMax)
            *border = true;
        else
            *border = false;
        return true;
    }
    else {
        *border = false;
        return false;
    }
}

bool App::inLatLonMinMaxArea(double latMin, double latMax,
                             double lonMin, double lonMax)
{
    if (!_setLatLonMid)  //最初だけ設定、後で正式の値
        setLatLonMid((latMin + latMax) / 2, (lonMin + lonMax) / 2);

//  qDebug("area %d %d %d %d",
//         _latMin - _latBorder <= latMax, latMin <= _latMax + _latBorder,
//         _lonMin - _lonBorder <= lonMax, lonMin <= _lonMax + _lonBorder);
    if (_latMin - _latBorder <= latMax && latMin <= _latMax + _latBorder &&
        _lonMin - _lonBorder <= lonMax && lonMin <= _lonMax + _lonBorder) {
//      qDebug("true");
        return true;
    }
    else {
//      qDebug("false");
        return false;
    }
}

void App::setLatLonMid(double latMid, double lonMid)
{
    _setLatLonMid = true;
    _latMid = latMid;
    _lonMid = lonMid;
    double sinLat = sin(_latMid * M_PI / 180);
    double weight = sqrt(1 - 0.00669438002301188 * sinLat * sinLat);
    _latToY = 6335439.32708317 / weight / weight / weight / 180 * M_PI;
    _lonToX = 6378137 / weight * cos(_latMid * M_PI / 180) / 180 * M_PI;

    _latBorder = 500 / _latToY;
    _lonBorder = 500 / _lonToX;

//  cout << " _latMid " << _latMid << " _lonMid = " << _lonMid << endl;
//  cout << " _latToY " << _latToY << " _lonToX = " << _lonToX << endl;
//  cout << " _latBorder " << _latBorder << " _lonBorder = " << _lonBorder << endl;

/*
    _yToLat = weight * weight * weight / 6335439.32708317 * 180 / M_PI;
    _xToLon = weight / 6378137 / cos(_lat * M_PI / 180) * 180 / M_PI;
*/
}

double App::latToY(double lat)
{
    return ((lat - _latMid) * _latToY);
}

double App::lonToX(double lon)
{
    return ((lon - _lonMid) * _lonToX);
}

void App::latLonJapanToWorld(double* lat, double* lon)
{
    double latTemp = *lat;
    double lonTemp = *lon;
    *lat = latTemp - latTemp * 0.00010695  + lonTemp * 0.000017464 + 0.0046017;
    *lon = lonTemp - latTemp * 0.000046038 - lonTemp * 0.000083043 + 0.010040;
/*
    //逆処理
    *lat = latTemp + latTemp * 0.00010696  - lonTemp * 0.000017467 - 0.0046020;
    *lon = lonTemp + latTemp * 0.000046047 + lonTemp * 0.000083049 - 0.010041;
*/
}

bool App::getSetupBoolData(string* key, stringstream* line, bool* boolData)
{
    string stringValue;

    if (!getSetupData(key, line, NULL, NULL, &stringValue))
        return false;
    if      (stringValue == SETUP_VAL_TRUE)
        *boolData = true;
    else if (stringValue == SETUP_VAL_FALSE)
        *boolData = false;
    else
    {
        cerr << "invalid setup file " << *key
             << " data \"" << stringValue << "\"" << endl;
        return false;
    }
    return true;
}

bool App::getSetupData(string* key, stringstream* line, int* intData,
                       double* doubleData, string* stringData)
{
    char tempChar;
    string tempStr;

    int pos = (int)line->tellg();
    if      (intData    != NULL)
    {
        *intData = 0;
        *line >> *intData;
        cout << "  " << *key << " " << *intData << endl;
    }
    else if (doubleData != NULL)
    {
        *doubleData = 0;
        *line >> *doubleData;
        printf("  %s %5.10f\n", key->c_str(), *doubleData);
    }
    else if (stringData != NULL)
    {
        *stringData = "";
        tempChar = ' ';
        *line >> tempChar;
//      cout << tempChar << endl;
        if (tempChar == '\"')
        {
            while (line->get(tempChar))
            {
                if (tempChar == '\"')
                    break;
                *stringData += tempChar;
//              cout << tempChar << " " << line->tellg() << endl;
            }
            if (line->tellg() < 0)
            {
                cerr << "invalid setup file " << *key << " \"...\"" << endl;
                return false;
            }
        }
        else
        {
            tempStr = "";
            *line >> tempStr;
            *stringData = tempChar + tempStr;
        }
        cout << "  " << *key << " \"" << *stringData << "\"" << endl;
    }
    // cout << pos << " " << line->tellg() << endl;
    /*
    if ( (int)line->tellg() < 0 || (int)line->tellg() - pos <= 0) {
        cerr << "no setup file " << *key << " data" << endl;
        return false;
    }
    */
    pos = (int)line->tellg();
    tempStr = "";
    *line >> tempStr;
//  cout << pos << " " << line->tellg() << endl;
    if ((int)line->tellg() >= 0 && (int)line->tellg() - pos > 0)
    {
        cerr << "too much setup file " << *key << " data " << tempStr << endl;
        return false;
    }

    return true;
}

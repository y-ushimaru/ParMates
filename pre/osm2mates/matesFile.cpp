/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
// MATES ファイルアクセス
//マップエディタの処理を元に作成

#include <Qt>
#include <QDir>
#include <QFileInfo>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "road.h"
#include "matesFile.h"
#include <AmuStringOperator.h>

using namespace std;

#define MAPPOSFILE      "/mapPosition.txt"              //ファイル／ディレクトリ名
#define NETWORKFILE     "/network.txt"
#define ERRORFILE       "/osm2matesError.txt"

#define NETWORK_HEADER  2                               //ネットワークファイルヘッダ

#define POS_MAX         1000000000                      //最大位置
#define POS_MIN         -1000000000                     //最小位置

//コンストラクタ
MatesFile::MatesFile()
{
}

//読み込み前処理
void MatesFile::prevRead()
{
    cout << "read map position file \"" << *app.inputMatesDir()
         << MAPPOSFILE << "\"" << endl;
    cout << "read network file \"" << *app.inputMatesDir()
         << NETWORKFILE << "\"" << endl;
}

//書き込み前処理
void MatesFile::prevWrite()
{
    //時間が掛かる可能性があるので先にやる、ファイル名は読み込み時に変わるのでローカル保持

    bool          overwrite;
    stringstream  message;

    string path = *app.outputDir();
    string mapPosFile = path + MAPPOSFILE;
    string networkFile = path + NETWORKFILE;
    string errorFile = path + ERRORFILE;

    cout << "write map position file \"" << mapPosFile << "\"" << endl;
    cout << "write network file \"" << networkFile << "\"" << endl;
    cout << "write error file \"" << errorFile << "\"" << endl;

    overwrite = false;
    message << endl << "overwrite files" << endl;
    if (checkFile(mapPosFile.c_str()))
    {
        overwrite = true;
        message << "  " << mapPosFile << endl;
    }
    if (checkFile(networkFile.c_str()))
    {
        overwrite = true;
        message << "  " << networkFile << endl;
    }
    if (checkFile(errorFile.c_str()))
    {
        overwrite = true;
        message << "  " << errorFile << endl;
    }
    if (overwrite)
        cout << message.str();
}

//読み込み
bool MatesFile::read()
{
    string path = *app.inputMatesDir();
    _mapPosFile = path + MAPPOSFILE;
    _networkFile = path + NETWORKFILE;

    //ファイルチェック
    if (!checkFile(_mapPosFile.c_str()))
    {
        cerr << "no map potision file" << endl << _mapPosFile << endl;
        return false;
    }
    if (!checkFile(_networkFile.c_str()))
    {
        cerr << "no network file" << endl << _networkFile << endl;
        return false;
    }

    //マップ位置ファイル読み込み
    if (!readMapPos())
        return false;

    //ネットワークファイル読み込み
    if (!readNetwork())
        return false;

    return true;
}

//書き込み
bool MatesFile::write()
{
    //読み込みでファイル名が入れ替わる可能性があるので必ず変更
    string path = *app.outputDir();
    _mapPosFile = path + MAPPOSFILE;
    _networkFile = path + NETWORKFILE;
    _errorFile = path + ERRORFILE;

    //マップ位置ファイル書き込み
    if (!writeMapPos())
        return false;

    //ネットワークファイル書き込み
    if (!writeNetwork())
        return false;

    //エラーファイル書き込み
    if (!writeError())
        return false;

    return true;
}

//ファイルチェック
bool MatesFile::checkFile(const char* file)
{
    QFileInfo   fileInfo;

//  qDebug("File::init checkFile '%s'", pFile);
    fileInfo.setFile(file);
    return fileInfo.isFile();
}

//マップ位置ファイル読み込み、MATES 本体の RoadMapBuilder から流用
bool MatesFile::readMapPos()
{
    ifstream            ifs;
    string              lineString;
    int                 lineNum, id;
    double              x, y, z;
    vector<string>      tokens;
    Intsec*             intsec;
    bool                okX, okY, okZ;

    cout << "read map position file..." << endl;

    //ファイルを読み込む
    ifs.open(_mapPosFile.c_str(), ios::in);
    if (!ifs)
    {
        cerr << "map position file open error" << endl << _mapPosFile << endl;
        return false;
    }

    //交差点の座標をセット、一応 double でも可能だが丸める
    lineNum = 0;
    while (ifs.good())
    {
        lineNum++;
        getline(ifs, lineString);
        AmuStringOperator::getAdjustString(&lineString);
        if (!lineString.empty())
        {
            AmuStringOperator::getTokens(&tokens, lineString, ',');
            if (tokens.size() != 3 && tokens.size() != 4)
            {
                cerr << "map position file format error line " << lineNum << endl;
                return false;
            }
            id = intsecId(tokens[0].c_str());
            if (id == MAP_NOID) {
                cerr << "map position file intersection id error line " << lineNum
                     << " id " << tokens[0] << endl;
                return false;
            }
            x = pos(tokens[1].c_str(), &okX);
            y = pos(tokens[2].c_str(), &okY);
            if (tokens.size() == 4)            // z 座標はオプション
                z = pos(tokens[3].c_str(), &okZ);
            else
                z = pos("0", &okZ);
            if (!okX || !okY || !okZ)
            {
                cerr << "map position file position error line " << lineNum << endl;
                return false;
            }

            //値をセットする、ID重複不可
            if (app.map()->findIntsec(id) != NULL)
            {
                cerr << "map position file duplicate intersection id error line "
                     << lineNum << " id " << id << endl;
                return false;
            }
            intsec = app.map()->createIntsec(id);
            Q_ASSERT(intsec != NULL);
            intsec->setMapPos(x, y, z);
        }
    }

    return true;
}

//ネットワークファイル読み込み、MATES 本体の RoadMapBuilder から流用
bool MatesFile::readNetwork()
{
    Map*                map = app.map();
    ifstream            ifs;
    string              lineString, lanes;
    int                 lineNum, token, id, idCon, laneOut, laneIn, intsecType, intsecTypeCon;
    vector<string>      tokens;
    Intsec*             intsec;
    Intsec*             intsecCon;
    Road*               road;
    MAP_ROADMAP_IT      rmi;

    cout << "read network file..." << endl;

    //ファイルを読み込む
    ifs.open(_networkFile.c_str(), ios::in);
    if (!ifs) {
        cerr << "network file open error" << endl << _networkFile << endl;
        return false;
    }

    //交差点の接続情報をセット、逆方向の車線には NOLANE を入れてエラーチェック
    lineNum = 0;
    while (ifs.good())
    {
        lineNum++;
        getline(ifs, lineString);
        AmuStringOperator::getAdjustString(&lineString);
        if (!lineString.empty())
        {
            AmuStringOperator::getTokens(&tokens, lineString, ',');
            if (tokens.size() < 3)
            {
                cerr << "network file lack of data error line " << lineNum << endl;
                return false;
            }
            id = intsecId(tokens[0].c_str());
            if (id == MAP_NOID)
            {
                cerr << "network file intersection id error line " << lineNum
                     << " id " << tokens[0] << endl;
                return false;
            }
            intsec = map->findIntsec(id);
            if (intsec == NULL)
            {
                cerr << "network file intersection id find error for map position line "
                     << lineNum << " id " << id << endl;
                return false;
            }
            lanes = tokens[1];
            if ((tokens.size() - NETWORK_HEADER) * 2 != lanes.size())
            {
                cerr << "network file lane count mismatch error line " << lineNum << endl;
                return false;
            }

            //接続交差点のIDをセットする
            for (token = NETWORK_HEADER; token < (int)tokens.size(); token++)
            {
                idCon = intsecId(tokens[token].c_str());
                if (idCon == MAP_NOID)
                {
                    cerr << "network file connect intersection id error line " << lineNum
                         << " id " << tokens[token] << endl;
                    return false;
                }
                if (idCon == id)
                {
                    cerr << "network file same connect intersection id error line " << lineNum
                         << " id " << idCon << endl;
                    return false;
                }
                intsecCon = map->findIntsec(idCon);
                if (intsecCon == NULL)
                {
                    cerr << "network file same connect intersection id error line " << lineNum
                         << " id " << idCon << endl;
                    return false;
                }
                road = map->createRoad(intsec, intsecCon, 0);
                laneOut = lane(lanes.at((token - NETWORK_HEADER) * 2));
                laneIn  = lane(lanes.at((token - NETWORK_HEADER) * 2 + 1));
                if (laneOut == -1 || laneIn == -1)
                {
                    cerr << "network file lane count errore line " << lineNum
                         << " lanes " << lanes << endl;
                    return false;
                }
                if (token == NETWORK_HEADER)
                    intsec->setFirstIdCon(idCon);  //反対側からの接続による設定を上書き

                intsecType = road->intsecType(id);
                Q_ASSERT(intsecType != ROAD_IT_NONE);

                //マップエディタと違い、単路重複は同一単路化
                road->addLane(intsecType, ROAD_LT_OUT, laneOut);
                road->addLane(intsecType, ROAD_LT_IN,  laneIn);
            }
        }
    }

    //一方しかない車線チェック
    road = map->nextRoad(&rmi, true);
    while (road != NULL)
    {
        for (intsecType = 0; intsecType < ROAD_IT_NUM; intsecType++)
        {
            if (road->laneFlag(intsecType, ROAD_LT_OUT) == false)
            {
                if (intsecType == ROAD_IT_SMALL)
                    intsecTypeCon = ROAD_IT_LARGE;
                else
                    intsecTypeCon = ROAD_IT_SMALL;
                cerr << "network file onside lane errore intersection id "
                     << road->intsec(intsecType)->id() << " to "
                     << road->intsec(intsecTypeCon)->id() << endl;
                return false;
            }
        }
        road = map->nextRoad(&rmi);
    }

    return true;
}

//交差点ID取得とチェック、エラーなら MAP_NOID
int MatesFile::intsecId(const char* id)
{
    QString     idString;
    int         idInt;
    bool        ok;

    idString = id;
    idInt = idString.toInt(&ok, 10);
    if (!ok)
        return MAP_NOID;
    if (idInt < MAP_INTSEC_ID_MIN || idInt > MAP_INTSEC_ID_MAX)
       return MAP_NOID;
    return idInt;
}

//位置取得とチェック、エラーなら Ok が false、データを補正
double MatesFile::pos(const char* posStr, bool* ok)
{
    QString     numString;
    double      pos;

    numString = posStr;
    pos = numString.toDouble(ok);
    if (!*ok)
        return 0;
    else
    {
        if      (pos < POS_MIN)
        {
            *ok = false;
            return POS_MIN;
        }
        else if (pos > POS_MAX)
        {
            *ok = false;
            return POS_MAX;
        }
    }
    return pos;
}

//車線数種得、エラーなら -1
int MatesFile::lane(const char lane)
{
    if (lane < '0' || lane > '9')
        return -1;
    else
        return (int)(lane - '0');
}

//マップ位置ファイル書き込み
bool MatesFile::writeMapPos()
{
    Map*                map = app.map();
    ofstream            ofs;
    MAP_INTSECMAP_IT    imi;
    Intsec*             intsec;

    cout << "write map position file..." << endl;

    ofs.open(_mapPosFile.c_str(), ios::out);
    if (!ofs)
    {
        cerr << "map position file open error" << endl << _mapPosFile << endl;
        return false;
    }

    intsec = map->nextIntsec(&imi, true);
    while (intsec != NULL)
    {
        int id = map->getIntsecFinalId(intsec->id());
        if (id > MAP_INTSEC_ID_MAX)
        {
            cerr << "too much intersection" << endl;
            return false;
        }
        ofs << id << ","
            << (int)intsec->mapPosX() << ","   //出力時に int 化、マニュアル等にあわせる
            << (int)intsec->mapPosY() << ","
            << (int)intsec->mapPosZ() << '\n';
        intsec = map->nextIntsec(&imi);
    }

    ofs.close();
    return true;
}

//ネットワークファイル書き込み
bool MatesFile::writeNetwork()
{
    Map*                map = app.map();
    stringstream        lanes, ids;
    ofstream            ofs;
    MAP_INTSECMAP_IT    imi;
    Intsec*             intsec;
    INTSEC_ANGLEMAP_IT  iami;
    Road*               road;
    int                 idCon, intsecType;

    cout << "write network file..." << endl;

    ofs.open(_networkFile.c_str(), ios::out);
    if (!ofs) {
        cerr << "network file open error" << endl << _networkFile << endl;
        return false;
    }

    intsec = map->nextIntsec(&imi, true);
    while (intsec != NULL)
    {
        lanes.str("");
        ids.str("");
        road = intsec->nextRoadByAngle(&iami, true, &idCon);
        if (road != NULL)
        {
            while (road != NULL)
            {
                intsecType = road->intsecType(intsec->id());
                Q_ASSERT(intsecType != ROAD_IT_NONE);
                lanes << road->lane(intsecType, ROAD_LT_OUT) << road->lane(intsecType, ROAD_LT_IN);
                ids << "," << map->getIntsecFinalId(idCon);
                road = intsec->nextRoadByAngle(&iami, false, &idCon);
            }
//          qDebug("MatesFile::writeNetwork id=%d lanes='%s' ids='%s'",
//                 pIntsec->id(), lanes.str().c_str(), ids.str().c_str());
            ofs << map->getIntsecFinalId(intsec->id()) << "," << lanes.str() << ids.str() << '\n';
        }
        intsec = map->nextIntsec(&imi);
    }

    ofs.close();
    return true;
}

//エラーファイル書き込み
bool MatesFile::writeError()
{
    Map*                map = app.map();
    ofstream            ofs;
    MAP_INTSECMAP_IT    imi;
    Intsec*             intsec;
    MAP_ROADMAP_IT      rmi;
    Road*               road;
    string              messagePart;
    vector<int>         errorCnt;
    vector<string>      errorStr;
    int                 errorIntsec, errorRoad;

    cout << "write error file..." << endl;

    errorCnt.resize(APP_ERROR_ID_NUM, 0);
    errorStr.resize(APP_ERROR_ID_NUM, "");
    errorIntsec = errorRoad = 0;

    ofs.open(_errorFile.c_str(), ios::out);
    if (!ofs) {
        cerr << "error file open error" << endl << _errorFile << endl;
        return false;
    }

    intsec = map->nextIntsec(&imi, true);
    while (intsec != NULL)
    {
        if (intsec->checkError())
        {
            intsec->getErrorMessages(&messagePart, &errorCnt, &errorStr);
            ofs << "intersection id " << map->getIntsecFinalId(intsec->id())
                << " road num " << intsec->roadNum() << '\n' << messagePart << '\n';
            errorIntsec++;
        }
        intsec = map->nextIntsec(&imi);
    }
    road = map->nextRoad(&rmi, true);
    while (road != NULL)
    {
        if (road->checkError())
        {
            road->getErrorMessages(&messagePart, &errorCnt, &errorStr);
            ofs << "road intersection id "
                << map->getIntsecFinalId(road->intsec(ROAD_IT_SMALL)->id()) << '-'
                << map->getIntsecFinalId(road->intsec(ROAD_IT_LARGE)->id()) << '\n'
                << messagePart << '\n';
            errorRoad++;
        }
        road = map->nextRoad(&rmi);
    }

    ofs << endl << "summary intersection" << endl;
    for (int i = 0; i < APP_ERROR_ID_ROAD - 1; i++)
    {
        if (errorCnt[i] != 0)
        {
            ofs << "  " << errorCnt[i] << " - " << errorStr[i] << endl;
        }
    }
    ofs << endl << "summary road" << endl;
    for (int i = APP_ERROR_ID_ROAD; i < APP_ERROR_ID_NUM; i++)
    {
        if (errorCnt[i] != 0)
        {
            ofs << "  " << errorCnt[i] << " - " << errorStr[i] << endl;
        }
    }

    ofs << endl << "summary all intersection " << errorIntsec
        << " road " << errorRoad << endl;

    ofs.close();
    return true;
}

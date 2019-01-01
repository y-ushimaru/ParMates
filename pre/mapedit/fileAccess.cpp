/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//ファイルアクセス

//ファイル I/O 時のみ使うクラス、コンストラクタに設定ファイル名を入れる

#include <QtGui>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include "app.h"
#include "mapframe.h"
#include "map.h"
#include "intsec.h"
#include "road.h"
#include "fileAccess.h"
#include <AmuStringOperator.h>

using namespace std;

#define TITLE                           "ファイルアクセス"              //タイトル
#define SETUP_KEY_FILETYPE              "MATES_INPUTEDITOR_SETUP"       //設定ファイルキー
#define SETUP_KEY_VIEWMAP               "VIEWMAP"
#define SETUP_KEY_VIEWMAGNI             "VIEWMAGNI"
#define SETUP_KEY_CLEARVIEW             "CLEARVIEW"
#define SETUP_KEY_SHOWID                "SHOWID"
#define SETUP_KEY_BACKIMAGE             "BACKIMAGE"
#define SETUP_KEY_BACKFILE              "BACKFILE"
#define SETUP_KEY_BACKMAP               "BACKMAP"
#define SETUP_KEY_BACKMAGNI             "BACKMAGNI"
#define SETUP_VAL_FILETYPE              "1.0"                           //設定ファイル値
#define READBUFFSIZE                    	1024                            //読み込みバッファサイズ
#define NETWORK_HEADER                  2                               //ネットワークファイルヘッダ
#define NOLANE                          -1                              //車線数なし

//コンストラクタ
FileAccess::FileAccess()
{
}

//初期処理
void FileAccess::init(QWidget* pParent, const char* pSetupFile)
{
    QFileInfo   fileInfo;
    string      path;

    _pParent = pParent;
    _setupFile = pSetupFile;
    fileInfo.setFile(pSetupFile);
    path = fileInfo.absolutePath().toStdString() + "/";
    _mapPosFile = path + APP_MAPPOSFILE;
    _networkFile = path + APP_NETWORKFILE;
    _errorFile = path + APP_ERRORFILE;
//  qDebug("File::init setup '%s'", _setupFile.c_str());
//  qDebug("File::init map position '%s'", _mapPosFile.c_str());
//  qDebug("File::init network '%s'", _networkFile.c_str());
}

//読み込み
bool FileAccess::read(bool* pCacncel, bool noSetupFile)
{
    stringstream        message;
    ifstream            ifs;
    stringstream        line;
    double              doubleValue1, doubleValue2, magniX, magniY;
    string              stringValue1, stringValue2;
    bool                boolValue, ok1, ok2;
    char                readBuff[READBUFFSIZE];
    int                 maxX, maxY, minX, minY;
    Map*                pMap = app.getMap();
    MapFrame*           pMapFrame = app.getMapFrame();
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;

    //ファイルチェック、キャンセル
    *pCacncel = true;
    if (!noSetupFile)
    {
        if (!checkFile(_setupFile.c_str())) {
            message << "設定ファイルがありません。\n" << _setupFile;
            return errorMessage(message.str().c_str());
        }
    }
    if (!checkFile(_mapPosFile.c_str())) {
        message << "マップ位置ファイルがありません。\n" << _mapPosFile;
        return errorMessage(message.str().c_str());
    }
    if (!checkFile(_networkFile.c_str())) {
        message << "ネットワークファイルがありません。\n" << _networkFile;
        return errorMessage(message.str().c_str());
    }

    //データクリア、以下キャンセル不可
    app.getMap()->deleteAll();
    app.setDefault();
    *pCacncel = false;

    //設定ファイル読み込み、位置は制限なし、不正なら補正
    if (!noSetupFile)
    {
        string key;
        ifs.open(_setupFile.c_str(), ios::in);
        if (!ifs) {
            message << "設定ファイルを開けません。\n" << _setupFile;
            return errorMessage(message.str().c_str());
        }
        ifs >> key >> stringValue1;
        if (key != SETUP_KEY_FILETYPE || stringValue1 != SETUP_VAL_FILETYPE) {
            message << "設定ファイルの形式が不正です。\n" << _setupFile;
            return errorMessage(message.str().c_str());
        }
        while(ifs.getline(readBuff, sizeof(readBuff)))
        {
            line.str("");
            line.clear();
            line << readBuff;
            key = "";
            line >> key;
//          qDebug("FileAccess::read '%s' key '%s'", line.str().c_str(), key.c_str());
            if     (key == SETUP_KEY_VIEWMAP)
            {
                line >> stringValue1 >> stringValue2;
                doubleValue1 = getPos(stringValue1.c_str(), &ok1);
                doubleValue2 = getPos(stringValue2.c_str(), &ok2);
                app.setViewMap(doubleValue1, doubleValue2);
            }
            else if (key == SETUP_KEY_VIEWMAGNI)
            {
                line >> doubleValue1;
                if      (doubleValue1 < APP_MAGNI_MIN)
                    doubleValue1 = APP_MAGNI_MIN;
                else if (doubleValue1 > APP_MAGNI_MAX)
                    doubleValue1 = APP_MAGNI_MAX;
                app.setViewMagni(doubleValue1);
            }
            else if (key == SETUP_KEY_CLEARVIEW)
            {
                line >> boolValue;
                app.setClearVivew(boolValue);
            }
            else if (key == SETUP_KEY_SHOWID)
            {
                line >> boolValue;
                app.setShowId(boolValue);
            }
            else if (key == SETUP_KEY_BACKIMAGE)
            {
                line >> boolValue;
                app.setBackImage(boolValue);
            }
            else if (key == SETUP_KEY_BACKFILE)
            {
                if (!getQuoteString(line.str().c_str(), key.size(), &stringValue1))
                    stringValue1 = "";
                app.setBackFile(stringValue1.c_str());
            }
            else if (key == SETUP_KEY_BACKMAP)
            {
                line >> stringValue1 >> stringValue2;
                doubleValue1 = getPos(stringValue1.c_str(), &ok1);
                doubleValue2 = getPos(stringValue2.c_str(), &ok2);
                app.setBackMap(doubleValue1, doubleValue2);
            }
            else if (key == SETUP_KEY_BACKMAGNI)
            {
                line >> doubleValue1;
                if      (doubleValue1 < APP_MAGNI_MIN)
                    doubleValue1 = APP_MAGNI_MIN;
                else if (doubleValue1 > APP_MAGNI_MAX)
                    doubleValue1 = APP_MAGNI_MAX;
                app.setBackMagni(doubleValue1);
            }
        }
        ifs.close();
    }

    //マップ位置ファイル読み込み
    if (!readMapPos())
        return false;

    //ネットワークファイル読み込み
    if (!readNetwork())
        return false;

    //設定ファイルなしなら画面位置補正
    if (noSetupFile)
    {
        maxX = maxY = minX = minY = 0;
        pIntsec = pMap->nextIntsec(&imi, true);
        while (pIntsec != NULL)
        {
            if (pIntsec->getMapPosX() > maxX)
                maxX = pIntsec->getMapPosX();
            if (pIntsec->getMapPosY() > maxY)
                maxY = pIntsec->getMapPosY();
            if (pIntsec->getMapPosX() < minX)
                minX = pIntsec->getMapPosX();
            if (pIntsec->getMapPosY() < minY)
                minY = pIntsec->getMapPosY();
            pIntsec = pMap->nextIntsec(&imi);
        }
//      qDebug("FileAccess::read max(%d,%d) min(%d,%d)", maxX, maxY, minX, minY);
        magniX = (double)pMapFrame->width()  / (maxX - minX);
        magniY = (double)pMapFrame->height() / (maxY - minY);
        if (magniX < magniY)
        {
            app.setViewMagni(magniX);
            app.setViewMap(minX, minY - (pMapFrame->height() / magniX - (maxY - minY)) / 2);
        }
        else
        {
            app.setViewMagni(magniY);
            app.setViewMap(minX - (pMapFrame->width() / magniY - (maxX - minX)) / 2, minY);
        }
   }
//  app.getMap()->debugTrace();

    return true;
}

//書き込み
bool FileAccess::write()
{
    bool            error, overwrite;
    stringstream    message;
    ofstream        ofs;
    string          backFile;

    //エラーチェック
    error = checkError();
    if (error)
    {
        message << "データに問題がありますが保存しますか？\n問題の詳細はエラーファイルに保存されます。\n"
                << _errorFile;
        if (QMessageBox::question(_pParent, _pParent->tr(TITLE), _pParent->tr(message.str().c_str()),
                                  QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return false;
    }
    message.str("");

    //ファイルチェック
    overwrite = false;
    message << "既存のファイルを上書きしてよろしいですか？";
    if (checkFile(_setupFile.c_str()))
    {
        overwrite = true;
        message << "\n" << _setupFile;
    }
    if (checkFile(_mapPosFile.c_str()))
    {
        overwrite = true;
        message << "\n" << _mapPosFile;
    }
    if (checkFile(_networkFile.c_str()))
    {
        overwrite = true;
        message << "\n" << _networkFile;
    }
    if (error)
    {
        if (checkFile(_errorFile.c_str()))
        {
            overwrite = true;
            message << "\n" << _errorFile;
        }
    }
    if (overwrite)
    {
        if (QMessageBox::question(_pParent, _pParent->tr(TITLE), _pParent->tr(message.str().c_str()),
                                  QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return false;
    }
    message.str("");

    //設定ファイル書き込み
    ofs.open(_setupFile.c_str(), ios::out);
    if (!ofs) {
        message << "設定ファイルを開けません。\n" << _setupFile;
        return errorMessage(message.str().c_str());
    }
    ofs << SETUP_KEY_FILETYPE << ' ' << SETUP_VAL_FILETYPE << '\n';
    ofs << SETUP_KEY_VIEWMAP << ' ' << app.getViewMapX() << ' ' << app.getViewMapY() << '\n';
    ofs << SETUP_KEY_VIEWMAGNI << ' ' << app.getViewMagni() << '\n';
    ofs << SETUP_KEY_CLEARVIEW << ' ' << app.getClearView() << '\n';
    ofs << SETUP_KEY_SHOWID << ' ' << app.getShowId() << '\n';
    ofs << SETUP_KEY_BACKIMAGE << ' ' << app.getBackImage() << '\n';
    app.getBackFile(&backFile);
    ofs << SETUP_KEY_BACKFILE << " \"" << backFile << "\"\n";
    ofs << SETUP_KEY_BACKMAP << ' ' << app.getBackMapX() << ' ' << app.getBackMapY() << '\n';
    ofs << SETUP_KEY_BACKMAGNI << ' ' << app.getBackMagni() << '\n';

    //マップ位置ファイル書き込み
    if (!writeMapPos())
        return false;

    //ネットワークファイル書き込み
    if (!writeNetwork())
        return false;

    //エラーファイル書き込み
    if (error)
    {
        if (!writeError())
            return false;
    }

    return true;
}

//ファイルチェック
bool FileAccess::checkFile(const char* pFile)
{
    QFileInfo   fileInfo;

//  qDebug("File::init checkFile '%s'", pFile);
    fileInfo.setFile(pFile);
    return fileInfo.isFile();
}

//マップ位置ファイル読み込み、MATES 本体の RoadMapBuilder から流用
bool FileAccess::readMapPos()
{
    stringstream        message;
    ifstream            ifs;
    string              lineString;
    int                 lineNum, id, x, y, z;
    vector<string>      tokens;
    Intsec*             pIntsec;
    bool                okX, okY, okZ;

    //ファイルを読み込む
    ifs.open(_mapPosFile.c_str(), ios::in);
    if (!ifs) {
        message << "マップ位置ファイルを開けません。\n" << _mapPosFile;
        return errorMessage(message.str().c_str());
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
            if (tokens.size() != 3 && tokens.size() != 4) {
                message << "マップ位置ファイルのデータ形式が不正です。\n" << _mapPosFile << "\nLine:" << lineNum;
                return errorMessage(message.str().c_str());
            }
            id = getIntsecId(tokens[0].c_str());
            if (id == MAP_NOID) {
                message << "マップ位置ファイルの交差点IDが不正です。\n" << _mapPosFile
                       << "\nLine:" << lineNum << " ID:" << tokens[0];
                return errorMessage(message.str().c_str());
            }
            x = getPos(tokens[1].c_str(), &okX);
            y = getPos(tokens[2].c_str(), &okY);
            if (tokens.size() == 4)            // z 座標はオプション
                z = getPos(tokens[3].c_str(), &okZ);
            else
                z = getPos("0", &okZ);
            if (!okX || !okY || !okZ) {
                message << "マップ位置ファイルの座標が不正です。\n" << _mapPosFile << "\nLine:" << lineNum;
                return errorMessage(message.str().c_str());
            }

            //値をセットする、ID重複不可
            if (app.getMap()->getIntsec(id) != NULL) {
                message << "マップ位置ファイルの交差点IDが重複しています。\n" << _mapPosFile
                       << "\nLine:" << lineNum << " ID:" << id;
                return errorMessage(message.str().c_str());
            }
            pIntsec = app.getMap()->createIntsec(id);
            Q_ASSERT(pIntsec != NULL);
            pIntsec->setMapPos(x, y, z);
        }
    }

    return true;
}

//ネットワークファイル読み込み、MATES 本体の RoadMapBuilder から流用
bool FileAccess::readNetwork()
{
    Map*                pMap = app.getMap();
    stringstream        message;
    ifstream            ifs;
    string              lineString, lanes;
    int                 lineNum, token, id, idCon, laneOut, laneIn, intsecType, intsecTypeCon;
    vector<string>      tokens;
    Intsec*             pIntsec;
    Intsec*             pIntsecCon;
    Road*               pRoad;
    bool                already;
    MAP_ROADMAP_IT      rmi;

    //ファイルを読み込む
    ifs.open(_networkFile.c_str(), ios::in);
    if (!ifs) {
        message << "ネットワークファイルを開けません。\n" << _networkFile;
        return errorMessage(message.str().c_str());
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
            if (tokens.size() < 3) {
                message << "ネットワークファイルのデータが足りません。\n" << _networkFile << "\nLine:" << lineNum;
                return errorMessage(message.str().c_str());
            }
            id = getIntsecId(tokens[0].c_str());
            if (id == MAP_NOID) {
                message << "ネットワークファイルの交差点IDが不正です。\n" << _networkFile
                       << "\nLine:" << lineNum << " ID:" << tokens[0];
                return errorMessage(message.str().c_str());
            }
            pIntsec = pMap->getIntsec(id);
            if (pIntsec == NULL) {
                message << "ネットワークファイルの交差点IDがマップ位置ファイルにありません。\n" << _networkFile
                       << "\n" << _mapPosFile << "\nLine:" << lineNum << " ID:" << id;
                return errorMessage(message.str().c_str());
            }
            lanes = tokens[1];
            if ((tokens.size() - NETWORK_HEADER) * 2 != lanes.size()) {
                message << "ネットワークファイルの車線数と接続交差点数が一致しません。\n" << _networkFile
                       << "\nLine:" << lineNum;
                return errorMessage(message.str().c_str());
            }

            //接続交差点のIDをセットする
            for (token = NETWORK_HEADER; token < (int)tokens.size(); token++)
            {
                idCon = getIntsecId(tokens[token].c_str());
                if (idCon == MAP_NOID) {
                    message << "ネットワークファイルの接続交差点IDが不正です。\n" << _networkFile
                           << "\nLine:" << lineNum << " ID:" << tokens[token];
                    return errorMessage(message.str().c_str());
                }
                if (idCon == id) {
                    message << "ネットワークファイルの接続交差点IDが接続元と同一です。\n" << _networkFile
                           << "\nLine:" << lineNum << " ID:" << idCon;
                    return errorMessage(message.str().c_str());
                }
                pIntsecCon = pMap->getIntsec(idCon);
                if (pIntsecCon == NULL) {
                    message << "ネットワークファイルの接続交差点IDがマップ位置ファイルにありません。\n" << _networkFile
                           << "\nLine:" << lineNum << " ID:" << idCon;
                    return errorMessage(message.str().c_str());
                }
                pRoad = pMap->createRoad(pIntsec, pIntsecCon, &already);
                if (!already)
                {
                    pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_OUT, NOLANE);
                    pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_IN,  NOLANE);
                    pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_OUT, NOLANE);
                    pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_IN,  NOLANE);
                }
                laneOut = getLane(lanes.at((token - NETWORK_HEADER) * 2));
                laneIn  = getLane(lanes.at((token - NETWORK_HEADER) * 2 + 1));
                if (token == NETWORK_HEADER)
                    pIntsec->setFirstIntsecIdCon(idCon);  //反対側からの接続による設定を上書き、2010.08.10 浅川
                if (laneOut == NOLANE || laneIn == NOLANE) {
                    message << "ネットワークファイルの車線数が不正です。\n" << _networkFile
                           << "\nLine:" << lineNum << " Lanes:" << lanes;
                    return errorMessage(message.str().c_str());
                }
                intsecType = pRoad->getIntsecType(id);
                Q_ASSERT(intsecType != ROAD_IT_NONE);
                if (pRoad->getLane(intsecType, ROAD_LT_OUT) != NOLANE ||
                    pRoad->getLane(intsecType, ROAD_LT_IN)  != NOLANE) {
                    //MATES 本体では可能だが区別できないので受け付けない
                    message << "ネットワークファイルの単路が重複しています。\n" << _networkFile
                           << "\nLine:" << lineNum << " ID:" << id << " 接続先ID:" << idCon;
                    return errorMessage(message.str().c_str());
                }
                pRoad->setLane(intsecType, ROAD_LT_OUT, laneOut);
                pRoad->setLane(intsecType, ROAD_LT_IN,  laneIn);
            }
        }
    }

    //一方しかない車線チェック
    pRoad = pMap->nextRoad(&rmi, true);
    while (pRoad != NULL)
    {
        for (intsecType = 0; intsecType < ROAD_IT_NUM; intsecType++)
        {
            if (pRoad->getLane(intsecType, ROAD_LT_OUT) == NOLANE)
            {
                if (intsecType == ROAD_IT_SMALL)
                    intsecTypeCon = ROAD_IT_LARGE;
                else
                    intsecTypeCon = ROAD_IT_SMALL;
                message << "ネットワークファイルに片方の車線がありません。\n" << _networkFile
                       << "\nID:"      << pRoad->getIntsec(intsecType)->getIntsecId()
                       << " 接続先ID:" << pRoad->getIntsec(intsecTypeCon)->getIntsecId();
                return errorMessage(message.str().c_str());
            }
        }
        pRoad = pMap->nextRoad(&rmi);
    }

    return true;
}

//交差点ID取得とチェック、エラーなら MAP_NOID
int FileAccess::getIntsecId(const char* pIntsecId)
{
    QString     idString;
    int         id;
    bool        ok;

    idString = pIntsecId;
    id = idString.toInt(&ok, 10);
    if (!ok)
        return MAP_NOID;
    if (id < MAP_INTSEC_ID_MIN || id > MAP_INTSEC_ID_MAX)
       return MAP_NOID;
    return id;
}

//位置取得とチェック、エラーなら Ok が false、データを補正
double FileAccess::getPos(const char* pPos, bool* pOk)
{
    QString     numString;
    double      pos;

    numString = pPos;
    pos = numString.toDouble(pOk);
    if (!*pOk)
        return 0;
    else
    {
        if      (pos < APP_POS_MIN)
        {
            *pOk = false;
            return APP_POS_MIN;
        }
        else if (pos > APP_POS_MAX)
        {
            *pOk = false;
            return APP_POS_MAX;
        }
    }
    return pos;
}

//車線数種得、エラーなら NOLANE
int FileAccess::getLane(const char lane)
{
    if (lane < '0' || lane > '9')
        return NOLANE;
    else
        return (int)(lane - '0');
}

//マップ位置ファイル書き込み
bool FileAccess::writeMapPos()
{
    Map*                pMap = app.getMap();
    stringstream        message;
    ofstream            ofs;
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;

    ofs.open(_mapPosFile.c_str(), ios::out);
    if (!ofs) {
        message << "マップ位置ファイルを開けません。\n" << _mapPosFile;
        return errorMessage(message.str().c_str());
    }

    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        ofs << pIntsec->getIntsecId() << ","
            << pIntsec->getMapPosX() << ","<< pIntsec->getMapPosY() << ","<< pIntsec->getMapPosZ() << '\n';
        pIntsec = pMap->nextIntsec(&imi);
    }

    return true;
}

//ネットワークファイル書き込み
bool FileAccess::writeNetwork()
{
    Map*                pMap = app.getMap();
    stringstream        message, lanes, ids;
    ofstream            ofs;
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    INTSEC_ANGLEMAP_IT  iami;
    Road*               pRoad;
    int                 intsecIdCon, intsecType;

    ofs.open(_networkFile.c_str(), ios::out);
    if (!ofs) {
        message << "ネットワークファイルを開けません。\n" << _networkFile;
        return errorMessage(message.str().c_str());
    }

    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        lanes.str("");
        ids.str("");
        pRoad = pIntsec->nextRoadByAngle(&iami, true, &intsecIdCon);
        if (pRoad != NULL)
        {
            while (pRoad != NULL)
            {
                intsecType = pRoad->getIntsecType(pIntsec->getIntsecId());
                Q_ASSERT(intsecType != ROAD_IT_NONE);
                lanes << pRoad->getLane(intsecType, ROAD_LT_OUT) << pRoad->getLane(intsecType, ROAD_LT_IN);
                ids << "," << intsecIdCon;
                pRoad = pIntsec->nextRoadByAngle(&iami, false, &intsecIdCon);
            }
//          qDebug("FileAccess::writeNetwork id=%d lanes='%s' ids='%s'",
//                 pIntsec->getIntsecId(), lanes.str().c_str(), ids.str().c_str());
            ofs << pIntsec->getIntsecId() << "," << lanes.str() << ids.str() << '\n';
        }
        pIntsec = pMap->nextIntsec(&imi);
    }

    return true;
}

//エラーチェック、エラーなら true
bool FileAccess::checkError()
{
    Map*                pMap = app.getMap();
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;

    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        if (pIntsec->checkError())
            return true;
        pIntsec = pMap->nextIntsec(&imi);
    }
    pRoad = pMap->nextRoad(&rmi, true);
    while (pRoad != NULL)
    {
        if (pRoad->checkError())
            return true;
        pRoad = pMap->nextRoad(&rmi);
    }

    return false;
}

//エラーファイル書き込み
bool FileAccess::writeError()
{
    Map*                pMap = app.getMap();
    ofstream            ofs;
    stringstream        message;
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;
    string              messagePart;
    int                 errorIntsec, errorRoad;

    errorIntsec = errorRoad = 0;

    ofs.open(_errorFile.c_str(), ios::out);
    if (!ofs) {
        message << "エラーファイルを開けません。\n" << _errorFile;
        return errorMessage(message.str().c_str());
    }

    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        if (pIntsec->checkError())
        {
            pIntsec->getErrorMessage(&messagePart);
            ofs << "交差点ID:" << pIntsec->getIntsecId() << '\n' << messagePart << '\n';
            errorIntsec++;
        }
        pIntsec = pMap->nextIntsec(&imi);
    }
    pRoad = pMap->nextRoad(&rmi, true);
    while (pRoad != NULL)
    {
        if (pRoad->checkError())
        {
            pRoad->getErrorMessage(&messagePart);
            ofs << "単路の交差点ID:" << pRoad->getIntsec(ROAD_IT_SMALL)->getIntsecId() << '-'
                                     << pRoad->getIntsec(ROAD_IT_LARGE)->getIntsecId() << '\n'
                                     << messagePart << '\n';
            errorRoad++;
        }
        pRoad = pMap->nextRoad(&rmi);
    }

    ofs << endl << "総計 交差点数 " << errorIntsec << " 単路数 " << errorRoad << endl;

    return true;
}

//"..."から文字列抽出、日本語不可、エラーなら false
bool FileAccess::getQuoteString(const char* pSource, int startPos, string* pQuote)
{
    int     pos, len;

    (*pQuote) = "";
    len = strlen(pSource);
    if (startPos >= len - 1)
        return false;
    for (pos = startPos; pos < len; pos++)
    {
        if (pSource[pos] == '\"')
            break;
    }
    if (pos == len)
        return false;
    for (pos++; pos < len; pos++)
    {
        if (pSource[pos] == '\"')
            break;
        (*pQuote) += pSource[pos];
    }
    if (pos == len)
        return false;
//  qDebug("FileAccess::getQuoteString '%s'->'%s'", &(pSource[startPos]), pQuote->c_str());
    return true;
}

//エラーメッセージ、常に false
bool FileAccess::errorMessage(const char* pMessage)
{
    QMessageBox::critical(_pParent, _pParent->tr(TITLE), _pParent->tr(pMessage));
    return false;
}


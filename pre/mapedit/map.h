/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップ

#ifndef MAP_H
#define MAP_H

#include <QtGui>
#include <stdio.h>
#include <map>

#define QT_FATAL_ASSERT

using namespace std;

class IntsecCommon;
class Intsec;
class Road;

#define MAP_NOID            -1              //IDなし
#define MAP_INTSEC_ID_MIN   0               //交差点ID最小／最大（MATES制限）
#define MAP_INTSEC_ID_MAX   999999

//stl map 短縮定義
typedef map<int, Intsec*>               MAP_INTSECMAP;
typedef map<int, Intsec*>::iterator     MAP_INTSECMAP_IT;
typedef map<int, Road*>                 MAP_ROADMAP;
typedef map<int, Road*>::iterator       MAP_ROADMAP_IT;

//マップ
class Map
{
    IntsecCommon*   _pIntsecCommon;                         //交差点共通
    MAP_INTSECMAP   _intsecMap;                             //交差点マップ（交差点IDと交差点情報）
    MAP_ROADMAP     _roadMap;                               //単路マップ（単路IDと単路情報）

public:
    Map();                                                  //コンストラクタ
    ~Map();                                                 //デストラクタ
    void            init();                                 //初期処理
    bool            isNoData()                              //データなし
    {
        return (_intsecMap.size() == 0);
    }
    int             getIntsecNum()                          //交差点数取得
    {
        return ((int)_intsecMap.size());
    }
    void            setCenter(double mapX, double mapY, double mapZ);
                                                            //中心座標設定
    void            deleteAll();                            //全削除
    void            debugTrace();                           //デバッグトレース

    IntsecCommon*   getIntsecCommon()                       //交差点共通取得
    {
        Q_ASSERT(_pIntsecCommon!=NULL);
        return _pIntsecCommon;
    }
    Intsec*         createIntsec(int intsecId=MAP_NOID);    //交差点作成取得、ID 指定なしなら最大 ID の次
                                                            // ID が大きすぎか既存 ID なら NULL
    Intsec*         getIntsec(int intsecId);                //交差点取得、なしなら NULL
    Intsec*         nextIntsec(MAP_INTSECMAP_IT* pimi, bool first=false);
                                                            //次の交差点取得、first なら先頭から、なしなら NULL
    void            deleteIntsec(int intsecId);             //交差点削除、接続単路も削除、なしなら無視
    Intsec*         changeIntsecId(Intsec* pIntsec, int newIntsecId);
                                                            //交差点ID変更、構造的変換なので注意、既存 ID 不可
                                                            //交差点をもらって戻す、元は使えないので注意

    Road*           createRoad(Intsec* pIntsec1, Intsec* pIntsec2, bool* pAlready);
                                                            //単路作成取得、既存なら既存単路を返す
    Road*           getRoad(int intsecId1, int intsecId2);  //単路取得、なしなら NULL
    Road*           nextRoad(MAP_ROADMAP_IT* prmi, bool first=false);
                                                            //次の単路取得、first なら先頭から、なしなら NULL
    void            deleteRoad(int roadId);                 //単路削除、なしなら無視
    Road*           replaceRoad(Intsec* pIntsec, Intsec* pIntsecNew, Intsec* pIntsecCon);
                                                            //単路付け換え、付け換え後単路返却、構造的変換なので注意
                                                            //既存単路不可
};

#endif // MAP_H

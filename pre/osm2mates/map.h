/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップ

#ifndef MAP_H
#define MAP_H

#include <Qt>
#include <stdio.h>
#include <vector>
#include <map>

#define QT_FATAL_ASSERT

using namespace std;

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
    MAP_INTSECMAP   _intsecMap;                             //交差点マップ（交差点IDと交差点情報）
    MAP_ROADMAP     _roadMap;                               //単路マップ（単路IDと単路情報）
    map<int, int>   _intsecFinalId;                         //交差点最終ID、圧縮したもの

public:
    //コンストラクタ
    Map();
                                                     //デストラクタ
    ~Map();
    //交差点数取得
    int             intsecNum()
    {
        return ((int)_intsecMap.size());
    }
    //全削除
    void            deleteAll();

    //交差点作成取得、ID 指定なしなら最大 ID の次
    //既存 ID なら NULL
    Intsec*         createIntsec(int id=MAP_NOID); 
    //交差点検索、なしなら NULL
    Intsec*         findIntsec(int id);
    //次の交差点取得、first なら先頭から、なしなら NULL
    Intsec*         nextIntsec(MAP_INTSECMAP_IT* imi, bool first=false);
    //交差点削除、接続単路も削除、なしなら無視
    void            deleteIntsec(int id);

    //単路作成取得、既存なら既存単路を返す
    Road*           createRoad(Intsec* intsec1, Intsec* intsec2, int defLane, bool* already=NULL);
    //一括道路作成処理
    void            createRoadPackage(Intsec* intsec1, Intsec* intsec2,
                                      int laneForward, int laneBackward);
    //単路検索、なしなら NULL
    Road*           findRoad(int intsecId1, int intsecId2);
    //次の単路取得、first なら先頭から、なしなら NULL
    Road*           nextRoad(MAP_ROADMAP_IT* rmi, bool first=false);
    //単路削除、なしなら無視
    void            deleteRoad(int roadId);

    //整形
    void            refine();
    //強制１レーン
    void            forceOneLane();
    //交差点最終ＩＤ設定
    void            setIntsecFinalId();
    //交差点最終ＩＤ取得
    int             getIntsecFinalId(int id);
};

#endif // MAP_H

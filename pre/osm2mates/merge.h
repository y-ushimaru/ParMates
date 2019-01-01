/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//統合

#ifndef MERGE_H
#define MERGE_H

#include <string>
#include <vector>
#include <map>
#include <AmuPoint.h>

#define QT_FATAL_ASSERT

//モード
#define MERGE_MODE_INTEROP  0
#define MERGE_MODE_NORMAL   1
#define MERGE_MODE_REVISE   2

using namespace std;

class Intsec;

//統合交差点
class MergeIntsec
{
    bool            _deleted;                   //削除フラグ
                                                //vectorから外しにくいのでフラグで処理
    map<int,Intsec*>
                    _intsecs;                   //交差点
    double          _mapX, _mapY, _mapZ;        //マップ位置

public:
    //コンストラクタ
    MergeIntsec();

    //マップ位置計算
    void calcMapPos(bool final=false);
    //マップ位置取得
    double mapPosX()
    {
        return _mapX;
    }
    double mapPosY()
    {
        return _mapY;
    }
    double mapPosZ()
    {
        return _mapZ;
    }

    //交差点追加
    void addIntsec(Intsec* intsec);
    //全交差点移動、ソースは削除フラグ設定
    void moveAllIntsecs(MergeIntsec* src);
    //統合
    void merge();
};

//統合
class Merge
{
    vector<MergeIntsec*>    _mergeIntsecs;      //統合交差点
    Intsec*                 _intsec;            //処理中の交差点
    AmuPoint                _pointIntsec;       //処理中の交差点の位置
    map<int,Intsec*>        _intsecsFind;       //検索済み交差点
    bool                    _interOp;           //補間点

public:
    //コンストラクタ
    Merge();
    //デストラクタ
    ~Merge();
    //全削除
    void deleteAll();
    //統合メイン
    void merge(int mode);

    //統合マップ位置
    void getMergeMapPos(Intsec* intsec, AmuPoint* point);
    //交差点を検索して統合
    void findMergeIntsec(Intsec* intsecFind, int findNum);
    //交差点を統合
    void mergeIntsec(Intsec* intsecCon);
};

#endif // MERGE_H

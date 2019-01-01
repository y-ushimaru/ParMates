/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点

#ifndef INTSEC_H
#define INTSEC_H

#include <Qt>
#include <map>
#include <vector>
#include <string>

#define QT_FATAL_ASSERT

using namespace std;

class Road;
class MergeIntsec;

// stl map 短縮定義
typedef map<int, Road*>                     INTSEC_ROADMAP;
typedef map<int, Road*>::iterator           INTSEC_ROADMAP_IT;
typedef multimap<double, int>               INTSEC_ANGLEMAP;
typedef multimap<double, int>::iterator     INTSEC_ANGLEMAP_IT;

// 交差点
// 単路マップはマップクラスが主体、単路の生成や消去はマップクラス、それに合わせる
// ID が接続先交差点なのは交差点だけから単路を探すため
class Intsec
{
    int             _id;                            //交差点ID
    double          _mapX, _mapY, _mapZ;            //マップ位置
    int             _firstIdCon;                    //先頭接続交差点ID
    INTSEC_ROADMAP  _roadMap;                       //単路マップ、接続交差点IDと単路情報
    INTSEC_ANGLEMAP _angleMap;                      //角度マップ、角度と接続交差点ID、先頭から反時計回り
    MergeIntsec*    _mergeIntsec;                   //統合交差点
    bool            _border;                        //境界
    bool            _interOp;                       //補間点、DRM 用
    int             _error;                         //エラー種別

public:
    //コンストラクタ
    Intsec(int id);
    //デストラクタ
    ~Intsec(){}

    //交差点ID取得
    int id()
    {
        return _id;
    }
    //マップ位置設定
    void setMapPos(double mx, double my, double mz)
    {
        _mapX = mx;
        _mapY = my;
        _mapZ = mz;
    }
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
    //マップ位置コピー
    void copyMapPos(Intsec* intsecSource)
    {
        _mapX = intsecSource->_mapX;
        _mapY = intsecSource->_mapY;
        _mapZ = intsecSource->_mapZ;
    }
    //統合交差点設定／取得
    void setMergeIntsec(MergeIntsec* mergeIntsec)
    {
        _mergeIntsec = mergeIntsec;
    }
    MergeIntsec* mergeIntsec()
    {
        return _mergeIntsec;
    }
    //境界設定／取得
    void setBorder(bool border)
    {
        _border = border;
    }
    bool border()
    {
        return _border;
    }
    //補間点
    void setInterOp(bool interOp)
    {
        _interOp = interOp;
    }
    bool interOp()
    {
        return _interOp;
    }
    //先頭接続交差点ID取得、単路なしならIDなし（NOID）
    int firstIdCon()
    {
        return _firstIdCon;
    }
    //先頭接続交差点ID設定、なし設定不可
    void setFirstIdCon(int idCon)
    {
        _firstIdCon = idCon;
    }
    //単路追加
    void addRoad(int idCon, Road* road);
    //単路取得、なしなら NULL
    Road* road(int idCon);
    //単路数取得
    int roadNum()
    {
        return (int)_roadMap.size();
    }
    //次の単路取得、first なら先頭から、なしなら NULL
    Road* nextRoad(INTSEC_ROADMAP_IT* rmi, bool first=false, int* idCon=NULL);
    //単路削除、なしなら無視
    void deleteRoad(int idCon);
private:
    //単路角度取得、反時計回りまたは絶対値、Z座標は無視
    double roadAngle(int idSource, int idDest, bool absolute=false);

public:
    //角度による次の単路取得、first なら先頭から、なしなら NULL
    //  先頭から反時計回り、first で角度計算後すぐに呼ぶ事
    //  先頭以外で同一角度なら ID 昇順
    Road* nextRoadByAngle(INTSEC_ANGLEMAP_IT* ami, bool first=false, int* idCon=NULL);
    //エラーチェック、エラーなら true
    bool checkError(); 
private:
    //2 単路車線エラーチェック、エラーなら true
    bool check2LaneError(Road* road1, Road* road2, int intsecType1, int intsecType2);

    //直線車線エラーチェック、エラーなら true
    bool checkStraightError(Road* road1, Road* road2, int intsecType1, int intsecType2);

public: 
    //エラーメッセージ取得、複数行
    void getErrorMessages(string *message, vector<int>* errorCnt, vector<string>* errorStr);

private:
    //エラーメッセージ取得、１行
    void getErrorMessage(string *message, vector<int>* errorCnt, vector<string>* errorStr,
                         int id);
    
};

#endif // INTSEC_H

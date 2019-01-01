/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点

#ifndef INTSEC_H
#define INTSEC_H

#include <QtGui>
#include <QLabel>
#include <QMessageBox>
#include <map>
#include <vector>

#define QT_FATAL_ASSERT

using namespace std;

class Road;

//ラベル種別
#define INTSEC_LT_NORMAL        0
#define INTSEC_LT_SELECT        1
#define INTSEC_LT_DEST          2
#define INTSEC_LT_NUM           3
#define INTSEC_LT_NOCHANGE      -1

//ラベルサイズ
#define INTSEC_LABELSIZE        16
#define INTSEC_LABELSIZE_H      (INTSEC_LABELSIZE / 2)

//stl map 短縮定義
typedef map<int, Road*>                     INTSEC_ROADMAP;
typedef map<int, Road*>::iterator           INTSEC_ROADMAP_IT;
typedef multimap<double, int>               INTSEC_ANGLEMAP;
typedef multimap<double, int>::iterator     INTSEC_ANGLEMAP_IT;

//交差点共通
class IntsecCommon
{
    vector<QPixmap*>   _pLabelPixmap;               //ラベル画像

 public:
    IntsecCommon();                                 //コンストラクタ
    ~IntsecCommon();                                //デストラクタ
    int                getLabelPixmapId(int labelType, bool error = false, bool dragPrev = false);
    int                getLabelPixmapId(int labelType, bool error, bool dragPrev, bool clear);
                                                    //ラベル画像ID取得
    QPixmap*           getLabelPixmap(int labelPixmapId)
                                                    //画像ラベル取得
    {
        Q_ASSERT(labelPixmapId >= 0 && labelPixmapId < (int)_pLabelPixmap.size());
        Q_ASSERT(_pLabelPixmap[labelPixmapId] != NULL);
        return _pLabelPixmap[labelPixmapId];
    }
};

//交差点ラベル
class IntsecLabel: public QLabel
{
    int         _intsecId;                              //交差点ID
    int         _labelPixmapId;                         //ラベル画像ID
public:
    IntsecLabel(int intsecId, QWidget *parent);         //コンストラクタ
    void        setLabelPixmap(int labelType, bool error = false, bool dragPrev = false);
                                                        //ラベル画像設定、変更時のみ
protected:
    void        mouseMoveEvent(QMouseEvent *event);     //マウス移動イベント
};

//交差点
//　単路マップはマップクラスが主体、単路の生成や消去はマップクラス、それに合わせる
//　ID が接続先交差点なのは交差点だけから単路を探すため
class Intsec
{
    int             _intsecId;                    	//交差点ID
    double          _mapX, _mapY, _mapZ;            //マップ位置
    int             _error;                         //エラー種別
    int             _labelType;                     //ラベル種別
    int             _firstIntsecIdCon;              //先頭接続交差点ID
    IntsecLabel*    _pLabelPicture;                 //画像表示用ラベル
    INTSEC_ROADMAP  _roadMap;                       //単路マップ、接続交差点IDと単路情報
    INTSEC_ANGLEMAP _angleMap;                      //角度マップ、角度と接続交差点ID、先頭から反時計回り

public:
    Intsec(int id);                                 //コンストラクタ
    ~Intsec()                                       //デストラクタ
    {
        if (_pLabelPicture != NULL)
            delete _pLabelPicture;
    }
    void debugTrace();                              //デバッグトレース
    int getIntsecId()                               //交差点ID取得
    {
        return _intsecId;
    }
    void setMapPos(double mx, double my, double mz) //マップ位置設定
    {
        _mapX = mx;
        _mapY = my;
        _mapZ = mz;
    }
    double getMapPosX()                             //マップ位置取得
    {
        return _mapX;
    }
    double getMapPosY()
    {
        return _mapY;
    }
    double getMapPosZ()
    {
        return _mapZ;
    }
    void copyMapPos(Intsec* pIntsecSource)          //マップ位置コピー
    {
        _mapX = pIntsecSource->_mapX;
        _mapY = pIntsecSource->_mapY;
        _mapZ = pIntsecSource->_mapZ;
    }
    bool checkError();                              //エラーチェック、エラーなら true、ラベル表示で反映
private:
    bool check2LaneError(Road* pRoad1, Road* pRoad2, int intsecType1, int intsecType2);
                                                    //2 単路車線エラーチェック、エラーなら true
    bool checkStraightError(Road* pRoad1, Road* pRoad2, int intsecType1, int intsecType2);
                                                    //直線車線エラーチェック、エラーなら true
public:
    bool isError();                                 //エラー判定（チェックなし）
    void getErrorMessage(string *pMessage);         //エラーメッセージ取得（複数行）

    void showLabel(int lt=INTSEC_LT_NOCHANGE, bool dragPrev = false);
                                                    //ラベル表示
    void hideLabel();                               //ラベル消去
    int getLabelPosX()                              //ラベル位置返却、画像用
    {
        return _pLabelPicture->pos().x();
    }
    int getLabelPosY()
    {
        return _pLabelPicture->pos().y();
    }
    const QPixmap* getPixmapLabel();                //ラベル画像取得

    int getFirstIntsecIdCon()                       //先頭接続交差点ID取得、単路なしならIDなし（NOID）
    {
        return _firstIntsecIdCon;
    }
    void setFirstIntsecIdCon(int intsecIdCon)       //先頭接続交差点ID設定、なし設定不可
    {
        _firstIntsecIdCon = intsecIdCon;
    }
    void addRoad(int intsecIdCon, Road* pRoad);     //単路追加
    Road* getRoad(int intsecIdCon);                 //単路取得、なしなら NULL
    int getRoadNum()                                //単路数取得
    {
        return (int)_roadMap.size();
    }
    Road* nextRoad(INTSEC_ROADMAP_IT* prmi, bool first=false, int* pIntsecIdCon=NULL);
                                                    //次の単路取得、first なら先頭から、なしなら NULL
    void deleteRoad(int intsecIdCon);               //単路削除、なしなら無視
private:
    double getRoadAngle(int intsecIdSource, int intsecIdDest, bool absolute=false);
                                                    //単路角度取得、反時計回りまたは絶対値、Z座標は無視
public:
    Road* nextRoadByAngle(INTSEC_ANGLEMAP_IT* pami, bool first=false, int* pIntsecIdCon=NULL);
                                                    //角度による次の単路取得、first なら先頭から、なしなら NULL
                                                    //　先頭から反時計回り、first で角度計算後すぐに呼ぶ事
                                                    //　先頭以外で同一角度なら ID 昇順
};

#endif // INTSEC_H

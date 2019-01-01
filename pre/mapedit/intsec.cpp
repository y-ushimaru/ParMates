/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点

//IrfanViewでpng透明画像を作れば透明にできる、セーブ時に背景色指定
//アイコンの中心が交差点位置でないのに注意、影がある分1ビット上、普通にやると左上

//直線レーン統合不可、交差点ファイルで統合しても衝突発生

#include <QtGui>
#include <vector>
#include <math.h>
#include "app.h"
#include "map.h"
#include "mapframe.h"
#include "intsec.h"
#include "road.h"
#include <AmuVector.h>

#define ROAD_MAX                10              //最大接続単路数、大きいと MATES 本体が動かない

//ラベル画像ID、エラー加算分、ドラッグ前加算分、クリア加算分、全体数、なし
#define LPID_ADD_CLEAR           INTSEC_LT_NUM
#define LPID_ADD_ERROR           (LPID_ADD_CLEAR    * 2)
#define LPID_ADD_DRAGPREV        (LPID_ADD_ERROR    * 2)        //必ず最後である必要
#define LPID_ALL                 (LPID_ADD_DRAGPREV * 2)
#define LPID_NONE                -1

//エラー種類（ビット）
#define ERROR_NONE              0x0000          //エラーなし
#define ERROR_ROAD_NONE         0x0001          //単路なし
#define ERROR_ROAD_OVERMAX      0x0002          //最大接続単路数より大きい
#define ERROR_LANE_ROAD2        0x0004          //2 単路車線数不正
#define ERROR_LANE_STRAIGHT     0x0008          //直進車線数不正、車線数減少が 3　以上、交差点ファイル必要

//交差点共通コンストラクタ
IntsecCommon::IntsecCommon()
{
    int         id;
    QPainter    painter;

    for (id = 0; id < LPID_ALL; id++)
        _pLabelPixmap.push_back(NULL);
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_NORMAL, false, false, false)] =
        new QPixmap(":/icon/intsec_normal.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_SELECT, false, false, false)] =
        new QPixmap(":/icon/intsec_select.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_DEST,   false, false, false)] =
        new QPixmap(":/icon/intsec_dest.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_NORMAL, false, false, true )] =
        new QPixmap(":/icon/intsec_clear_normal.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_SELECT, false, false, true )] =
        new QPixmap(":/icon/intsec_clear_select.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_DEST,   false, false, true )] =
        new QPixmap(":/icon/intsec_clear_dest.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_NORMAL, true,  false, false)] =
        new QPixmap(":/icon/intsec_error_normal.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_SELECT, true,  false, false)] =
        new QPixmap(":/icon/intsec_error_select.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_DEST,   true,  false, false)] =
        new QPixmap(":/icon/intsec_error_dest.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_NORMAL, true,  false, true )] =
        new QPixmap(":/icon/intsec_clear_error_normal.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_SELECT, true,  false, true )] =
        new QPixmap(":/icon/intsec_clear_error_select.png");
    _pLabelPixmap[getLabelPixmapId(INTSEC_LT_DEST,   true,  false, true )] =
        new QPixmap(":/icon/intsec_clear_error_dest.png");

    for (id = 0; id < LPID_ADD_DRAGPREV; id++)
    {
        _pLabelPixmap[id + LPID_ADD_DRAGPREV] = new QPixmap(*_pLabelPixmap[id]);
        painter.begin(_pLabelPixmap[id + LPID_ADD_DRAGPREV]);
        painter.fillRect(_pLabelPixmap[id + LPID_ADD_DRAGPREV]->rect(), QColor(127, 127, 127, 127));
        painter.end();
    }
}

//交差点共通デストラクタ
IntsecCommon::~IntsecCommon()
{
    int     id;

    for (id = 0; id < LPID_ALL; id++)
    {
        if (_pLabelPixmap[id] != NULL)
            delete _pLabelPixmap[id];
    }
}

//ラベル画像ID取得
int IntsecCommon::getLabelPixmapId(int labelType, bool error, bool dragPrev)
{
    return getLabelPixmapId(labelType, error, dragPrev, app.getClearView());
}
int IntsecCommon::getLabelPixmapId(int labelType, bool error, bool dragPrev, bool clear)
{
    int     id;

    id = labelType;
    if (clear)
        id += LPID_ADD_CLEAR;
    if (error)
        id += LPID_ADD_ERROR;
    if (dragPrev)
        id += LPID_ADD_DRAGPREV;
//  qDebug("IntsecCommon::getLabelPixmapId %d", id);
    return id;
}

//交差点ラベルコンストラクタ
IntsecLabel::IntsecLabel(int intsecId, QWidget *parent) : QLabel(parent)
{
    _intsecId = intsecId;
    _labelPixmapId = LPID_NONE;
    setMouseTracking(true);
}

//ラベル画像設定、変更時のみ
void IntsecLabel::setLabelPixmap(int labelType, bool error, bool dragPrev)
{
    int             labelPixmapId;
    IntsecCommon*   pic = app.getMap()->getIntsecCommon();

    Q_ASSERT(labelType >= 0 && labelType < INTSEC_LT_NUM);
    labelPixmapId = pic->getLabelPixmapId(labelType, error, dragPrev);
    if (labelPixmapId != _labelPixmapId)
    {
        _labelPixmapId = labelPixmapId;
        setPixmap(*(pic->getLabelPixmap(_labelPixmapId)));
    }
}

//交差点ラベルマウス移動イベント
void IntsecLabel::mouseMoveEvent(QMouseEvent *event)
{
    //ステータス表示、マウス移動
    app.getMapFrame()->showStatus(x() + event->pos().x(), y() + event->pos().y(), _intsecId);
    app.getMapFrame()->mouseMove(x() + event->pos().x(), y() + event->pos().y());
}

//コンストラクタ
Intsec::Intsec(int id)
{
    _intsecId = id;
    _mapX = _mapY = _mapZ = 0;
    _error = ERROR_NONE;
    _labelType = INTSEC_LT_NORMAL;
    _pLabelPicture = NULL;
    _firstIntsecIdCon = MAP_NOID;
}

//デバッグトレース
void Intsec::debugTrace()
{
    INTSEC_ROADMAP_IT   rmi;

    qDebug("Intsec::DebugTrace id=%d, map=(%d, %d, %d)", _intsecId,
           (int)_mapX, (int)_mapY, (int)_mapZ);
    for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
        qDebug("Intsec::DebugTrace id=%d, roadId=%d", _intsecId, rmi->first);
}

//エラーチェック、エラーなら true、ラベル表示で反映
bool Intsec::checkError()
{
    INTSEC_ANGLEMAP_IT  ami;
    Road*               pRoad;
    int                 intsecType, intsecTypeCon, i, j, k, l;
    vector<Road*>       pRoads;
    vector<int>         intsecTypes;
    vector<int>         intsecIdCons;
    vector<double>      angleRoads;
    vector<int>         straightRoads;
    double              angleSum, angleStraight, angleStraightMin;

    _error = ERROR_NONE;

    if      (_roadMap.size() > ROAD_MAX)
        _error = _error | ERROR_ROAD_OVERMAX;
    else if (_roadMap.size() == 0)
        _error = _error | ERROR_ROAD_NONE;

    if (_roadMap.size() >= 2)
    {
//      qDebug("road %d", _roadMap.size());
        pRoad = nextRoadByAngle(&ami, true);
        while (pRoad != NULL)
        {
            intsecType = pRoad->getIntsecType(_intsecId);
            Q_ASSERT(intsecType != ROAD_IT_NONE);
            if (intsecType == ROAD_IT_SMALL)
                intsecTypeCon = ROAD_IT_LARGE;
            else
                intsecTypeCon = ROAD_IT_SMALL;
            pRoads.push_back(pRoad);
            intsecTypes.push_back(intsecType);
            intsecIdCons.push_back(pRoad->getIntsec(intsecTypeCon)->getIntsecId());
//          qDebug("Intsec::checkError road %d-%d", _intsecId, pRoad->getIntsec(intsecTypeCon)->getIntsecId());
            pRoad = nextRoadByAngle(&ami);
        }
        if (_roadMap.size() == 2)
        {
            if (check2LaneError(pRoads[0], pRoads[1], intsecTypes[0], intsecTypes[1]) ||
                check2LaneError(pRoads[1], pRoads[0], intsecTypes[1], intsecTypes[0]))
                _error = _error | ERROR_LANE_ROAD2;
        }
        else
        {
//          qDebug("Intsec::checkError many roads");
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                j = (i + (int)intsecIdCons.size() + 1) % (int)intsecIdCons.size();
                angleRoads.push_back(getRoadAngle(intsecIdCons[i], intsecIdCons[j], true));
                straightRoads.push_back(-1);
            }
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                angleSum = 0;
                angleStraightMin = 1000;
                for (j = 1; j < (int)intsecIdCons.size(); j++)
                {
                    k = (i + j - 1 + (int)intsecIdCons.size()) % (int)intsecIdCons.size();
                    l = (i + j     + (int)intsecIdCons.size()) % (int)intsecIdCons.size();
                    angleSum += angleRoads[k];
                    angleStraight = fabs(angleSum - M_PI);
                    if (angleStraightMin > angleStraight)
                    {
                        angleStraightMin = angleStraight;
                        straightRoads[i] = l;
                    }
                }
            }
            for (i = 0; i < (int)intsecIdCons.size(); i++)
            {
                j = straightRoads[i];
                Q_ASSERT(j >= 0);
                if (straightRoads[j] == i)
                {
                    //逆は後でやるので一度呼べばいい
                    if (checkStraightError(pRoads[i], pRoads[j], intsecTypes[i], intsecTypes[j]))
                        _error = _error | ERROR_LANE_STRAIGHT;
                }
            }
        }
    }

//  qDebug("Intsec::checkError error 0x%x", _error);
    return (_error != ERROR_NONE);
}

//2 単路車線エラーチェック、エラーなら true
bool Intsec::check2LaneError(Road* pRoad1, Road* pRoad2, int intsecType1, int intsecType2)
{
//  qDebug("check 2 lane %d %d - %d", _intsecId,
//         pRoad1->getIntsec(ROAD_IT_REVERSE(intsecType1))->getIntsecId(),
//         pRoad2->getIntsec(ROAD_IT_REVERSE(intsecType2))->getIntsecId());
    if (pRoad1->getLane(intsecType1, ROAD_LT_OUT) > pRoad2->getLane(intsecType2, ROAD_LT_IN))
    {
//      qDebug("error 1");
        return true;
    }
    else if (pRoad1->getLane(intsecType1, ROAD_LT_OUT) == 0 &&
             pRoad2->getLane(intsecType2, ROAD_LT_IN)  >= 1)
    {
//      qDebug("error 2");
        return true;
    }
    return false;
}

//直線車線エラーチェック、エラーなら true
bool Intsec::checkStraightError(Road* pRoad1, Road* pRoad2, int intsecType1, int intsecType2)
{
//  qDebug("check straight %d %d(%d) - %d(%d)", _intsecId,
//         pRoad1->getIntsec(ROAD_IT_REVERSE(intsecType1))->getIntsecId(),
//         pRoad1->getLane(intsecType1, ROAD_LT_OUT),
//         pRoad2->getIntsec(ROAD_IT_REVERSE(intsecType2))->getIntsecId(),
//         pRoad2->getLane(intsecType2, ROAD_LT_IN));
    if (pRoad1->getLane(intsecType1, ROAD_LT_OUT) > pRoad2->getLane(intsecType2, ROAD_LT_IN) + 2)
    {
//      qDebug("error");
        return true;
    }
    return false;
}

//エラー判定（チェックなし）
bool Intsec::isError()
{
    return (_error != ERROR_NONE);
}

//エラーメッセージ取得（複数行）
void Intsec::getErrorMessage(string *pMessage)
{
    (*pMessage) = "";
    if (_error & ERROR_ROAD_NONE)
        (*pMessage) += "There are no sections.\n";
    if (_error & ERROR_ROAD_OVERMAX)
        (*pMessage) += "There are too many sections.\n";
    if (_error & ERROR_LANE_ROAD2)
        (*pMessage) += "Numbers of lanes in 2 sections are invalid.\n";
    if (_error & ERROR_LANE_STRAIGHT)
        (*pMessage) += "Intersection file is needed due to invalid numbers of straight lanes.\n";
}

//ラベル表示
void Intsec::showLabel(int lt, bool dragPrev)
{
    MapFrame*       pmf = app.getMapFrame();
    QPalette        pal;

    if (lt != INTSEC_LT_NOCHANGE)
        _labelType = lt;
    Q_ASSERT(0<=_labelType&&_labelType<=INTSEC_LT_NUM);

//  qDebug("Intsec::showLabel (%d, %d) id=%d type=%d",
//         pmf->getViewY(_mapX) - INTSEC_LABELSIZE_H,
//         pmf->getViewY(_mapY) - INTSEC_LABELSIZE_H, _intsecId, _labelType);
    if (_pLabelPicture == NULL)
        _pLabelPicture = new IntsecLabel(_intsecId, pmf);
    _pLabelPicture->setLabelPixmap(_labelType, _error != ERROR_NONE, dragPrev);
    _pLabelPicture->move(pmf->getViewX(_mapX) - INTSEC_LABELSIZE_H,
                         pmf->getViewY(_mapY) - INTSEC_LABELSIZE_H);
    _pLabelPicture->show();
//  _pLabelPicture->setAttribute(Qt::WA_DeleteOnClose);	//自分で管理するので不要
}

//ラベル消去
void Intsec::hideLabel()
{
    if (_pLabelPicture != NULL)
        delete _pLabelPicture;
    _pLabelPicture = NULL;
}

//ラベル画像取得
const QPixmap* Intsec::getPixmapLabel()
{
    if (_pLabelPicture == NULL)
        showLabel();
    return _pLabelPicture->pixmap();
}

//単路追加
void Intsec::addRoad(int intsecIdCon, Road* pRoad)
{
    INTSEC_ROADMAP_IT   rmi;

    //重複IDならエラー、先頭接続交差点IDなしなら設定
//  qDebug("Intsec::addRoad %d-%d", _intsecId, intsecIdCon);
    rmi = _roadMap.find(intsecIdCon);
    Q_ASSERT(rmi==_roadMap.end());
    _roadMap.insert(pair<int, Road*>(intsecIdCon, pRoad));
    if (_firstIntsecIdCon == MAP_NOID)
        _firstIntsecIdCon = intsecIdCon;
}

//単路取得、なしなら NULL
Road* Intsec::getRoad(int intsecIdCon)
{
    INTSEC_ROADMAP_IT   rmi;

//  qDebug("Intsec::getRoad %d-%d", _intsecId, intsecIdCon);
    rmi = _roadMap.find(intsecIdCon);
    if (rmi == _roadMap.end())
        return NULL;

//  qDebug("Intsec::getRoad not null end %d-%d", _intsecId, intsecIdCon);
    return rmi->second;
}

//次の単路取得、first なら先頭から、なしなら NULL
Road* Intsec::nextRoad(INTSEC_ROADMAP_IT* prmi, bool first, int* pIntsecIdCon)
{
    if (first)
        *prmi = _roadMap.begin();
    else
        (*prmi)++;

    if (*prmi == _roadMap.end())
    {
        if (pIntsecIdCon != NULL)
            *pIntsecIdCon = MAP_NOID;
        return NULL;
    }
    if (pIntsecIdCon != NULL)
        *pIntsecIdCon = (*prmi)->first;
    return (*prmi)->second;
}

//単路削除、なしなら無視
void Intsec::deleteRoad(int intsecIdCon)
{
    INTSEC_ROADMAP_IT   rmi;

    //自データのみ削除、オブジェクト削除はマップ、先頭接続交差点ID設定
//  qDebug("Intsec::deleteRoad %d-%d", _intsecId, intsecIdCon);
    rmi = _roadMap.find(intsecIdCon);
    if (rmi != _roadMap.end())
    {
        _roadMap.erase(rmi);
        if (intsecIdCon == _firstIntsecIdCon)
        {
            rmi = _roadMap.begin();
            if (rmi != _roadMap.end())
                _firstIntsecIdCon = rmi->first;
            else
                _firstIntsecIdCon = MAP_NOID;
        }
    }
}

//単路角度取得、反時計回りまたは絶対値、Z座標は無視
double Intsec::getRoadAngle(int intsecIdSource, int intsecIdDest, bool absolute)
{
    Map*        pMap = app.getMap();
    Intsec*     pIntsecSource;
    Intsec*     pIntsecDest;
    AmuVector    vectorSource, vectorDest;
    double      angle;

    pIntsecSource = pMap->getIntsec(intsecIdSource);
    pIntsecDest   = pMap->getIntsec(intsecIdDest);
    Q_ASSERT(pIntsecSource != NULL && pIntsecDest != NULL);
    vectorSource.setXYZ(pIntsecSource->getMapPosX() - _mapX,
                        pIntsecSource->getMapPosY() - _mapY, 0);
    vectorDest.setXYZ(pIntsecDest->getMapPosX() - _mapX,
                      pIntsecDest->getMapPosY() - _mapY, 0);
    angle = -vectorSource.calcAngle(vectorDest);        //時計回り→反時計回り
    if (angle < 0)
    {
        if (!absolute)
            angle = 2 * M_PI + angle;
        else
            angle = -angle;
    }
    return angle;
}

//角度による次の単路取得、first なら先頭から、なしなら NULL
//  先頭から反時計回り、first で角度計算後すぐに呼ぶ事、先頭以外で同一角度なら ID 昇順
Road* Intsec::nextRoadByAngle(INTSEC_ANGLEMAP_IT* pami, bool first, int* pIntsecIdCon)
{
    INTSEC_ROADMAP_IT   rmi;

    if (first)
    {
        _angleMap.clear();
        if (_firstIntsecIdCon != MAP_NOID)
        {
            _angleMap.insert(pair<double, int>(0, _firstIntsecIdCon));
            for (rmi = _roadMap.begin(); rmi != _roadMap.end(); rmi++)
            {
                if (rmi->first != _firstIntsecIdCon)
                    _angleMap.insert(pair<double, int>(getRoadAngle(_firstIntsecIdCon, rmi->first),
                                                       rmi->first));
            }
        }
        *pami = _angleMap.begin();
    }
    else
        (*pami)++;

    if (*pami == _angleMap.end())
    {
        if (pIntsecIdCon != NULL)
            *pIntsecIdCon = MAP_NOID;
        return NULL;
    }
    if (pIntsecIdCon != NULL)
        *pIntsecIdCon = (*pami)->second;
    rmi = _roadMap.find((*pami)->second);
    Q_ASSERT(rmi != _roadMap.end());    
//  qDebug("Intsec::nextRoadByAngle %d %d %f", _intsecId, (*pami)->second, (*pami)->first);
    return rmi->second;
}


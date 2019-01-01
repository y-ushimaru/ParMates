/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップフレーム

#include <stdlib.h>
#include <string>
#include <sstream>
#include <QtGui>
#include <QStatusBar>
#include "app.h"
#include "mainwindow.h"
#include "mapframe.h"
#include "map.h"
#include "intsec.h"
#include "intsecDialog.h"
#include "road.h"
#include "roadDialog.h"
#include <iostream>
using namespace std;

//座標は設定位置で受けて表示位置に直す
//画面系の全体的処理のみ、それ以外は別クラスに渡す
// QPicture は全然遅いので使えない、QPixmap ならそうでもない

#define TITLE               "MapEditor"                     //ダイアログタイトル
#define DRAGBEGIN_DIST      (INTSEC_LABELSIZE / 4)          //ドラッグ開始距離
#define DRAGBEGIN_TIME      300                             //ドラッグ開始時間(ms)
#define ITEM_MINETYPE       "application/x-dnditemdata"     //ドラッグ用アイテムデータ種別
#define ROADSELECT_MIN      (INTSEC_LABELSIZE / 2)          //単路選択最小距離
#define DIALOG_NOPOS        0x7FFFFFFF                      //ダイアログ位置なし
#define MINIMUM_SIZE_X      200                             //最小サイズ
#define MINIMUM_SIZE_Y      100
#define VIEWMAGNI_CHANGE    0.2                             //表示倍率変更率

//コンストラクタ
MapFrame::MapFrame(QWidget *parent): QFrame(parent)
{
    //初期化
    _intsecIdSelect =_intsecIdDest =_roadIdSelect = _intsecIdDrag = _intsecIdReplCon = MAP_NOID;
    _clickDrag = false;
    _clickDragX = _clickDragY = 0;
    _dragType = none;
    _offsetDragX = _offsetDragY = 0;
    _dragRoadX = _dragRoadY = _dragRoadConX = _dragRoadConY = 0;
    _intsecDialogX = _intsecDialogY = _roadDialogX = _roadDialogY = DIALOG_NOPOS;
    _scrollBeginMapX = _scrollBeginMapY = 0;

    //フレーム設定
    setMinimumSize(MINIMUM_SIZE_X, MINIMUM_SIZE_Y);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAcceptDrops(true);
    setMouseTracking(true);
}

//マップ位置から表示位置取得、四捨五入
int MapFrame::getViewX(double mapX)
{
//  qDebug("MapFrame::getViewX %d->(-%f *%f)->%d", mapX, app.getViewMapX(), app.getViewMagni(),
//         (int)(((double)mapX - app.getViewMapX()) * app.getViewMagni()));
    return (int)(((double)mapX - app.getViewMapX()) * app.getViewMagni() + 0.5);
}
int MapFrame::getViewY(double mapY)
{
//  qDebug("MapFrame::getViewY %d->(-%f *%f)->%d", mapY, app.getViewMapY(), app.getViewMagni(),
//         (int)(((double)mapY - app.getViewMapY()) * app.getViewMagni()));
    return (height() - (int)(((double)mapY - app.getViewMapY()) * app.getViewMagni() + 0.5));
}

//表示位置からマップ位置取得、四捨五入
double MapFrame::getMapX(int viewX)
{
    return ((double)viewX / app.getViewMagni() + app.getViewMapX() + 0.5);
}
double MapFrame::getMapY(int viewY)
{
//  qDebug("MapFrame::getMapY %d->(-%f *%f)->%d", viewY, app.getViewMapY(), app.getViewMagni(),
//         (int)((double)(height() - viewY) / app.getViewMagni() + app.getViewMapY()));
    return ((double)(height() - viewY) / app.getViewMagni() + app.getViewMapY() + 0.5);
}

//編集モード変更
void MapFrame::changeEditMode()
{
    //選択解除
    if (app.getMainWindow()->getEditMode() == MainWindow::intsec)
        changeShowSelect(_intsecIdSelect, MAP_NOID, MAP_NOID);
}

//特定位置の交差点取得、なしなら NULL
Intsec* MapFrame::getIntsecAt(int viewX, int viewY)
{
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    Map*                pMap = app.getMap();
    int                 intsecViewX, intsecViewY;

    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        intsecViewX = getViewX(pIntsec->getMapPosX());
        intsecViewY = getViewY(pIntsec->getMapPosY());
//      qDebug("MapFrame::getIntsecAt intsec=(%d, %d) view=(%d, %d)",
//             intsecViewX, intsecViewY, viewX, viewY);
        if (intsecViewX - INTSEC_LABELSIZE_H <= viewX &&
            intsecViewY - INTSEC_LABELSIZE_H <= viewY &&
            intsecViewX + INTSEC_LABELSIZE_H >= viewX &&
            intsecViewY + INTSEC_LABELSIZE_H >= viewY )
            return pIntsec;
        pIntsec = pMap->nextIntsec(&imi);
    }
    return NULL;
}

//特定位置の道路取得、なしなら NULL
Road* MapFrame::getRoadAt(int viewX, int viewY)
{
    Map*            pMap = app.getMap();
    Road*           pRoad;
    Road*           pRoadSelect;
    MAP_ROADMAP_IT  rmi;
    double          distance, distancePrev;

    pRoad = pMap->nextRoad(&rmi, true);
    pRoadSelect = NULL;
    distancePrev = (double)ROADSELECT_MIN;
    while (pRoad != NULL)
    {
        if (app.measureLinePoint(getViewX(pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosX()),
                                 getViewY(pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosY()),
                                 getViewX(pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosX()),
                                 getViewY(pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosY()),
                                 viewX, viewY, ROADSELECT_MIN, &distance))
        {
            if (distance < distancePrev)
            {
//              qDebug("MapFrame::mouseReleaseEvent road %f-%f", distance, distancePrev);
                pRoadSelect = pRoad;
                distancePrev = distance;
            }
        }
        pRoad = pMap->nextRoad(&rmi);
    }

    return pRoadSelect;
}

//操作系-------------------------------------------------------

//マウス移動
void MapFrame::mouseMove(int viewX, int viewY)
{
    //クリック／ドラッグで一定距離／時間以上ならドラッグ
//  qDebug("MapFrame::MouseMove");
    if (!_clickDrag)
        return;
//  qDebug("MapFrame::MouseMove click-drag (%d,%d)", viewX, viewY);
    if ( abs(_clickDragX - viewX) <= DRAGBEGIN_DIST && abs(_clickDragY - viewY) <= DRAGBEGIN_DIST )
        return;

    //ドラッグ
    drag();
}

//マウス押下イベント
void MapFrame::mousePressEvent(QMouseEvent *event)
{
    //右ボタンならクリック／ドラッグ、まだ未確定
//  qDebug("MapFrame::mousePressEvent");
    if (event->button()==Qt::LeftButton)
    {
//      qDebug("MapFrame::mousePressEvent click-drag (%d,%d)",
//              event->pos().x(), event->pos().y());
        _clickDrag = true;
        _clickDragX = event->pos().x();
        _clickDragY = event->pos().y();
        QTimer::singleShot(DRAGBEGIN_TIME, this, SLOT(dragTimer()));
    }
}

//マウス開放イベント
void MapFrame::mouseReleaseEvent(QMouseEvent *event)
{
    Map*        pMap = app.getMap();
    Intsec*     pIntsec;
    Road*       pRoad;
    bool        roadSelected;
    int         intsecIdSmall, intsecIdLarge, intsecIdNew;
    double      x, y;

    //右ボタンクリック／ドラッグならクリック、このチェックはいらないようだが念のため
//  qDebug("MapFrame::mouseReleaseEvent");
    if (event->button()!=Qt::LeftButton || !_clickDrag)
        return;
//  qDebug("MapFrame::mouseReleaseEvent click");
    _clickDrag = false;

    //交差点選択あり、交差点以外は NULL
    pIntsec = getIntsecAt(_clickDragX, _clickDragY);
    if (pIntsec != NULL)
    {
        changeShowSelect(pIntsec->getIntsecId(), MAP_NOID, MAP_NOID);
        showStatus(event->pos().x(), event->pos().y(), pIntsec->getIntsecId());

        //交差点モードなら編集画面
        if (app.getMainWindow()->getEditMode() == MainWindow::intsec)
        {
            IntsecDialog id(this);
//          qDebug("MapFrame::mouseReleaseEvent intsec dialog (%d, %d)", _intsecDialogX, _intsecDialogY);
            id.init(pIntsec);
            if (_intsecDialogX != DIALOG_NOPOS)
                id.move(_intsecDialogX, _intsecDialogY);
            if (id.exec() == QDialog::Accepted)
            {
                if (id.isDelete())
                {
                    pMap->deleteIntsec(pIntsec->getIntsecId());
                    changeShowSelect(MAP_NOID, MAP_NOID, MAP_NOID, true);
                }
                else
                {
                    if (id.isCenter())
                    {
                        app.setViewMap(app.getViewMapX() - pIntsec->getMapPosX(),
                                       app.getViewMapY() - pIntsec->getMapPosY());
                        if (app.getBackImage())
                            app.setBackMap(app.getBackMapX() - pIntsec->getMapPosX(),
                                           app.getBackMapY() - pIntsec->getMapPosY());
                        pMap->setCenter(pIntsec->getMapPosX(), pIntsec->getMapPosY(), 0);
                    }
                    //ID が変わったらマップで入れ替え、再選択
                    intsecIdNew = id.getIntsecId();
                    if (intsecIdNew != pIntsec->getIntsecId())
                    {
                        pIntsec = pMap->changeIntsecId(pIntsec, intsecIdNew);
                        changeShowSelect(intsecIdNew, MAP_NOID, MAP_NOID, true);
                    }
                    showIntsec(pIntsec);        //全体再描画は遅いので先に書いとく
                }
                repaintAll(true);
            }
//          qDebug("MapFrame::mouseReleaseEvent intsec dialog (%d, %d)", id.x(), id.y());
            _intsecDialogX = id.x();
            _intsecDialogY = id.y();
        }

        //単路／単路付け替えモードならモード切り替え（メインウィンドウから）
        else if (app.getMainWindow()->getEditMode() == MainWindow::road    ||
                 app.getMainWindow()->getEditMode() == MainWindow::roadRepl)
            app.getMainWindow()->setEditMode(MainWindow::intsec);
    }

    //交差点選択なし
    else
    {
        //単路選択あり
        pRoad = getRoadAt(_clickDragX, _clickDragY);
        if (pRoad != NULL)
        {
            roadSelected = false;
            if (pRoad->getRoadId() == _roadIdSelect)
                roadSelected = true;
            intsecIdSmall = pRoad->getIntsec(ROAD_IT_SMALL)->getIntsecId();
            intsecIdLarge = pRoad->getIntsec(ROAD_IT_LARGE)->getIntsecId();
            if (intsecIdLarge == _intsecIdSelect)
                changeShowSelect(intsecIdLarge, intsecIdSmall, pRoad->getRoadId());
            else
                changeShowSelect(intsecIdSmall, intsecIdLarge, pRoad->getRoadId());
            showStatus(event->pos().x(), event->pos().y(), MAP_NOID);

            //交差点モードならモード切り替え（メインウィンドウから）
            if (app.getMainWindow()->getEditMode() == MainWindow::intsec)
                app.getMainWindow()->setEditMode(MainWindow::road);

            //単路モードなら編集画面、単路付け替えモードでも選択済みなら編集画面
            else if (app.getMainWindow()->getEditMode() == MainWindow::road                   ||
                    (app.getMainWindow()->getEditMode() == MainWindow::roadRepl && roadSelected))
            {
                RoadDialog	rd(this);
                rd.init(pRoad);
                if (_roadDialogX != DIALOG_NOPOS)
                    rd.move(_roadDialogX, _roadDialogY);
                if (rd.exec() == QDialog::Accepted)
                {
                    if (rd.isDelete())
                    {
                        pMap->deleteRoad(pRoad->getRoadId());
                        changeShowSelect(MAP_NOID, MAP_NOID, MAP_NOID, true);
                    }
                    else if (rd.isDelIntsec())
                    {
                        Intsec* intsec1 = pRoad->getIntsec(ROAD_IT_SMALL);
                        Intsec* intsec2 = pRoad->getIntsec(ROAD_IT_LARGE);
                        pMap->deleteRoad(pRoad->getRoadId());
                        pMap->deleteIntsec(intsec1->getIntsecId());
                        pMap->deleteIntsec(intsec2->getIntsecId());
                        changeShowSelect(MAP_NOID, MAP_NOID, MAP_NOID, true);
                    }
                    repaintAll(true);
                }
                _roadDialogX = rd.x();
                _roadDialogY = rd.y();
            }
        }

        //単路選択なし
        else
        {
            //交差点モードなら交差点新規作成、選択変更とエラーチェックと表示、ここは全体再描画なし
            if (app.getMainWindow()->getEditMode() == MainWindow::intsec)
            {
                pIntsec = pMap->createIntsec();
                x = getMapX(_clickDragX);
                y = getMapY(_clickDragY);
                if (pIntsec == NULL)
                    QMessageBox::critical(this, TITLE, "Intersection ID is too large.");
                if      (x < APP_POS_MIN || y < APP_POS_MIN )
                    QMessageBox::critical(this, TITLE, "Max position is too small.");
                else if (x > APP_POS_MAX || y > APP_POS_MAX )
                    QMessageBox::critical(this, TITLE, tr("Max position is too large."));
                else
                {
                    pIntsec->setMapPos(x, y, 0);
                    changeShowSelect(pIntsec->getIntsecId(), _intsecIdDest, _roadIdSelect);
                    //　showStatus　なしでも変わるが念のため
                    showStatus(event->pos().x(), event->pos().y(), pIntsec->getIntsecId());
                    pIntsec->checkError();
                    showIntsec(pIntsec);
//                  app.getMap()->debugTrace();
                }
            }
        }
    }
}

//マウス移動イベント（ドラッグ中以外）
void MapFrame::mouseMoveEvent(QMouseEvent *event)
{
    //ステータス表示、マウス移動
    showStatus(event->pos().x(), event->pos().y());
    mouseMove(event->pos().x(), event->pos().y());
}

//ドラッグ開始イベント
void MapFrame::dragEnterEvent(QDragEnterEvent *event)
{
    //自ドラッグ判定、ドラッグ開始
    if (!event->mimeData()->hasFormat(ITEM_MINETYPE) || (event->source() != this))
    {
//      qDebug("MapFrame::dragEnterEvent ignore");
        event->ignore();
        return;
    }
//  qDebug("MapFrame::dragEnterEvent accept");
    event->accept();
}

//ドラッグ移動イベント
void MapFrame::dragMoveEvent(QDragMoveEvent *event)
{
    double  scrollMapX, scrollMapY;

    //ステータス表示、自ドラッグ判定、ドラッグ開始で弾かれるようだが念のため
    showStatus(event->pos().x(), event->pos().y());
    if (!event->mimeData()->hasFormat(ITEM_MINETYPE) || (event->source() != this))
    {
//      qDebug("MapFrame::dragMoveEvent ignore");
        event->ignore();
        return;
    }

    //ドラッグ処理、単路ドラッグ位置変更
    if (_dragType == intsec ||_dragType == road || _dragType == roadRepl)
    {
        _dragRoadX = event->pos().x() - _offsetDragX;
        _dragRoadY = event->pos().y() - _offsetDragY;
//      qDebug("MapFrame::dragMoveEvent (%d, %d)", _dragRoadX, _dragRoadY);
        repaint();
    }
    else if (_dragType == scroll)
    {
        scrollMapX = (double)(event->pos().x() - _clickDragX) / app.getViewMagni();
        scrollMapY = (double)(event->pos().y() - _clickDragY) / app.getViewMagni();
        app.setViewMap(_scrollBeginMapX - scrollMapX, _scrollBeginMapY + scrollMapY);   // Y 軸は逆方向
//      qDebug("MapFrame::dragMoveEvent scroll (%f, %f)", scrollMapX, scrollMapY);
        repaintAll();
    }
//  qDebug("MapFrame::dragMoveEvent accept");
    event->accept();
}

//ドロップイベント
void MapFrame::dropEvent(QDropEvent *event)
{
    Intsec*         pIntsec;
    Intsec*         pIntsecDrop;
    Intsec*         pIntsecReplCon;
    Map*            pMap = app.getMap();
    double          x, y;
    bool            already;
    Road*           pRoad;

    //自ドラッグ判定、ドラッグ開始で弾かれるようだが念のため
    if (!event->mimeData()->hasFormat(ITEM_MINETYPE) || (event->source() != this))
    {
        event->ignore();
        return;
    }

    //スクロール以外なら交差点取得、必ずあるはずだが念のため
    if (_dragType != scroll)
    {
//      qDebug("drop %d", _intsecIdDrag);
        pIntsec = pMap->getIntsec(_intsecIdDrag);
        if (pIntsec == NULL)
        {
            event->ignore();
            return;
        }

        //単路ドラッグ位置設定
        if (_dragType == intsec ||_dragType == road || _dragType == roadRepl)
        {
            _dragRoadX = event->pos().x() - _offsetDragX;
            _dragRoadY = event->pos().y() - _offsetDragY;
        }

        //交差点移動
        if (_dragType == intsec)
        {
            x = getMapX(event->pos().x() - _offsetDragX);
            y = getMapY(event->pos().y() - _offsetDragY);
            if      (x < APP_POS_MIN || y < APP_POS_MIN )
                QMessageBox::critical(this, TITLE, "Max position is too small.");
            else if (x > APP_POS_MAX || y > APP_POS_MAX )
                QMessageBox::critical(this, TITLE, "Max position is too large.");
            else
                pIntsec->setMapPos(x, y, pIntsec->getMapPosZ());
//          pMap->debugTrace();
        }

        //単路作成
        else if (_dragType == road)
        {
            pIntsecDrop = getIntsecAt(_dragRoadX, _dragRoadY);
            if (pIntsecDrop != NULL && pIntsecDrop->getIntsecId() != _intsecIdDrag)
            {
                pRoad = pMap->createRoad(pIntsec, pIntsecDrop, &already);
                if (already)
                    QMessageBox::warning(this, TITLE, "Section is already exist.");
                else
                {
                    if (!app.getMainWindow()->isLane2())
                        pRoad->setLane4();
                    changeShowSelect(_intsecIdDrag, pIntsecDrop->getIntsecId(), pRoad->getRoadId());
                }
//              pMap->debugTrace();
            }
        }

        //単路入れ替え、先頭接続先交差点ID変更、タイミング注意
        else if (_dragType == roadRepl)
        {
            pIntsecDrop = getIntsecAt(_dragRoadX, _dragRoadY);
            if (pIntsecDrop != NULL && pIntsecDrop->getIntsecId() != _intsecIdDrag
                                    && pIntsecDrop->getIntsecId() != _intsecIdReplCon)
            {
                pIntsecReplCon = pMap->getIntsec(_intsecIdReplCon);
                Q_ASSERT(pIntsecReplCon!=NULL);
                if (pMap->getRoad(_intsecIdReplCon, pIntsecDrop->getIntsecId()) != NULL)
                    QMessageBox::warning(this, TITLE, "Section is already exist.");
                else
                {
                    pRoad = pMap->replaceRoad(pIntsec, pIntsecDrop, pIntsecReplCon);
                    if (_intsecIdDrag == _intsecIdSelect)
                        changeShowSelect(pIntsecDrop->getIntsecId(), _intsecIdDest, pRoad->getRoadId());
                    else
                        changeShowSelect(_intsecIdSelect, pIntsecDrop->getIntsecId(), pRoad->getRoadId());
                }
            }
        }
    }

    //ドロップ終了、再描画はドラッグ処理で実行
    event->accept();
}

//ドラッグタイマ、微細移動を可能にするための処理
void  MapFrame::dragTimer()
{
    if (_clickDrag)
        drag();
}

//ドラッグ
void MapFrame::drag()
{
    Map*            pMap = app.getMap();
    IntsecCommon*   pic = pMap->getIntsecCommon();
    Intsec*         pIntsec;
    Intsec*         pIntsecReplCon;
    QPixmap         pixmapDrag;

//  qDebug("MapFrame::drag start");
    _clickDrag = false;

    //交差点ドラッグ開始、なしならスクロール
    _dragType = scroll;
    pIntsec = getIntsecAt(_clickDragX, _clickDragY);
    if (pIntsec != NULL)
    {
        //交差点モードなら交差点移動、表示変更後にフラグ指定でないと画面がずれる
        if (app.getMainWindow()->getEditMode() == MainWindow::intsec)
        {
            changeShowSelect(pIntsec->getIntsecId(), MAP_NOID, MAP_NOID);
            pixmapDrag = *(pIntsec->getPixmapLabel());
            _dragRoadX = getViewX(pIntsec->getMapPosX());
            _dragRoadY = getViewY(pIntsec->getMapPosY());
            _dragType = intsec;
        }

        //単路モードなら単路新規作成、表示変更後にフラグ指定でないと画面がずれる
        else if (app.getMainWindow()->getEditMode() == MainWindow::road)
        {
            changeShowSelect(pIntsec->getIntsecId(), MAP_NOID, MAP_NOID);
            pixmapDrag = *(pic->getLabelPixmap(pic->getLabelPixmapId(INTSEC_LT_DEST)));
            _dragRoadX = _dragRoadConX = getViewX(pIntsec->getMapPosX());
            _dragRoadY = _dragRoadConY = getViewY(pIntsec->getMapPosY());
            _dragType = road;
        }

        //単路付け替えモードなら選択単路を付け替え
        else if (app.getMainWindow()->getEditMode() == MainWindow::roadRepl)
        {
            if (_intsecIdSelect == MAP_NOID || _intsecIdDest == MAP_NOID || _roadIdSelect == MAP_NOID)
                return;
            if     (pIntsec->getIntsecId() == _intsecIdSelect)
                _intsecIdReplCon = _intsecIdDest;
            else if (pIntsec->getIntsecId() == _intsecIdDest)
                _intsecIdReplCon = _intsecIdSelect;
            else
                return;
            pixmapDrag = *(pIntsec->getPixmapLabel());
            pIntsecReplCon = pMap->getIntsec(_intsecIdReplCon);
            _dragRoadX    = getViewX(pIntsec->getMapPosX());
            _dragRoadY    = getViewY(pIntsec->getMapPosY());
            _dragRoadConX = getViewX(pIntsecReplCon->getMapPosX());
            _dragRoadConY = getViewY(pIntsecReplCon->getMapPosY());
            _dragType = roadRepl;
        }
    }

    //ドラッグ情報設定
    if (_dragType == scroll)
    {
        _intsecIdDrag = MAP_NOID;
        _offsetDragX = _offsetDragY = 0;
        _scrollBeginMapX = app.getViewMapX();
        _scrollBeginMapY = app.getViewMapY();
    }
    else
    {
        _intsecIdDrag = pIntsec->getIntsecId();
        _offsetDragX = _clickDragX - getViewX(pIntsec->getMapPosX());
        _offsetDragY = _clickDragY - getViewY(pIntsec->getMapPosY());
//      qDebug("MapFrame::mouseMoveEvent %d (%d,%d) = (%d,%d) - (%d,%d)",
//             _intsecIdDrag, _offsetDragX, _offsetDragY, _clickDragX, _clickDragY,
//             getViewX(pIntsec->getMapPosX()), getViewY(pIntsec->getMapPosY()));
    }
    QByteArray itemData;
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(ITEM_MINETYPE, itemData);

    //ドラッグ中表示設定、 デフォルト pixmap の□は消せない、透明色ファイルを置いても透明にならない
//  qDebug("MapFrame::mouseMoveEvent setPixmap intsecId=%d", pIntsec->getIntsecId());
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    QPoint pointHotSpot;
    if (_dragType == scroll)
    {

#ifdef USE_QT5
        if (!pixmapDrag)
        {
            //pixmapDrag = QPixmap(width(), height());
            pixmapDrag = QPixmap(1, 1);
        }
#endif //USE_QT5

        drag->setPixmap(pixmapDrag);
        drag->setDragCursor(QPixmap(":/icon/scroll_cursor.png"), Qt::MoveAction);
    }
    else
    {
        drag->setPixmap(pixmapDrag);
        pointHotSpot.setX(_clickDragX - pIntsec->getLabelPosX());
        pointHotSpot.setY(_clickDragY - pIntsec->getLabelPosY());
//      qDebug("MapFrame::drag hotspot(%d, %d) = click(%d, %d) - label(%d, %d)",
//             pointHotSpot.x(), pointHotSpot.y(), _clickDragX, _clickDragY,
//             pIntsec->getLabelPosX(), pIntsec->getLabelPosY());
        drag->setHotSpot(pointHotSpot);
        if (pIntsec->getIntsecId() == _intsecIdSelect)
            pIntsec->showLabel(INTSEC_LT_SELECT, true);
        else
            pIntsec->showLabel(INTSEC_LT_DEST,   true);
    }

    //ドラッグ移動／終了、dragオブジェクト削除不可、ドラッグ種別クリア後再描画
    drag->exec(Qt::MoveAction, Qt::MoveAction);
    _dragWholePixmap = QPixmap();
    if (_dragType == scroll)
    {
        _dragType = none;
        unsetCursor();
    }
    else
    {
        _dragType = none;
        showIntsec(pIntsec);        //これがないと一瞬灰色
        repaintAll(true);
    }
//  qDebug("mousepress end %d", childLabel->getIntsecId());
}

//拡大縮小系-------------------------------------------------------

//拡大縮小、変更なしなら false
bool MapFrame::zoomInOut(int numSteps)
{
    int     step;
    double  viewMagni, offsetMapX, offsetMapY;

    viewMagni = app.getViewMagni();
    offsetMapX = (double)width() / viewMagni / 2;
    offsetMapY = (double)height() / viewMagni / 2;
//  qDebug("MapFrame::ZoomInOut step %d magni %f", numSteps, viewMagni);
    if     (numSteps > 0)
    {
        if (viewMagni >= APP_MAGNI_MAX)
            return false;
        for (step = 0; step < numSteps; step++)
            viewMagni += viewMagni * VIEWMAGNI_CHANGE;
        if (viewMagni > APP_MAGNI_MAX)
            viewMagni = APP_MAGNI_MAX;
    }
    else if (numSteps < 0)
    {
        if (viewMagni <= APP_MAGNI_MIN)
            return false;
        numSteps = -numSteps;
        for (step = 0; step < numSteps; step++)
            viewMagni -= viewMagni * VIEWMAGNI_CHANGE;
        if (viewMagni < APP_MAGNI_MIN)
            viewMagni = APP_MAGNI_MIN;
    }
    else
        return false;
    app.setViewMagni(viewMagni);
//  qDebug("MapFrame::ZoomInOut move (%f,%f) - offset (%f,%f) + offset prev (%f,%f)",
//         app.getViewMapX(), app.getViewMapY(),
//         (double)width() / viewMagni / 2, (double)height() / viewMagni / 2,
//         offsetMapX, offsetMapY);
    app.setViewMap(app.getViewMapX() - (double)width() / viewMagni / 2 + offsetMapX,
                   app.getViewMapY() - (double)height() / viewMagni / 2 + offsetMapY);
    repaintAll();
    return true;
}

//ホイールイベント
void MapFrame::wheelEvent(QWheelEvent *event)
{
    int     numSteps;

//  qDebug("MapFrame::wheelEvent %d %d", event->delta(), event->orientation());
    if (event->orientation() != Qt::Vertical)
    {
        event->ignore();
        return;
    }
    numSteps = event->delta() / 120;
    if (!zoomInOut(numSteps))
        event->ignore();
    event->accept();
}

//リサイズイベント
void MapFrame::resizeEvent(QResizeEvent *)
{
    //これだけダイアログが上がっていても入ってしまう。描画のみなので大丈夫そうだが。
//  qDebug("MapFrame::resizeEvent (%d,%d)-(%d,%d)",
//         event->size().width(), event->size().height(), width(), height());
//  app.setViewMap(app.getViewMapX() - getMapX((width() - event->oldSize().width()) / 2),
//                app.getViewMapY() + getMapY((height() - event->oldSize().height()) / 2));
    repaintAll();
}

//描画系-------------------------------------------------------

//背景画像ロード、相対パスだと設定ファイル必須、失敗時は表示させないこと、現状 Windows 不可
bool MapFrame::loadBackImage(const char* pImageFile, const char* pSetupFile, bool* pNoSetupError)
{
    QFileInfo   fileInfo;
    string      imageFile;
    QImage      image;

    if (pNoSetupError != NULL)
        *pNoSetupError = false;
    if (pImageFile[0] == '/')
        imageFile = pImageFile;
    else if (pImageFile[0] != '\0' && pImageFile[1] == ':' )
        imageFile = pImageFile;
    else if (pSetupFile[0] == '\0')
    {
        if (pNoSetupError != NULL)
            *pNoSetupError = true;
        return false;
    }
    else
    {
        fileInfo.setFile(pSetupFile);
        imageFile = fileInfo.absolutePath().toStdString() + "/" + pImageFile;
    }
//  qDebug("MapFrame::loadBackImage %s", pFile);
    if (!image.load(imageFile.c_str()))
        return false;
    _backPixmap = QPixmap::fromImage(image);

    return true;
}

//全体再描画、ラベルを含む、エラーチェック
void MapFrame::repaintAll(bool checkError, bool clearSelect)
{
    Map*                pMap = app.getMap();
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;

//  qDebug("MapFrame::repaintAll");
    if (clearSelect)
        _intsecIdSelect =_intsecIdDest =_roadIdSelect = MAP_NOID;
    pIntsec = pMap->nextIntsec(&imi, true);
    while (pIntsec != NULL)
    {
        if (checkError)
            pIntsec->checkError();
        showIntsec(pIntsec);
        pIntsec = pMap->nextIntsec(&imi);
    }
    if (checkError)
    {
        pRoad = pMap->nextRoad(&rmi, true);
        while (pRoad != NULL)
        {
            pRoad->checkError();
            pRoad = pMap->nextRoad(&rmi);
        }
    }
    repaint();
}

//ステータス表示、ID ありなら表示
void MapFrame::showStatus(int viewX, int viewY, int intsecId)
{
    stringstream ss;

    ss << "(" << (int)getMapX(viewX) << "," << (int)getMapY(viewY) << ")";
    //広域は選択しないと交差点IDが取れないので常に外す
    if (intsecId != MAP_NOID &&
        app.getMainWindow()->getEditMode() != MainWindow::large)
        ss << " ID " << intsecId;
    if (_intsecIdSelect != MAP_NOID)
    {
        Road *road = NULL;
        if (_intsecIdDest != MAP_NOID)
            road = app.getMap()->getRoad(_intsecIdSelect, _intsecIdDest);
        if (road == NULL)
            ss << " Select ID " << _intsecIdSelect;
        else
        {
            int itSelect = road->getIntsecType(_intsecIdSelect);
            int itDest   = road->getIntsecType(_intsecIdDest);
            ss << " Select ID (Lane) " << _intsecIdSelect
               << " ("                 << road->getLane(itSelect, ROAD_LT_IN)
               << ") -> "              << _intsecIdDest
               << " ("                 << road->getLane(itDest,   ROAD_LT_OUT)
               << ") / "               << _intsecIdDest
               << " ("                 << road->getLane(itDest,   ROAD_LT_IN)
               << ") -> "              << _intsecIdSelect
               << " ("                 << road->getLane(itSelect, ROAD_LT_OUT)
               << ") Distance "        << (int)road->getDistance();
        }
    }
    app.getMainWindow()->statusBar()->showMessage(ss.str().c_str());
}

//選択変更と表示、再描画なし指定可能
void MapFrame::changeShowSelect(int intsecIdSelect, int intsecIdDest, int roadIdSelect, bool noRepaint)
{
    Map*    pMap = app.getMap();
    int     intsecIdPrev;

    //ここでメモリに入れる
    app.shMem().set(intsecIdSelect, intsecIdDest);

    if (intsecIdSelect != _intsecIdSelect)
    {
        intsecIdPrev    = _intsecIdSelect;
        _intsecIdSelect = intsecIdSelect;
        if (intsecIdPrev    != MAP_NOID)
            showIntsec(pMap->getIntsec(intsecIdPrev));
        if (_intsecIdSelect != MAP_NOID)
            showIntsec(pMap->getIntsec(_intsecIdSelect));
    }
    if (intsecIdDest != _intsecIdDest)
    {
        intsecIdPrev  = _intsecIdDest;
        _intsecIdDest = intsecIdDest;
        if (intsecIdPrev    != MAP_NOID)
            showIntsec(pMap->getIntsec(intsecIdPrev));
        if (_intsecIdDest   != MAP_NOID)
            showIntsec(pMap->getIntsec(_intsecIdDest));
        _intsecIdDest = intsecIdDest;
    }
    if (roadIdSelect != _roadIdSelect)
        _roadIdSelect = roadIdSelect;
    if (!noRepaint)
        repaint();
}

//描画イベント
void MapFrame::paintEvent(QPaintEvent*)
{
    QPainter            painter(this), painterDRP;
    Map*                pMap = app.getMap();
    int                 intsecIdCon;
    Intsec*             pIntsec;
    Intsec*             pIntsecCon;
    Road*               pRoad;
    INTSEC_ROADMAP_IT   irmi;

//  qDebug("MapFrame::paintEvent (%d,%d)-(%d,%d)",
//         event->rect().right(), event->rect().top(), event->rect().left(), event->rect().bottom());
//  qDebug("MapFrame::paintEvent back pixmap (%d,%d)-%d",
//         _backPixmap.size().width(), _backPixmap.size().height(), _backPixmap.isNull());

    //交差点ドラッグなら接続単路を書く、単路ドラッグならドラッグ線を描く、他はそのまま描画
    //　通常画面を全体画像に入れてその上にドラッグ線を描く、そうでないと数が多い時に非常に遅い
    if (_dragType == intsec || _dragType == road || _dragType == roadRepl)
    {
//      qDebug("MapFrame::paintEvent _dragWholePixmap (%d,%d)-%d",
//             _dragWholePixmap.width(), _dragWholePixmap.height(), _dragWholePixmap.isNull());
        if (_dragWholePixmap.isNull())
        {
//          qDebug("MapFrame::paintEvent create whole pixmap");
            _dragWholePixmap = QPixmap(width(), height());
            painterDRP.begin(&_dragWholePixmap);
            paint(&painterDRP);
            painterDRP.end();
        }
        painter.drawPixmap(0, 0, width(), height(), _dragWholePixmap);
        if (_dragType == intsec)
        {
//          qDebug("MapFrame::paintEvent drag intsec %d", _intsecIdDrag);
            pIntsec = pMap->getIntsec(_intsecIdDrag);
            Q_ASSERT(pIntsec != NULL);
            pRoad = pIntsec->nextRoad(&irmi, true, &intsecIdCon);
            while (pRoad != NULL)
            {
//              qDebug("MapFrame::paintEvent drag intsec road %d-%d", _intsecIdDrag, intsecIdCon);
                pIntsecCon = pMap->getIntsec(intsecIdCon);
                Q_ASSERT(pIntsecCon != NULL);
                drawLine(&painter, _dragRoadX, _dragRoadY,
                         getViewX(pIntsecCon->getMapPosX()),
                         getViewY(pIntsecCon->getMapPosY()), selected,
                         pRoad->isLaneOver4(), pRoad->isError());
                pRoad = pIntsec->nextRoad(&irmi, false, &intsecIdCon);
            }
        }
        else if (_dragType == road)
        {
//          qDebug("MapFrame::paintEvent road line (%d, %d)-(%d, %d)",
//                 _dragRoadX, _dragRoadY, _dragRoadConX, _dragRoadConY);
            drawLine(&painter, _dragRoadX,    _dragRoadY,
                     _dragRoadConX, _dragRoadConY, selected, !app.getMainWindow()->isLane2());
        }
        else if (_dragType == roadRepl)
        {
            pRoad = pMap->getRoad(_intsecIdDrag, _intsecIdReplCon);
//          qDebug("MapFrame::paintEvent road repl %d-%d", _intsecIdDrag, _intsecIdReplCon);
            if (pRoad != NULL)
            {
//              qDebug("MapFrame::paintEvent road repl line (%d, %d)-(%d, %d)",
//                     _dragRoadX, _dragRoadY, _dragRoadConX, _dragRoadConY);
                drawLine(&painter, _dragRoadX,    _dragRoadY,
                         _dragRoadConX, _dragRoadConY, selected,
                         pRoad->isLaneOver4(), pRoad->isError());
            }
            else
            {
//              qDebug("MapFrame::paintEvent road repl null line (%d, %d)-(%d, %d)",
//                     _dragRoadX, _dragRoadY, _dragRoadConX, _dragRoadConY);
                drawLine(&painter, _dragRoadX,    _dragRoadY,
                         _dragRoadConX, _dragRoadConY, selected);       //最後に一瞬だけある
            }
        }
    }
    else
    {
//      qDebug("MapFrame::paintEvent normal");
        paint(&painter);
    }
}

//交差点表示、選択等の判定、ID表示はここでない
void MapFrame::showIntsec(Intsec* pIntsec)
{
    int     intsecId;

    //削除時のため NULL なら無視
    if (pIntsec == NULL)
        return;
    if (!isViewArea(pIntsec) || app.getMainWindow()->getEditMode() == MainWindow::large)
    {
//      qDebug("hide label %d", pIntsec->getIntsecId());
        pIntsec->hideLabel();
    }
    else
    {
//      qDebug("show label %d", pIntsec->getIntsecId());
        intsecId = pIntsec->getIntsecId();
        if      (intsecId == _intsecIdSelect)
            pIntsec->showLabel(INTSEC_LT_SELECT);
        else if (intsecId == _intsecIdDest)
            pIntsec->showLabel(INTSEC_LT_DEST);
        else
            pIntsec->showLabel(INTSEC_LT_NORMAL);
    }
}

//表示エリア内チェック
bool MapFrame::isViewArea(Intsec* pIntsec)
{
    int     viewX, viewY;

    if (pIntsec == NULL)
        return false;
    viewX = getViewX(pIntsec->getMapPosX());
    viewY = getViewY(pIntsec->getMapPosY());
    if      (viewX < 0 - width() / 2)
        return false;
    else if (viewY < 0 - height() / 2)
        return false;
    else if (viewX > width() * 3 / 2)
        return false;
    else if (viewY > height() * 3 / 2)
        return false;
    return true;
}

//描画
void MapFrame::paint(QPainter* pp)
{
    Map*                pMap = app.getMap();
    QRect               rectSource, rectDest;
    double              viewBackMagni;
    MAP_INTSECMAP_IT    imi;
    Intsec*             pIntsec;
    stringstream        ss;
    int                 viewX, viewY;
    MAP_ROADMAP_IT      rmi;
    Road*               pRoad;
    DrawLineType        drawLineType;

    //塗りつぶし、ドラッグ画面用
    pp->fillRect(0, 0, width(), height(), Qt::gray);

    //背景描画
    if (app.getBackImage() && !_backPixmap.isNull())
    {
        viewBackMagni = app.getBackMagni() * app.getViewMagni();    // view / back
        rectDest.setCoords(0, 0, width(), height());
        rectSource.setLeft(  (int)((double)(app.getViewMapX() - app.getBackMapX()) / app.getBackMagni()));
        rectSource.setBottom(_backPixmap.size().height() -
                             (int)((double)(app.getViewMapY() - app.getBackMapY()) / app.getBackMagni()));
        rectSource.setRight( rectSource.left()   + (int)((double)width() / viewBackMagni));
        rectSource.setTop(  rectSource.bottom() - (int)((double)height() / viewBackMagni));
        pp->drawPixmap(rectDest, _backPixmap, rectSource);
    }

    //全単路描画
    pMap = app.getMap();
    pRoad = pMap->nextRoad(&rmi, true);
    while (pRoad != NULL)
    {
        if (pRoad->getRoadId() == _roadIdSelect)
        {
            if (_dragType == roadRepl)
            {
//                  qDebug("MapFrame::paint drag road replace selected");
                drawLineType = selectedDark;
            }
            else
                drawLineType = selected;
        }
        else
            drawLineType = normal;
        if (_dragType == intsec)
        {
//              qDebug("MapFrame::paint drag intsec road");
            if (pRoad->getIntsec(ROAD_IT_SMALL)->getIntsecId() == _intsecIdDrag ||
                pRoad->getIntsec(ROAD_IT_LARGE)->getIntsecId() == _intsecIdDrag )
            {
//                  qDebug("MapFrame::paint drag intsec road selected");
                drawLineType = selectedDark;
            }
        }
//          qDebug("MapFrame::paint road (%d,%d) drag %d lineType:%d",
//                 pRoad->getIntsec(ROAD_IT_SMALL)->getIntsecId(),
//                 pRoad->getIntsec(ROAD_IT_LARGE)->getIntsecId(), _intsecIdDrag, drawLineType);
        if (isViewArea(pRoad->getIntsec(ROAD_IT_SMALL)) || isViewArea(pRoad->getIntsec(ROAD_IT_LARGE)))
            drawLine(pp, getViewX(pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosX()),
                         getViewY(pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosY()),
                         getViewX(pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosX()),
                         getViewY(pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosY()),
                         drawLineType, pRoad->isLaneOver4(), pRoad->isError());
        pRoad = pMap->nextRoad(&rmi);
    }

    //交差点ID描画、広域表示では点描画
    if (app.getShowId())
    {
        pIntsec = pMap->nextIntsec(&imi, true);
        while (pIntsec != NULL)
        {
            if (isViewArea(pIntsec))
            {
                if (app.getMainWindow()->getEditMode() != MainWindow::large)
                {
                    ss.str("");
                    ss << pIntsec->getIntsecId();
                    viewX = getViewX(pIntsec->getMapPosX()) + INTSEC_LABELSIZE_H + 2;
                    viewY = getViewY(pIntsec->getMapPosY()) + INTSEC_LABELSIZE_H + 2;
                    pp->setPen(QColor("#FFFFFF"));
                    pp->drawText(viewX - 1, viewY + 1, ss.str().c_str());
                    pp->drawText(viewX - 1, viewY - 1, ss.str().c_str());
                    pp->drawText(viewX + 1, viewY - 1, ss.str().c_str());
                    pp->drawText(viewX + 1, viewY + 1, ss.str().c_str());
                    pp->setPen(QColor("#0000FF"));
                    pp->drawText(viewX,     viewY,     ss.str().c_str());
                }
                else
                {
                    viewX = getViewX(pIntsec->getMapPosX()) - 2;
                    viewY = getViewY(pIntsec->getMapPosY()) - 2;
                    QString colorName;
                    if (pIntsec->getIntsecId() == _intsecIdSelect)
                    {
                        if (pIntsec->isError())
                            colorName = "#FF00FF";
                        else
                            colorName = "#0000FF";
                    }
                    else
                    {
                        if (pIntsec->isError())
                            colorName = "#FF0000";
                        else
                            colorName = "#000000";
                    }
                    //何故かこうしないと描けない
                    QBrush brush(QColor(colorName.toStdString().c_str()));
                    pp->setPen(QColor(colorName));
                    pp->fillRect(viewX, viewY, 4, 4, brush);
                    pp->drawRect(viewX, viewY, 4, 4);
                }
            }
            pIntsec = pMap->nextIntsec(&imi);
        }
    }
}

//直線描画
void MapFrame::drawLine(QPainter* pp, int viewX1, int viewY1, int viewX2, int viewY2,
                        DrawLineType drawLineType, bool bold, bool error)
{
    QPen        pen;

    if (!app.getClearView())
    {
        if (!bold)
            pen.setWidth(2);
        else
            pen.setWidth(3);
    }
    else
        pen.setWidth(1);
    if (app.getMainWindow()->getEditMode() != MainWindow::large)
    {
        if      (drawLineType == normal)
            setPenColor(pen, Qt::white, QColor("#FFC0C0"), error);
        else if (drawLineType == selected)
            setPenColor(pen, QColor("#80FFFF"), QColor("#FFC0FF"), error);
        else
            setPenColor(pen, QColor("#60C0C0"), QColor("#C0C0C0"), error);
        pp->setPen(pen);
        if (abs(viewX1 - viewX2) > abs(viewY1 - viewY2))
            pp->drawLine(viewX1,   viewY1-1, viewX2,   viewY2-1);
        else if ((viewX1 - viewX2) * (viewY1 - viewY2) <= 0)
            pp->drawLine(viewX1-1, viewY1,   viewX2-1, viewY2);
        else
            pp->drawLine(viewX1+1, viewY1,   viewX2+1, viewY2);
    }
    if      (drawLineType == normal)
        setPenColor(pen, Qt::black, QColor("#FF0000"), error);
    else if (drawLineType == selected)
        setPenColor(pen, QColor("#0000FF"), QColor("#FF00FF"), error);
    else
        setPenColor(pen, QColor("#0000C0"), QColor("#C000C0"), error);
    pp->setPen(pen);
    pp->drawLine(viewX1, viewY1, viewX2, viewY2);
}

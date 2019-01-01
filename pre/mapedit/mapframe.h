/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//マップフレーム

#ifndef MAPFRAME_H
#define MAPFRAME_H

#include <QFrame>
#include <QPicture>
#include <QTime>
#include "map.h"

#define QT_FATAL_ASSERT

class Intsec;
class Road;

class MapFrame : public QFrame
{
    Q_OBJECT                                            //シグナル／スロット用
    QPixmap     _backPixmap;                            //背景イメージ（ファイル）
    QPixmap     _dragWholePixmap;                       //ドラッグ全体画像（ドラッグ線以外）
    int         _intsecIdSelect;                        //選択／単路元交差点ID
    int         _intsecIdDest;                          //単路先交差点ID
    int         _roadIdSelect;                          //選択単路ID
    bool        _clickDrag;                             //クリック／ドラッグ（未確定状態）
    int         _clickDragX, _clickDragY;               //クリック／ドラッグ位置
    enum DragType {none, intsec, road, roadRepl, scroll};
    DragType    _dragType;                              //ドラッグ種別
    int         _intsecIdDrag;                          //ドラッグ交差点ID
    int         _offsetDragX, _offsetDragY;             //ドラッグオフセット
    double      _scrollBeginMapX, _scrollBeginMapY;     //スクロール開始マップ位置
    int         _dragRoadX, _dragRoadY;                 //単路ドラッグ位置、表示位置、移動先、交差点移動時も使用
    int         _dragRoadConX, _dragRoadConY;           //単路ドラッグ接続先位置、表示位置
    int         _intsecIdReplCon;                       //単路入れ替え接続先交差点ID
    int         _intsecDialogX, _intsecDialogY, _roadDialogX, _roadDialogY;
                                                        //編集画面位置

public:
    MapFrame(QWidget *parent=0);                        //コンストラクタ
    int         getViewX(double mapX);                  //マップ位置から表示位置取得
    int         getViewY(double mapY);
    double      getMapX(int viewX);                     //表示位置からマップ位置取得
    double      getMapY(int viewY);
    void        changeEditMode();                       //編集モード変更
private:
    Intsec*     getIntsecAt(int viewX, int viewY);      //特定位置の交差点取得、なしなら NULL
    Road*       getRoadAt(int viewX, int viewY);        //特定位置の道路取得、なしなら NULL

    //操作系
public:
    void        mouseMove(int viewX, int viewY);        //マウス移動
protected:
    void        mousePressEvent(QMouseEvent *event);    //マウス押下イベント
    void        mouseReleaseEvent(QMouseEvent *event);  //マウス開放イベント
    void        mouseMoveEvent(QMouseEvent *event);     //マウス移動イベント（ドラッグ中以外）
    void        dragEnterEvent(QDragEnterEvent *event); //ドラッグ開始イベント
    void        dragMoveEvent(QDragMoveEvent *event);   //ドラッグ移動イベント
    void        dropEvent(QDropEvent *event);           //ドロップイベント
private slots:
    void        dragTimer();                            //ドラッグタイマ
private:
    void        drag();                                 //ドラッグ

    //拡大縮小系
public:
    bool        zoomInOut(int numSteps);                //拡大縮小、正なら拡大、変更なしなら false
protected:
    void        wheelEvent(QWheelEvent *event);         //ホイールイベント
    void        resizeEvent(QResizeEvent *event);       //リサイズイベント

    //描画系
public:
    bool        loadBackImage(const char* pImageFile, const char* pSetupFile, bool* pNoSetupError=NULL);
                                                        //背景画像ロード、相対パスだと設定ファイル必須
                                                        //　失敗時は表示させないこと
    void        repaintAll(bool checkError = false, bool clearSelect = false);
                                                        //全体再描画、背景やラベルを含む、エラーチェック
    void        showStatus(int viewX, int viewY, int intsecId = MAP_NOID);
                                                        //ステータス表示、ID ありなら表示
    void        changeShowSelect(int intsecIdSelect, int intsecIdDest, int roadIdSelect,
                                 bool noRepaint = false);
                                                        //選択変更と表示、再描画なし指定可能
protected:
    void        paintEvent(QPaintEvent *event);         //描画イベント
private:
    void        showIntsec(Intsec* pIntsec);            //交差点表示、選択等の判定、ID表示はここでない
    bool        isViewArea(Intsec* pIntsec);            //表示エリア内チェック
    void        paint(QPainter* pp);                    //描画
    enum DrawLineType {normal, selected, selectedDark};
    void        drawLine(QPainter* pp, int viewX1, int viewY1, int viewX2, int viewY2,
                         DrawLineType drawLineType=normal, bool bold=false, bool error=false);
                                                        //直線描画
    void        setPenColor(QPen &pen, const QColor &color, const QColor &colorError, bool error)
                                                        //ペン色設定
    {
        if (!error)
            pen.setColor(color);
        else
            pen.setColor(colorError);
    }
};

#endif // MAPFRAME_H

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//単路編集画面

#ifndef ROADDIALOG_H
#define ROADDIALOG_H

#include <QDialog>

#define QT_FATAL_ASSERT

class IntsecCommon;
class Intsec;
class Road;

//ユーザインターフェイス
namespace Ui {
    class RoadDialog;
}

//単路編集画面
class RoadDialog : public QDialog
{
    Q_OBJECT                                    //シグナル／スロット用
    Ui::RoadDialog* _ui;                        //ユーザインターフェイス
    Road*           _pRoad;                     //単路
    int             _roadIntsecType1, _roadIntsecType2;
                                                //単路交差点種別（上下）
    bool            _delete;                    //削除
    bool            _delIntsec;                 //交差点ごと削除

public:
    RoadDialog(QWidget *parent = 0);            //コンストラクタ
    ~RoadDialog();                              //デストラクタ
    void init(Road* pRoad);                     //初期処理
    bool isDelete()                             //削除判定
    {
        return _delete;
    }
    bool isDelIntsec()                          //交差点ごと削除判定
    {
        return _delIntsec;
    }
    protected:
    void changeEvent(QEvent *e);                //変換イベント

private slots:
    void on_radioLane2_clicked();               //往復2車線
    void on_radioLane4_clicked();               //往復4車線
    void on_radioLaneOther_clicked();           //その他車線
    void on_buttonDelete_clicked();             //削除ボタン
    void on_buttonDelIntsec_clicked();          //交差点ごと削除ボタン
    void on_buttonVerify_clicked();             //検証ボタン
    void on_buttonOk_clicked();                 //OKボタン
    void on_buttonCancel_clicked();             //キャンセルボタン
private:
    enum laneNumType {lane2, lane4, laneOther};
    void setLaneNumType(laneNumType lnt);       //車線数種別設定
    void getViewData();                         //表示データ取得
};

#endif // ROADDIALOG_H

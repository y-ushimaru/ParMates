/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点編集画面

#ifndef INTSECDIALOG_H
#define INTSECDIALOG_H

#include <QDialog>
#include <QResizeEvent>

#define QT_FATAL_ASSERT

class Intsec;

//ユーザインターフェイス
namespace Ui {
    class IntsecDialog;
}

//交差点編集画面
class IntsecDialog : public QDialog
{
    Q_OBJECT                                        //シグナル／スロット用
    Ui::IntsecDialog*   _ui;                        //ユーザインターフェイス
    Intsec*             _pIntsec;                   //交差点
    int                 _intsecId;                  //交差点ID
    bool                _center;                    //中心座標
    bool                _delete;                    //削除

public:
    IntsecDialog(QWidget *parent = 0);              //コンストラクタ
    ~IntsecDialog();                                //デストラクタ
    void init(Intsec* pIntsec);                     //初期処理
    bool isCenter()                                 //中心座標判定
    {
        return _center;
    }
    bool isDelete()                                 //削除判定
    {
        return _delete;
    }
    int getIntsecId()                               //交差点ID取得
    {
        return _intsecId;
    }
protected:
    void changeEvent(QEvent *e);                    //変換イベント
private slots:
    void on_buttonCenter_clicked();                 //中心座標ボタン
    void on_buttonDelete_clicked();                 //削除ボタン
    void on_buttonVerify_clicked();                 //検証ボタン
    void on_buttonOk_clicked();                     //OKボタン
    void on_buttonCancel_clicked();                 //キャンセルボタン
private:
    void getViewData(bool getIntsecId=true);        //表示データ取得
};

#endif // INTSECDIALOG_H

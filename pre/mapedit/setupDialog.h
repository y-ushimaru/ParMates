/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//設定画面

#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <string>

#define QT_FATAL_ASSERT

using namespace std;

//ユーザインターフェイス
namespace Ui {
    class SetupDialog;
}

//設定画面
class SetupDialog : public QDialog
{
    Q_OBJECT                                            //シグナル／スロット用
    Ui::SetupDialog*	_ui;                                //ユーザインターフェイス
public:
    SetupDialog(QWidget *parent = 0);                   //コンストラクタ
    ~SetupDialog();                                     //デストラクタ
    void        init();                                 //初期処理

protected:
    void        changeEvent(QEvent *e);                 //変換イベント

private:
    void        setEnableBackImageInfo();               //背景画像情報可能設定

private slots:
    void        on_checkBackImage_clicked();            //背景画像チェックボックス
    void        on_buttonBackFile_clicked();            //背景画像ファイルボタン
    void        on_buttonOk_clicked();                  //OKボタン
    void        on_buttonCancel_clicked();              //キャンセルボタン
};

#endif // SETUPDIALOG_H

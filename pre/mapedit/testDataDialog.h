/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//テストデータ画面

#ifndef TESTDATADIALOG_H
#define TESTDATADIALOG_H

#include <QDialog>

//ユーザインターフェイス
namespace Ui {
    class TestDataDialog;
}

//テストデータ画面
class TestDataDialog : public QDialog
{
    Q_OBJECT                                            //シグナル／スロット用
    Ui::TestDataDialog *_ui;                            //ユーザインターフェイス
public:
    explicit TestDataDialog(QWidget *parent = 0);      //コンストラクタ
    ~TestDataDialog();                                  //デストラクタ
    void        init();                                 //初期処理
private slots:
    void on_buttonOk_clicked();
    void on_buttonCancel_clicked();
};

#endif // TESTDATADIALOG_H











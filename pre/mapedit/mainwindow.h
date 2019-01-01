/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//メインウィンドウ

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define QT_FATAL_ASSERT

//ユーザインターフェイス
namespace Ui {
    class MainWindow;
}

//メインウィンドウ
class MainWindow : public QMainWindow
{
    Q_OBJECT                                                    //シグナル／スロット用
    Ui::MainWindow* _ui;                                        //ユーザインターフェイス
public:
    enum EditMode {select, large, intsec, road, roadRepl};
private:
    EditMode        _editMode;                                  //編集モード
    bool            _lane2;                                     //往復2車線(1+1、新規単路）

public:
    MainWindow(QWidget *parent = 0);                            //コンストラクタ
    ~MainWindow();                                              //デストラクタ
    void            init();                                     //初期処理
protected:
    void            changeEvent(QEvent *e);                     //変換イベント
    void            closeEvent(QCloseEvent *e);                 //クローズイベント
public:
    void            fileOpen(const char* path = NULL);          //ファイルオープン
    EditMode        getEditMode()                               //編集モード取得
    {
        return _editMode;
    }
    void            setEditMode(EditMode em);                   //編集モード設定
    bool            isLane2()                                   //往復2車線確認(1+1、新規単路）
    {
//      qDebug("MainWindow::isLane2 %d", _lane2);
        return _lane2;
    }

private slots:
    void            contextMenuEvent(QContextMenuEvent *event); //右クリックメニューイベント
    void            on_actionFileNew_triggered();               //ファイル新規
    void            on_actionFileOpen_triggered();              //ファイルオープン
    void            on_actionFileSave_triggered();              //ファイル保存
    void            on_actionFileSaveAs_triggered();            //ファイル別名保存
    void            on_actionFileOption_triggered();            //ファイル設定
    void            on_actionFileTestData_triggered();          //ファイルテストデータ
    void            on_actionAbout_triggered();                 //マップファイルエディタについて
    void            on_actionExit_triggered();                  //終了
    void            on_actionSelect_triggered();                //選択
    void            on_actionLarge_triggered();                 //広域表示
    void            on_actionIntsec_triggered();                //交差点
    void            on_actionRoad_triggered();                  //単路
    void            on_actionRoadRepl_triggered();              //単路付け替え
    void            on_actionLane2_triggered();                 //往復2車線
    void            on_actionLane4_triggered();                 //往復4車線
    void            on_actionZoomOut_triggered();               //拡大
    void            on_actionZoomIn_triggered();                //縮小
    void            on_actionDirJump_triggered();               //指定ジャンプ
    void            on_actionAllInfo_triggered();               //全体情報

private:
    void            setTitle();                                 //タイトル設定
    void            fileSave(bool saveAs);                      //ファイル保存（共通）
    void            createTestData();                           //テストデータ作成
};

#endif // MAINWINDOW_H

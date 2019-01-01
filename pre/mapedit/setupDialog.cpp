/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//設定画面
// OK 時にデータを App に入れること

#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include <sstream>
#include "app.h"
#include "mapframe.h"
#include "setupDialog.h"
#include "ui_setupDialog.h"

using namespace std;

#define TITLE	"Setting"

//コンストラクタ
SetupDialog::SetupDialog(QWidget *parent) : QDialog(parent), _ui(new Ui::SetupDialog)
{
    _ui->setupUi(this);
}

//デストラクタ
SetupDialog::~SetupDialog()
{
    delete _ui;
}

//初期処理
void SetupDialog::init()
{
    string      backFile;

    _ui->dbSpinViewMapX->setValue(app.getViewMapX());
    _ui->dbSpinViewMapY->setValue(app.getViewMapY());
    _ui->dbSpinViewMagni->setValue(app.getViewMagni());
    if (app.getClearView())
        _ui->checkClearView->setChecked(Qt::Checked);
    else
        _ui->checkClearView->setChecked(Qt::Unchecked);
    if (app.getShowId())
        _ui->checkShowId->setChecked(Qt::Checked);
    else
        _ui->checkShowId->setChecked(Qt::Unchecked);
    if (app.getBackImage())
        _ui->checkBackImage->setChecked(Qt::Checked);
    else
        _ui->checkBackImage->setChecked(Qt::Unchecked);
    app.getBackFile(&backFile);
    _ui->lineEditBackFile->setText(backFile.c_str());
    _ui->lineEditBackFile->setToolTip(backFile.c_str());
    _ui->dbSpinBackMapX->setValue(app.getBackMapX());
    _ui->dbSpinBackMapY->setValue(app.getBackMapY());
    _ui->dbSpinBackMagni->setValue(app.getBackMagni());
    setEnableBackImageInfo();
}

//変換イベント
void SetupDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//背景画像情報可能設定
void	 SetupDialog::setEnableBackImageInfo()
{
    bool        enable;

    enable = _ui->checkBackImage->isChecked();
    _ui->lineEditBackFile->setEnabled(enable);
    _ui->buttonBackFile->setEnabled(enable);
    _ui->dbSpinBackMapX->setEnabled(enable);
    _ui->dbSpinBackMapY->setEnabled(enable);
    _ui->dbSpinBackMagni->setEnabled(enable);
}

//背景画像チェックボックス
void SetupDialog::on_checkBackImage_clicked()
{
    setEnableBackImageInfo();
}

//背景画像ファイルボタン
void SetupDialog::on_buttonBackFile_clicked()
{
    string      backFile;

    backFile = _ui->lineEditBackFile->text().toStdString();
    if (!app.backFileDialog(this, &backFile))
        return;
    _ui->lineEditBackFile->setText(backFile.c_str());
    _ui->lineEditBackFile->setToolTip(backFile.c_str());
}

//OKボタン
void SetupDialog::on_buttonOk_clicked()
{
    string  file;
    string  setupFile;
    bool    noSetupFile;

    if (_ui->checkBackImage->isChecked())
    {
        app.getSetupFile(&setupFile);
        if (!app.getMapFrame()->loadBackImage(_ui->lineEditBackFile->text().toStdString().c_str(),
                                              setupFile.c_str(), &noSetupFile))
        {
            if (noSetupFile)
                QMessageBox::critical(this, TITLE, "Confirm config file.");
            else
                QMessageBox::critical(this, TITLE, "Cannot load image file.");
            return;
        }
    }

    app.setViewMap(_ui->dbSpinViewMapX->value(), _ui->dbSpinViewMapY->value());
    app.setViewMagni(_ui->dbSpinViewMagni->value());
    app.setClearVivew(_ui->checkClearView->isChecked());
    app.setShowId(_ui->checkShowId->isChecked());
    app.setBackImage(_ui->checkBackImage->isChecked());
    if (_ui->checkBackImage->isChecked())
    {
        app.setBackFile(_ui->lineEditBackFile->text().toStdString().c_str());
        app.setBackMap(_ui->dbSpinBackMapX->value(), _ui->dbSpinBackMapY->value());
        app.setBackMagni(_ui->dbSpinBackMagni->value());
    }
    done(QDialog::Accepted);
}

//キャンセルボタン
void SetupDialog::on_buttonCancel_clicked()
{
    done(QDialog::Rejected);
}

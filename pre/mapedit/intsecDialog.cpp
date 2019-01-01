/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//交差点編集画面

#include <sstream>
#include "ui_intsecDialog.h"
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "intsecDialog.h"

#define TITLE	"Edit intersection"

//コンストラクタ
IntsecDialog::IntsecDialog(QWidget *parent) : QDialog(parent), _ui(new Ui::IntsecDialog)
{
    _ui->setupUi(this);
}

//デストラクタ
IntsecDialog::~IntsecDialog()
{
    delete _ui;
}

//初期処理
void IntsecDialog::init(Intsec* pIntsec)
{
    INTSEC_ANGLEMAP_IT  iami;
    Road*               pRoad;
    int                 index, intsecIdCon;
    stringstream        ss;

    _pIntsec = pIntsec;
    _intsecId = pIntsec->getIntsecId();
    _center = _delete = false;
    _ui->spinBoxId->setValue(_intsecId);
    _ui->spinBoxX->setValue(_pIntsec->getMapPosX());
    _ui->spinBoxY->setValue(_pIntsec->getMapPosY());
    _ui->spinBoxZ->setValue(_pIntsec->getMapPosZ());

    //右揃え不能、仕方ないか
    index = 0;
    pRoad = _pIntsec->nextRoadByAngle(&iami, true, &intsecIdCon);
    while (pRoad != NULL)
    {
        ss.str("");
        ss << intsecIdCon;
        _ui->comboFirstIntsec->insertItem(index, QString(ss.str().c_str()), QVariant(intsecIdCon));
        if (_pIntsec->getFirstIntsecIdCon() == intsecIdCon)
            _ui->comboFirstIntsec->setCurrentIndex(index);
        pRoad = _pIntsec->nextRoadByAngle(&iami, false, &intsecIdCon);
        index++;
    }
}

//変換イベント
void IntsecDialog::changeEvent(QEvent *e)
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

//中心座標ボタン、表示データ取得はやらない
void IntsecDialog::on_buttonCenter_clicked()
{
    int     intsecIdSave;

    if (QMessageBox::question(this, TITLE, "Set this intersection to (0, 0, 0)?",
                              QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        intsecIdSave = _intsecId;
        getViewData();
        if (_intsecId != _pIntsec->getIntsecId() && app.getMap()->getIntsec(_intsecId) != NULL)
        {
            QMessageBox::warning(this, TITLE, "This ID is already used by existing intersection.");
            _intsecId = intsecIdSave;
            return;
        }
        _center = true;
        done(QDialog::Accepted);
    }
}

//削除ボタン
void IntsecDialog::on_buttonDelete_clicked()
{
    if (QMessageBox::question(this, TITLE, "Delete this intersection?",
                              QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        _delete = true;
        done(QDialog::Accepted);
    }
}

//検証ボタン
void IntsecDialog::on_buttonVerify_clicked()
{
    Intsec  intsecSave(_pIntsec->getIntsecId());
    string  message;

    intsecSave.copyMapPos(_pIntsec);
    intsecSave.setFirstIntsecIdCon(_pIntsec->getFirstIntsecIdCon());
    getViewData(false);
    if (!_pIntsec->checkError())
        QMessageBox::information(this, TITLE, "This intersection is valid.");
    else
    {
        _pIntsec->getErrorMessage(&message);
        QMessageBox::warning(this, TITLE, message.c_str());
    }
    _pIntsec->copyMapPos(&intsecSave);
    _pIntsec->setFirstIntsecIdCon(intsecSave.getFirstIntsecIdCon());
}

//OKボタン
void IntsecDialog::on_buttonOk_clicked()
{
    int     intsecIdSave;

    intsecIdSave = _intsecId;
    getViewData();
    if (_intsecId != _pIntsec->getIntsecId() && app.getMap()->getIntsec(_intsecId) != NULL)
    {
        QMessageBox::warning(this, TITLE, "This ID is already used by existing intersection.");
        _intsecId = intsecIdSave;
        return;
    }
    done(QDialog::Accepted);
}

//キャンセルボタン
void IntsecDialog::on_buttonCancel_clicked()
{
    done(QDialog::Rejected);
}

//表示データ取得
void IntsecDialog::getViewData(bool getIntsecId)
{
    int	index;

    if (getIntsecId)
       _intsecId = _ui->spinBoxId->value();
    _pIntsec->setMapPos(_ui->spinBoxX->value(), _ui->spinBoxY->value(), _ui->spinBoxZ->value());
    index = _ui->comboFirstIntsec->currentIndex();
    if (index != -1)
        _pIntsec->setFirstIntsecIdCon(_ui->comboFirstIntsec->itemData(index).toInt());
}

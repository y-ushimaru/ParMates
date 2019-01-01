/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//単路編集画面

#include <sstream>
#include "ui_roadDialog.h"
#include "intsec.h"
#include "road.h"
#include "roadDialog.h"

#define TITLE	"Edit Section"

//コンストラクタ
RoadDialog::RoadDialog(QWidget *parent) : QDialog(parent), _ui(new Ui::RoadDialog)
{
    _ui->setupUi(this);
}

//デストラクタ
RoadDialog::~RoadDialog()
{
    delete _ui;
}

//変換イベント
void RoadDialog::changeEvent(QEvent *e)
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

//初期処理、上（左）が1、Y座標は逆なので注意
void RoadDialog::init(Road* pRoad)
{
    Intsec*	pIntsec1;
    Intsec*	pIntsec2;
    stringstream	intsecPos1, intsecPos2;

    _pRoad = pRoad;
    if     (pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosY() > pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosY())
    {
        _roadIntsecType1 = ROAD_IT_SMALL;
        _roadIntsecType2 = ROAD_IT_LARGE;
    }
    else if (pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosY() < pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosY())
    {
        _roadIntsecType1 = ROAD_IT_LARGE;
        _roadIntsecType2 = ROAD_IT_SMALL;
    }
    else
    {
        if (pRoad->getIntsec(ROAD_IT_SMALL)->getMapPosX() < pRoad->getIntsec(ROAD_IT_LARGE)->getMapPosX())
        {
            _roadIntsecType1 = ROAD_IT_SMALL;
            _roadIntsecType2 = ROAD_IT_LARGE;
        }
        else
        {
            _roadIntsecType1 = ROAD_IT_LARGE;
            _roadIntsecType2 = ROAD_IT_SMALL;
        }
    }
    pIntsec1 = pRoad->getIntsec(_roadIntsecType1);
    pIntsec2 = pRoad->getIntsec(_roadIntsecType2);
    _ui->labelIntsecIcon1->setPixmap(*(pIntsec1->getPixmapLabel()));
    _ui->labelIntsecIcon2->setPixmap(*(pIntsec2->getPixmapLabel()));
    _ui->labelIntsecId1->setNum(pIntsec1->getIntsecId());
    _ui->labelIntsecId2->setNum(pIntsec2->getIntsecId());
    intsecPos1 << "(" << pIntsec1->getMapPosX() << ", " << pIntsec1->getMapPosY() << ")";
    intsecPos2 << "(" << pIntsec2->getMapPosX() << ", " << pIntsec2->getMapPosY() << ")";
    _ui->labelIntsecPos1->setText(intsecPos1.str().c_str());
    _ui->labelIntsecPos2->setText(intsecPos2.str().c_str());
    _ui->spinBoxOut1->setValue(pRoad->getLane(_roadIntsecType1, ROAD_LT_OUT));
    _ui->spinBoxOut2->setValue(pRoad->getLane(_roadIntsecType2, ROAD_LT_OUT));
    _ui->spinBoxIn1->setValue(pRoad->getLane(_roadIntsecType1, ROAD_LT_IN));
    _ui->spinBoxIn2->setValue(pRoad->getLane(_roadIntsecType2, ROAD_LT_IN));
    if      (pRoad->isLane2())
        setLaneNumType(lane2);
    else if (pRoad->isLane4())
        setLaneNumType(lane4);
    else
        setLaneNumType(laneOther);
    _delete = false;
    _delIntsec = false;
}

//往復2車線
void RoadDialog::on_radioLane2_clicked()
{
    setLaneNumType(lane2);
}

//往復4車線
void RoadDialog::on_radioLane4_clicked()
{
    setLaneNumType(lane4);
}

//その他車線
void RoadDialog::on_radioLaneOther_clicked()
{
    setLaneNumType(laneOther);
}

//削除ボタン
void RoadDialog::on_buttonDelete_clicked()
{
    if (QMessageBox::question(this, TITLE, "Delete this section?",
                              QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        _delete = true;
        done(QDialog::Accepted);
    }
}

//交差点ごと削除ボタン
void RoadDialog::on_buttonDelIntsec_clicked()
{
    if (QMessageBox::question(this, TITLE, "Delete this section and connected intersections?",
                              QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        _delIntsec = true;
        done(QDialog::Accepted);
    }
}

//検証ボタン
void RoadDialog::on_buttonVerify_clicked()
{
    Road    roadSave(_pRoad->getRoadId(), _pRoad->getIntsec(ROAD_IT_SMALL),
                                          _pRoad->getIntsec(ROAD_IT_LARGE));
    string  message;

//  qDebug("RoadDialog::on_buttonVerify_clicked %s", message.c_str());
    roadSave.copyLane(_pRoad);
    getViewData();
    if (!_pRoad->checkError())
        QMessageBox::information(this, TITLE, "This section is valid.");
    else
    {
        _pRoad->getErrorMessage(&message);
        QMessageBox::warning(this, TITLE, message.c_str());
    }
    _pRoad->copyLane(&roadSave);
//  qDebug("RoadDialog::on_buttonVerify_clicked %s", message.c_str());
}

//OKボタン
void RoadDialog::on_buttonOk_clicked()
{
    getViewData();
    done(QDialog::Accepted);
}

//キャンセルボタン
void RoadDialog::on_buttonCancel_clicked()
{
    done(QDialog::Rejected);
}

//車線数種別設定
void RoadDialog::setLaneNumType(laneNumType lnt)
{
    if      (lnt == lane2)
    {
        _ui->radioLane2->setChecked(true);
        _ui->spinBoxOut1->setValue(1);
        _ui->spinBoxOut2->setValue(1);
        _ui->spinBoxIn1->setValue(1);
        _ui->spinBoxIn2->setValue(1);
    }
    else if (lnt == lane4)
    {
        _ui->radioLane4->setChecked(true);
        _ui->spinBoxOut1->setValue(2);
        _ui->spinBoxOut2->setValue(2);
        _ui->spinBoxIn1->setValue(2);
        _ui->spinBoxIn2->setValue(2);
    }
    else
        _ui->radioLaneOther->setChecked(true);
    _ui->spinBoxOut1->setEnabled(lnt == laneOther);
    _ui->spinBoxOut2->setEnabled(lnt == laneOther);
    _ui->spinBoxIn1->setEnabled(lnt == laneOther);
    _ui->spinBoxIn2->setEnabled(lnt == laneOther);
}

//表示データ取得
void RoadDialog::getViewData()
{
    _pRoad->setLane(_roadIntsecType1, ROAD_LT_OUT, _ui->spinBoxOut1->value());
    _pRoad->setLane(_roadIntsecType2, ROAD_LT_OUT, _ui->spinBoxOut2->value());
    _pRoad->setLane(_roadIntsecType1, ROAD_LT_IN,  _ui->spinBoxIn1->value());
    _pRoad->setLane(_roadIntsecType2, ROAD_LT_IN,  _ui->spinBoxIn2->value());
}

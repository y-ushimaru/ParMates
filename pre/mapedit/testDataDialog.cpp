/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//テストデータ画面

#include <stdlib.h>
#include <time.h>
#include "app.h"
#include "mapframe.h"
#include "intsec.h"
#include "road.h"
#include "testDataDialog.h"
#include "ui_testDataDialog.h"

//コンストラクタ
TestDataDialog::TestDataDialog(QWidget *parent) : QDialog(parent),
    _ui(new Ui::TestDataDialog)
{
    _ui->setupUi(this);
}

//デストラクタ
TestDataDialog::~TestDataDialog()
{
    delete _ui;
}

//初期処理
void TestDataDialog::init()
{
    _ui->spinBoxMaxX->setValue(10);
    _ui->spinBoxMaxY->setValue(10);
    _ui->dbSpinDistX->setValue(100.0);
    _ui->dbSpinDistY->setValue(100.0);
    _ui->spinBoxCycle4X->setValue(3);
    _ui->spinBoxCycle4Y->setValue(3);
    _ui->spinBoxCycle8X->setValue(6);
    _ui->spinBoxCycle8Y->setValue(6);
    _ui->spinBoxCycleBrX->setValue(2);
    _ui->spinBoxCycleBrY->setValue(2);
}

//OKボタン
void TestDataDialog::on_buttonOk_clicked()
{
    Intsec*   pIntsec;
    Road*     pRoad;
    int       intsecId, x, y, maxX, maxY;
    int       cycle4X, cycle4Y, cycle8X, cycle8Y, cycleBrX, cycleBrY, rnd, rndX, rndY;
    bool      bAlready;
    double    distX, distY, initX, initY;

    app.getMap()->deleteAll();
    app.setDefault();
    maxX = _ui->spinBoxMaxX->value();
    maxY = _ui->spinBoxMaxY->value();
    distX = _ui->dbSpinDistX->value();
    distY = _ui->dbSpinDistY->value();
    cycle4X = _ui->spinBoxCycle4X->value();
    cycle4Y = _ui->spinBoxCycle4Y->value();
    cycle8X = _ui->spinBoxCycle8X->value();
    cycle8Y = _ui->spinBoxCycle8Y->value();
    cycleBrX = _ui->spinBoxCycleBrX->value();
    cycleBrY = _ui->spinBoxCycleBrY->value();
    rnd = _ui->spinBoxRnd->value();
    rndX = rndY = 0;
    srand((unsigned int)time(NULL));
    initX = -distX * maxX / 2;
    initY = -distY * maxY / 2;
    app.setViewMagni(1.0);
    app.setViewMap(initX - distX, initY - distY);
    for (intsecId = 0; intsecId < maxX * maxY; intsecId++)
    {
        pIntsec = app.getMap()->createIntsec(intsecId);
        if (rnd != 0) {
            rndX = rand() % (rnd * 2 + 1) - rnd;
            rndY = rand() % (rnd * 2 + 1) - rnd;
//          qDebug("%d %d", rndX, rndY);
        }
        pIntsec->setMapPos((intsecId % maxX) * distX + initX + rndX,
                           (intsecId / maxX) * distY + initY + rndY, 0);
    }
    for (x = 0; x < maxX - 1; x++)
    {
        for (y = 1; y < maxY - 1; y++)
        {
            pRoad = app.getMap()->createRoad(app.getMap()->getIntsec(x + y * maxX),
                                               app.getMap()->getIntsec(x + 1 + y * maxX),
                                               &bAlready);
            if      (y % cycle8Y == 0)
            {
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_IN,  4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_IN,  4);
            }
            else if (y % cycle4Y == 0)
                pRoad->setLane4();
        }
    }
    for (x = 1; x < maxX - 1; x++)
    {
        for (y = 0; y < maxY - 1; y++)
        {
            pRoad = app.getMap()->createRoad(app.getMap()->getIntsec(x + y * maxX),
                                               app.getMap()->getIntsec(x + ( y + 1 ) * maxX), &bAlready);
            if      (x % cycle8X == 0)
            {
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_IN,  4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_IN,  4);
            }
            else if (x % cycle4X == 0)
                pRoad->setLane4();
        }
    }
    app.getMap()->deleteIntsec(0);
    app.getMap()->deleteIntsec(maxX - 1);
    app.getMap()->deleteIntsec(maxX * (maxY - 1));
    app.getMap()->deleteIntsec(maxX * maxY - 1);
    intsecId = maxX * maxY;
    for (x = 1; x < maxX - 2; x += cycleBrX)
    {
        for (y = 1; y < maxY - 2; y += cycleBrY)
        {
            pIntsec = app.getMap()->createIntsec(intsecId);
            pIntsec->setMapPos(x * distX + distX / 2 + initX,
                                y * distY +           + initY, 0);
            app.getMap()->replaceRoad(app.getMap()->getIntsec(x + y * maxX),
                                        app.getMap()->getIntsec(intsecId),
                                        app.getMap()->getIntsec(x + 1 + y * maxX));
            pRoad = app.getMap()->createRoad(app.getMap()->getIntsec(intsecId),
                                               app.getMap()->getIntsec(x + y * maxX), &bAlready);
            if      (y % cycle8Y == 0)
            {
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_SMALL, ROAD_LT_IN,  4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_OUT, 4);
                pRoad->setLane(ROAD_IT_LARGE, ROAD_LT_IN,  4);
            }
            else if (y % cycle4Y == 0)
                pRoad->setLane4();
            intsecId++;
            pIntsec = app.getMap()->createIntsec(intsecId);
            pIntsec->setMapPos(x * distX + distX / 2 + initX,
                                y * distY + distY / 2 + initY, 0);
            app.getMap()->createRoad(app.getMap()->getIntsec(intsecId - 1),
                                       app.getMap()->getIntsec(intsecId),     &bAlready);
            intsecId++;
        }
    }
    app.getMapFrame()->repaintAll(true, true);

    done(QDialog::Accepted);
}

//キャンセルボタン
void TestDataDialog::on_buttonCancel_clicked()
{
    done(QDialog::Rejected);
}

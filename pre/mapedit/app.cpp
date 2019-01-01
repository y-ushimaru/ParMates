/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//アプリケーション

#include <math.h>
#include <QtGui>
#include <QFileDialog>
#include <sstream>
#include "app.h"

using namespace std;

//背景画像ファイルダイアログ、背景画像ファイル名を指定し戻す
bool App::backFileDialog(QWidget* pParent, string* pBackFile)
{
	stringstream	filter;

    filter << "画像ファイル (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm "
          <<               "*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.TIFF *.XBM *.XPM);;"
          << "すべてのファイル (*.*)";
    QString newFile = QFileDialog::getOpenFileName(pParent, pParent->tr("背景画像ファイル"), pBackFile->c_str(),
                                                    pParent->tr(filter.str().c_str()), 0,
                                                    QFileDialog::DontUseNativeDialog);
    if (newFile == "")
        return false;
    *pBackFile = newFile.toStdString();
    return true;
}

//直線と点の計測、最小以内なら true
bool App::measureLinePoint(int lineX1, int lineY1, int lineX2, int lineY2,
                           int pointX, int pointY, int minDistance, double* pDistance)
{
    int     lineMinX, lineMinY, lineMaxX, lineMaxY;
    double  a, b, c, distance;

    //高速化のため先に範囲外を弾く
//  qDebug("App::measureLinePoint (%d, %d)-(%d, %d), (%d, %d)",
//         lineX1, lineY1, lineX2, lineY2, pointX, pointY);
    if (lineX1 < lineX2)
    {
        lineMinX = lineX1 - minDistance;
        lineMaxX = lineX2 + minDistance;
    }
    else
    {
        lineMinX = lineX2 - minDistance;
        lineMaxX = lineX1 + minDistance;
    }
    if (lineY1 < lineY2)
    {
        lineMinY = lineY1 - minDistance;
        lineMaxY = lineY2 + minDistance;
    }
    else
    {
        lineMinY = lineY2 - minDistance;
        lineMaxY = lineY1 + minDistance;
    }
    if (lineMinX > pointX || lineMaxX < pointX || lineMinY > pointY || lineMaxY < pointY )
        return false;

    // ax + by + c = 0
    a = lineY2 - lineY1;
    b = lineX1 - lineX2;
    c = (double)lineX2 * lineY1 - (double)lineX1 * lineY2;
    distance = fabs(a * pointX + b * pointY + c) / sqrt(a * a + b * b);
//  qDebug("App::measureLinePoint distance %f(%d)", distance, minDistance);
    if (distance > (double)minDistance)
        return false;
    *pDistance = distance;
    return true;
}

//設定エディタ
void App::configEditor(int intsecId)
{
    //交差点IDが負なら全体
    // config_editor [-p <パス> [-i <交差点ID>]]、(パスは終端/なし)

    stringstream ss;
#ifdef Q_OS_WIN32
    ss << "config_editor.exe";
#else
    ss << "./config_editor";
#endif
    if (_setupFile != "")
    {
        QFileInfo info(_setupFile.c_str());
        ss << " -p \"" << info.path().toStdString() << "\"";  //"" はlinux でも使える
        if (intsecId >= 0)
            ss << " -i " << intsecId;
    }

//  qDebug() << ss.str().c_str();
    QProcess proc;
    proc.startDetached(ss.str().c_str());
}

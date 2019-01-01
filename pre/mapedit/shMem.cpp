/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <QtGui>
#include <QMessageBox>
#include <sstream>
#include "shMem.h"

using namespace std;

//設定
void ShMem::set(int intsecId1, int intsecId2)
{
    stringstream ssError;

    if (!_shMem.attach() && _shMem.error() != 0)
    {
//      qDebug() << "attach error" << _shMem.error() << " " <<_shMem.errorString();
        if (!_shMem.create(sizeof(ShMemData)))
        {
            ssError << "shared memory create error code=" << _shMem.error()
                    << "\n" <<_shMem.errorString().toStdString();
            QMessageBox::warning(NULL, "shared memory error", ssError.str().c_str());
            return;
        }
    }

    if (!_shMem.lock())
    {
        ssError << "shared memory lock error code=" << _shMem.error()
                << "\n" <<_shMem.errorString().toStdString();
        QMessageBox::warning(NULL, "shared memory error", ssError.str().c_str());
        return;
    }

    ShMemData data;
    data.intsecId1 = intsecId1;
    data.intsecId2 = intsecId2;
    memcpy(_shMem.data(), &data, sizeof(ShMemData));

    _shMem.unlock();
//  qDebug() << intsecId1 << " " << intsecId2;
}

//取得
void ShMem::get(int* intsecId1, int* intsecId2)
{
    stringstream ssError;

    *intsecId1 = -1;
    if (intsecId2 != NULL)
        *intsecId2 = -1;

    if (!_shMem.attach() && _shMem.error() != 0)
    {
//      qDebug() << "attach error" << _shMem.error() << " " <<_shMem.errorString();
        return;
    }

    if (!_shMem.lock()) {
        ssError << "shared memory lock error code=" << _shMem.error()
                << "\n" <<_shMem.errorString().toStdString();
        QMessageBox::warning(NULL, "shared memory error", ssError.str().c_str());
        _shMem.detach();
        return;
    }

    ShMemData data;
    memcpy(&data, _shMem.data(), sizeof(ShMemData));

    _shMem.unlock();

    *intsecId1 = data.intsecId1;
    if (intsecId2 != NULL)
        *intsecId2 = data.intsecId2;
//   qDebug() << data.intsecId1 << " " << data.intsecId2;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//メイン

#include <QTranslator>
#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <sstream>
#define MAIN_CPP
#include "app.h"
#include "mainwindow.h"
#include "mapframe.h"
#include "map.h"
#include <iostream>

//メイン
int main(int argc, char *argv[])
{
#ifdef USE_QT4
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif // USE_QT4
    QApplication a(argc, argv);
    MainWindow mw;
    MapFrame mf;
    Map m;

    const char* path = NULL;
    stringstream ss;
    ss << "Usage: mapedit [-p <Path>] \n\n";
    ss << "parametor error :\nmapedit ";
    for (int i = 1; i < argc; i++)
        ss << argv[i] << " ";
    if (argc == 3 && strcmp(argv[1], "-p") == 0)
        path = argv[2];
    else if (argc != 1)
        QMessageBox::warning(NULL, mw.tr("mapedit"), mw.tr(ss.str().c_str()));

    app.init(&mw, &mf, &m);
    mw.setCentralWidget(&mf);
    mw.init();
    m.init();
    mw.show();
    if (path != NULL)
        mw.fileOpen(path);

    return a.exec();
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//メイン

#include <Qt>
#include <QDir>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "app.h"
#include "map.h"
#include "osmFile.h"
#include "matesFile.h"
#include "merge.h"

using namespace std;

#define USAGE "USAGE: advmates-osm2mates <setup file>"

int main(int argc, char *argv[])
{
    Map map;
    osmFile osmf;
    MatesFile mf;
    Merge merge;
    char buff[1024];

    if (argc != 2)
    {
        cout << USAGE << endl;
        return -1;
    }

    cout << "current dir \"" << QDir::currentPath().toStdString()
         << "\"" << endl;

    if (!app.init(&map, argv[1]))
        return -1;

    if (app.osm())
        osmf.prevRead();
    else
        mf.prevRead();
    mf.prevWrite();

    //確認
    if (!app.noConfirm())
    {
        cout << endl << "ok ? (y/other)" << endl << "> ";
        cin.getline(buff, sizeof(buff));
        if (strcmp(buff, "y") != 0)
        {
            cout << "canceled" << endl;
            return -1;
        }
    }
    cout << endl;

    if (app.osm())
    {
        if (!osmf.read())
            return -1;
    }
    else {
        if (!mf.read())
            return -1;
    }
    map.refine();

    if (app.modify())
    {
        if (app.modMergeLen() >= 0)
        {
            merge.merge(MERGE_MODE_NORMAL);
            merge.merge(MERGE_MODE_REVISE);
            map.refine();
        }

        if (app.modForceOneLane() >= 0)
            map.forceOneLane();
    }

    map.setIntsecFinalId();
    if (!mf.write())
        return -1;

    return 0;
}

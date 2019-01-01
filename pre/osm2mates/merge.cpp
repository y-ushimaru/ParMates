/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
//統合

#include <QTime>
#include <iostream>
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "road.h"
#include <AmuPoint.h>
#include <AmuVector.h>
#include "merge.h"

MergeIntsec::MergeIntsec()
{
    _deleted = false;
    _mapX = _mapY = _mapZ = 0.0;
}

void MergeIntsec::calcMapPos(bool final)
{
    map<int,Intsec*>::iterator iti;
    int prefNum = 0;
    double prefMapX, prefMapY, prefMapZ, allMapX, allMapY, allMapZ;

    //最終的には三叉路以上優先、途中は平均で計算しないと統合できない

    prefMapX = prefMapY = prefMapZ = allMapX = allMapY = allMapZ = 0.0;
    for (iti = _intsecs.begin(); iti != _intsecs.end(); iti++)
    {
        Intsec* intsec = iti->second;
        if (final && intsec->roadNum() >= 3)
        {
            prefNum++;
            prefMapX += intsec->mapPosX();
            prefMapY += intsec->mapPosY();
            prefMapZ += intsec->mapPosZ();
        }
        else
        {
            allMapX += intsec->mapPosX();
            allMapY += intsec->mapPosY();
            allMapZ += intsec->mapPosZ();
        }
    }
    if (prefNum != 0)
    {
        _mapX = prefMapX / prefNum;
        _mapY = prefMapY / prefNum;
        _mapZ = prefMapZ / prefNum;
    }
    else
    {
        _mapX = allMapX / _intsecs.size();
        _mapY = allMapY / _intsecs.size();
        _mapZ = allMapZ / _intsecs.size();
    }
}

void MergeIntsec::addIntsec(Intsec* intsec)
{
//  if (intsec->id() == 8 || intsec->id() == 334 )
//      cout << "addIntsec " << intsec->id() << endl;
    map<int,Intsec*>::iterator iti = _intsecs.find(intsec->id());
    if (iti == _intsecs.end())
        _intsecs.insert(pair<int, Intsec*>(intsec->id(), intsec));
    intsec->setMergeIntsec(this);
}

void MergeIntsec::moveAllIntsecs(MergeIntsec* src)
{
    map<int,Intsec*>::iterator iti;
    for (iti = src->_intsecs.begin(); iti != src->_intsecs.end(); iti++)
    {
        Intsec* intsec = iti->second;
        _intsecs.insert(pair<int, Intsec*>(intsec->id(), intsec));
        intsec->setMergeIntsec(this);
//      if (intsec->id() == 8 || intsec->id() == 334 )
//          cout << "moveAllIntsecs " << intsec->id() << endl;
    }
    src->_intsecs.clear();
    src->_deleted = true;
}

void MergeIntsec::merge()
{
    map<int,Intsec*>::iterator iti;

//  cout << "mi " << _deleted << " " << _mapX << " " << _mapY << endl << " ";
//  for (iti = _intsecs.begin(); iti != _intsecs.end(); iti++)
//      cout << " " << iti->first;
//  cout << endl;

    if (_deleted)
        return;

    calcMapPos(true);
    Intsec* intsec = app.map()->createIntsec();
    int id = intsec->id();
    intsec->setMapPos(_mapX, _mapY, _mapZ);

    for (iti = _intsecs.begin(); iti != _intsecs.end(); iti++)
    {
        int idCon;
        INTSEC_ROADMAP_IT iami;
        Road* road = (iti->second)->nextRoad(&iami, true, &idCon);
        while (road != NULL)
        {
            if (id != idCon)
            {
                //道路作成後に自分につながる場合があるので排除
                Intsec* intsecCon = app.map()->findIntsec(idCon);
                if (intsecCon->mergeIntsec() != this)
                {
                    Road* mergeRoad = app.map()->createRoad(intsec, intsecCon, 0);
                    int intsecTypeCon = road->intsecType(idCon);
                    Q_ASSERT(intsecTypeCon != ROAD_IT_NONE);
                    mergeRoad->addLanes(intsecCon,
                                        road->lane(intsecTypeCon, ROAD_LT_OUT),
                                        road->lane(intsecTypeCon, ROAD_LT_IN),
                                        road->lane(ROAD_IT_REVERSE(intsecTypeCon), ROAD_LT_OUT),
                                        road->lane(ROAD_IT_REVERSE(intsecTypeCon), ROAD_LT_IN));
                }
            }
            road = (iti->second)->nextRoad(&iami, false, &idCon);
        }
    }

    for (iti = _intsecs.begin(); iti != _intsecs.end(); iti++)
    {
        app.map()->deleteIntsec(iti->first);
    }
}

Merge::Merge()
{
    _intsec = NULL;
    _interOp = false;
}

Merge::~Merge()
{
    deleteAll();
}

void Merge::deleteAll()
{
    for (int i = 0; i < (int)_mergeIntsecs.size(); i++)
        delete _mergeIntsecs[i];
    _mergeIntsecs.clear();
    _intsecsFind.clear();
}

void Merge::merge(int mode)
{
    deleteAll();

    int find;
    _interOp = false;
    if (mode == MERGE_MODE_INTEROP)
    {
        cout << "merge interop..." << endl;
        find = app.modMergeFind();
        _interOp = true;
    }
    else if (mode == MERGE_MODE_NORMAL)
    {
        cout << "merge..." << endl;
        find = app.modMergeFind();
    }
    else if (mode == MERGE_MODE_REVISE && app.modMergeFind() >= 3)
    {
        cout << "merge revise..." << endl;
        find = 3;
    }
    else
        return;

    MAP_INTSECMAP_IT imi;
    QTime time;
    time.start();
    _intsec = app.map()->nextIntsec(&imi, true);
    int intsecCnt = 0;
    while (_intsec != NULL)
    {

        if (time.elapsed() >= app.reportIntv())
        {
            cout << "  " << (double)intsecCnt / (double)app.map()->intsecNum() * 100
                 << " %" << endl;
            time.restart();
        }

        //補間点チェック
        if (!_interOp || _intsec->interOp())
        {
            getMergeMapPos(_intsec, &_pointIntsec);
            _intsecsFind.clear();
            findMergeIntsec(_intsec, find);
        }

        intsecCnt++;
        _intsec = app.map()->nextIntsec(&imi, false);
    }

    for (int i = 0; i < (int)_mergeIntsecs.size(); i++)
        _mergeIntsecs[i]->merge();
//  cout << "merge " << _mergeIntsecs.size() << endl;
}

void Merge::getMergeMapPos(Intsec* intsec, AmuPoint* point)
{
    MergeIntsec* mergeIntsec = intsec->mergeIntsec();
    if (intsec->mergeIntsec() != NULL)
    {
        point->setX(mergeIntsec->mapPosX());
        point->setY(mergeIntsec->mapPosY());
        point->setZ(mergeIntsec->mapPosZ());
    }
    else
    {
        point->setX(intsec->mapPosX());
        point->setY(intsec->mapPosY());
        point->setZ(intsec->mapPosZ());
    }
}

void Merge::findMergeIntsec(Intsec* intsecFind, int findNum)
{
    INTSEC_ROADMAP_IT iami;
    int idCon;
    AmuPoint pointIntsecCon;

    if (_intsecsFind.find(intsecFind->id()) != _intsecsFind.end())
        return;
    _intsecsFind.insert(pair<int, Intsec*>(intsecFind->id(), intsecFind));

    findNum--;

    Road* road = intsecFind->nextRoad(&iami, true, &idCon);
    while (road != NULL)
    {
        Intsec* intsecCon = app.map()->findIntsec(idCon);

        //補間点チェック
        if (!_interOp || intsecCon->interOp())
        {
            getMergeMapPos(intsecCon, &pointIntsecCon);
            AmuVector vec(_pointIntsec, pointIntsecCon);

            if (vec.size() <= app.modMergeLen())
            {
                mergeIntsec(intsecCon);
            }

            if (findNum > 0)
                findMergeIntsec(intsecCon, findNum);
        }

        road = intsecFind->nextRoad(&iami, false, &idCon);
    }
}

void Merge::mergeIntsec(Intsec* intsecCon)
{
    MergeIntsec* mergeIntsec    = _intsec->mergeIntsec();
    MergeIntsec* mergeIntsecCon = intsecCon->mergeIntsec();

//  cout << "m " << _intsec->id() << " " << intsecCon->id();

    if (mergeIntsec == NULL)
    {
        if (mergeIntsecCon == NULL)
        {
//          cout << " 1" << endl;
            mergeIntsec = new MergeIntsec();
            _mergeIntsecs.push_back(mergeIntsec);
            mergeIntsec->addIntsec(_intsec);
            mergeIntsec->addIntsec(intsecCon);
            mergeIntsec->calcMapPos();
        }
        else
        {
//          cout << " 2" << endl;
            mergeIntsecCon->addIntsec(_intsec);
            mergeIntsecCon->calcMapPos();
        }
    }
    else
    {
        if (mergeIntsecCon == NULL)
        {
//          cout << " 3" << endl;
            mergeIntsec->addIntsec(intsecCon);
            mergeIntsec->calcMapPos();
        }
        else if (mergeIntsec != mergeIntsecCon)
        {
//          cout << " 4" << endl;
            mergeIntsec->moveAllIntsecs(mergeIntsecCon);
            mergeIntsec->calcMapPos();
        }
    }
}


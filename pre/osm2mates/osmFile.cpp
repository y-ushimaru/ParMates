/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
// OSM ファイルアクセス

#include <Qt>
#include <QFile>
#include <QXmlStreamReader>
#include <QTime>
#include <iostream>
#include <math.h>
#include "app.h"
#include "map.h"
#include "intsec.h"
#include "road.h"
#include "osmFile.h"

using namespace std;

osmFileNode::osmFileNode(double lat, double lon, bool border)
{
    _intsec = NULL;
    _lat = lat;
    _lon = lon;
    _border = border;
}

Intsec* osmFileNode::createIntsec()
{
    if (_intsec == NULL)
    {
        _intsec = app.map()->createIntsec();
        setIntsecMapPos();
        _intsec->setBorder(_border);
    }

    return _intsec;
}

void osmFileNode::setIntsecMapPos()
{
    if (_intsec != NULL)
        _intsec->setMapPos(app.lonToX(_lon), app.latToY(_lat), 0);
}

osmFile::osmFile()
{
    _lanes = -1;
    _oneway = false;

    _latMin = _lonMin =  APP_LATLON_MAX;
    _latMax = _lonMax = -APP_LATLON_MAX;
}

osmFile::~osmFile()
{
    map<string, osmFileNode*>::iterator itn = _nodes.begin();
    while (itn != _nodes.end())
    {
        delete (*itn).second;
        itn++;
    }
}

void osmFile::prevRead()
{
    cout << "read osm file \"" << *app.inputOSM_File() << "\"" << endl;
}

bool osmFile::read()
{
    //順番に並んでるか不明なので２回に分けて読む

    _fileName = *app.inputOSM_File();

    if (!readNodes())
        return false;

    if (!readWays())
        return false;

    return true;
}

bool osmFile::readNodes()
{
    cout << "read osm nodes..." << endl;

    QFile* file = new QFile(_fileName.c_str());
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr << "osm file open error (node)" << endl << _fileName << endl;
        return false;
    }

    QXmlStreamReader xml(file);

    _latMin = _lonMin =  APP_LATLON_MAX;
    _latMax = _lonMax = -APP_LATLON_MAX;

    QTime time;
    time.start();
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (time.elapsed() >= app.reportIntv())
        {
            cout << "  " << (double)xml.characterOffset() / (double)file->size() * 100
                 << " %" << endl;
            time.restart();
        }

        if (token == QXmlStreamReader::StartElement)
        {
            QXmlStreamAttributes attr = xml.attributes();
            if (xml.name() == "node")
            {
                string id = attr.value("id").toString().toStdString();
                double lat = attr.value("lat").toString().toDouble();
                double lon = attr.value("lon").toString().toDouble();
                bool border;
                if (app.inLatLonMinMax(lat, lon, &border))
                {
                    setLatLonMinMax(lat, lon);
                    osmFileNode* node = new osmFileNode(lat, lon, border);
                    _nodes.insert(make_pair(id, node));
/*
                    cout << "node "
                         << " id="  << attr.value("id").toString().toStdString()
                         << " lat=" << attr.value("lat").toString().toStdString()
                         << " lon=" << attr.value("lon").toString().toStdString() << endl;
*/
                }
            }
        }
    }

    if(xml.hasError()) {
        cerr << "osm file format error (node)" << endl;
        return false;
    }
    file->close();
    xml.clear();

    app.setLatLonMid((_latMin + _latMax) / 2, (_lonMin + _lonMax) / 2);
//  cout << _latMin << " " << _latMax << " " << _lonMin << " " << _lonMax << endl;

    return true;
}

bool osmFile::readWays()
{
    cout << "read osm ways..." << endl;

    QFile* file = new QFile(_fileName.c_str());
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr << "osm file open error (way)" << endl << _fileName << endl;
        return false;
    }

    QXmlStreamReader xml(file);
    bool way = false;

    _ndRefs.clear();
    _highway = "";
    _lanes = -1;
    _oneway = false;

    _latMin = _lonMin =  APP_LATLON_MAX;
    _latMax = _lonMax = -APP_LATLON_MAX;

    QTime time;
    time.start();
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (time.elapsed() >= app.reportIntv())
        {
            cout << "  " << (double)xml.characterOffset() / (double)file->size() * 100
                 << " %" << endl;
            time.restart();
        }

        if (token == QXmlStreamReader::StartElement)
        {
            QXmlStreamAttributes attr = xml.attributes();
            if (xml.name() == "way")
            {
                way = true;
//              cout << "way " << attr.value("id").toString().toStdString() << endl;
            }
            else if (xml.name() == "nd" && way)
                _ndRefs.push_back(attr.value("ref").toString().toStdString());
            else if (xml.name() == "tag" && way)
            {
                if (attr.value("k") == "highway")
                    _highway = attr.value("v").toString().toStdString();
                else if (attr.value("k") == "lanes")
                    _lanes = attr.value("v").toString().toInt();
                else if (attr.value("k") == "oneway")
                    _oneway = (attr.value("v") == "yes");
            }
        }
        else if (token == QXmlStreamReader::EndElement)
        {
            if (xml.name() == "way")
            {
                setWay();
                way = false;
                _ndRefs.clear();
                _highway = "";
                _lanes = -1;
                _oneway = false;
            }
        }
    }

    if(xml.hasError())
    {
        cerr << "osm file format error (way)" << endl;
        return false;
    }
    file->close();
    xml.clear();

    //道路だけの座標を元に位置設定
    app.setLatLonMid((_latMin + _latMax) / 2, (_lonMin + _lonMax) / 2);
//  cout << _latMin << " " << _latMax << " " << _lonMin << " " << _lonMax << endl;
    map<string, osmFileNode*>::iterator itn = _nodes.begin();
    while (itn != _nodes.end())
    {
        (*itn).second->setIntsecMapPos();
        itn++;
    }

    return true;
}

void osmFile::setWay()
{
    map<string, osmFileNode*>::iterator itn;
    Intsec* intsec1;
    Intsec* intsec2;
    int osmRT, laneForward, laneBackward;

//  cout << "setWay" << endl;

    for (osmRT = 0; osmRT < APP_OSM_RT_NUM; osmRT++)
    {
        if (_highway == *app.osmRT_Key(osmRT))
            break;
    }
    if (osmRT == APP_OSM_RT_NUM || app.osmRT_Lane(osmRT) == 0)
        return;
    if (_lanes == -1)
        laneForward = laneBackward = app.osmRT_Lane(osmRT);
    else
        laneForward = laneBackward = _lanes;
    if (_oneway)
        laneBackward = 0;

    intsec1 = intsec2 = NULL;
    for (int i = 0; i < (int)_ndRefs.size(); i++)
    {
        itn = _nodes.find(_ndRefs[i]);
        if (itn == _nodes.end())
        {
            //ノードなし、範囲外など、完全に道路を切る
            intsec1 = intsec2 = NULL;
            continue;
        }

        //道路だけ抽出して後で位置再設定
        setLatLonMinMax((*itn).second->lat(), (*itn).second->lon());
//      cout << (*itn).second->lat() << " " << (*itn).second->lon() << endl;

        intsec1 = intsec2;
        intsec2 = (*itn).second->createIntsec();
        if (intsec1 != NULL && intsec2 != NULL)
            app.map()->createRoadPackage(intsec1, intsec2,
                                         laneForward, laneBackward);
    }
}

void osmFile::setLatLonMinMax(double lat, double lon)
{
    if (_latMin > lat)
        _latMin = lat;
    if (_lonMin > lon)
        _lonMin = lon;
    if (_latMax < lat)
        _latMax = lat;
    if (_lonMax < lon)
        _lonMax = lon;
}

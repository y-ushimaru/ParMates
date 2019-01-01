/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Border.h"
#include "AmuLineSegment.h"
#include "ObjManager.h"
#include "Connector.h"
#include "Conf.h"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

//======================================================================
Border::Border(const AmuPoint begin, const AmuPoint end,
               double roadsideWidth)
{
    _roadsideWidth = roadsideWidth;
    _ptLSegment = new AmuLineSegment(begin, end);
}

//======================================================================
Border::Border(const AmuPoint begin, const AmuPoint end,
               int din, int dout, double roadsideWidth)
{
    assert(din >= 0 && dout >= 0);
    _roadsideWidth = roadsideWidth;
    _ptLSegment = new AmuLineSegment(begin, end);
    _inPoints.resize(din);
    _outPoints.resize(dout);
    _createConnectors();
}

//======================================================================
Border::~Border()
{
    delete _ptLSegment;
}

//======================================================================
const AmuLineSegment* Border::lineSegment() const
{
    return _ptLSegment;
}

//======================================================================
const Connector* Border::connector(int i) const
{
    // 流入ポイントから順に数えてi番目
    if(i < static_cast<signed int>(_inPoints.size()))
    {
        assert(0 <= i);
        assert(i < static_cast<signed int>(_inPoints.size()));
        return _inPoints[i];
    }
    else if (i < static_cast<signed int>(_inPoints.size()
                                         + _outPoints.size()))
    {
        i -= _inPoints.size();
        assert(0 <= i);
        assert(i < static_cast<signed int>(_outPoints.size()));
        return _outPoints[i];
    }
    else
    {
        return NULL;
    }
}

//======================================================================
vector<const Connector*> Border::inPoints() const
{
    return _inPoints;
}

//======================================================================
vector<const Connector*> Border::outPoints() const
{
    return _outPoints;
}

//======================================================================
bool Border::addConnector(double r, bool isIn)
{
    if (r >= 0 && r <= 1)
    {
        const Connector* connector = _createInnerConnector(r, 1 - r);
        assert(connector);
        if (isIn)
        {
            _inPoints.push_back(connector);
        }
        else
        {
            _outPoints.push_back(connector);
        }
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Border::addConnector(double x, double y, double z, bool isIn)
{
    const Connector* connector = ObjManager::createConnector(x, y, z);
    assert(connector);
    if (isIn)
    {
        _inPoints.push_back(connector);
    }
    else
    {
        _outPoints.push_back(connector);
    }
    return true;
}

//======================================================================
Border Border::reversal() const
{
    Border rev(lineSegment()->pointEnd(), lineSegment()->pointBegin(),
               _roadsideWidth);
    return rev;
}

//======================================================================
bool Border::isIn(Connector* pt) const
{
    vector<const Connector*>::const_iterator where;
    where = find(_inPoints.begin(),_inPoints.end(),pt);
    if(where != _inPoints.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Border::isOut(Connector* pt) const
{
    vector<const Connector*>::const_iterator where;
    where = find(_outPoints.begin(),_outPoints.end(),pt);
    if(where != _outPoints.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
int Border::numIn() const
{
    return static_cast<int>(_inPoints.size());
}

//======================================================================
int Border::numOut() const{
    return static_cast<int>(_outPoints.size());
}

//======================================================================
void Border::_createConnectors()
{
    //コネクタは境界線を等分する。
    double sum = _inPoints.size() + _outPoints.size();
    for(int i = 0;i < static_cast<signed int>(_inPoints.size());i++)
    {
        _inPoints[i] = _createInnerConnector(2*i + 1, 2*(sum-i) - 1);
    }

    for(int i = 0;i < static_cast<signed int>(_outPoints.size());i++)
    {
        _outPoints[i] = _createInnerConnector(2*(i+_inPoints.size()) + 1,
                                              2*(_outPoints.size()-i) - 1);
    }
}

//======================================================================
const Connector* Border::_createInnerConnector(const double d0,
                                         const double d1)
{
    AmuVector vec = _ptLSegment->directionVector();
    vec.normalize();
    AmuLineSegment line(_ptLSegment->pointBegin()+vec*_roadsideWidth,
                        _ptLSegment->pointEnd()-vec*_roadsideWidth);
    const AmuPoint tmpPoint = line.createInteriorPoint(d0, d1);
    if (!tmpPoint.flag())
    {
        cerr << "WARNING: Border::createInnerConnector(): bad Point."
             << endl;
    }
    return ObjManager::createConnector(tmpPoint.x(),
                                       tmpPoint.y(),
                                       tmpPoint.z());
}

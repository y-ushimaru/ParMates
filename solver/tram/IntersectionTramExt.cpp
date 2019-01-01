/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "IntersectionTramExt.h"
#include "TramLaneInIntersection.h"
#include "BorderTram.h"
#include "TramLineManager.h"
#include "../Intersection.h"
#include "../Section.h"
#include "../ObjManager.h"
#include "../GVManager.h"
#include "../AmuConverter.h"
#include "../Conf.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <string>

using namespace std;

//======================================================================
IntersectionTramExt::IntersectionTramExt(Intersection* inter)
{
    _inter = inter;
}

//======================================================================
void IntersectionTramExt::setNumTramConnectors()
{
    assert(_inter);

    _numInTramLeft.resize(_inter->numNext());
    fill(_numInTramLeft.begin(), _numInTramLeft.end(), 0);
    _numOutTramLeft.resize(_inter->numNext());
    fill(_numOutTramLeft.begin(), _numOutTramLeft.end(), 0);

    _numInTramCenter.resize(_inter->numNext());
    fill(_numInTramCenter.begin(), _numInTramCenter.end(), 0);
    _numOutTramCenter.resize(_inter->numNext());
    fill(_numOutTramCenter.begin(), _numOutTramCenter.end(), 0);

    _numInTramRight.resize(_inter->numNext());
    fill(_numInTramRight.begin(), _numInTramRight.end(), 0);
    _numOutTramRight.resize(_inter->numNext());
    fill(_numOutTramRight.begin(), _numOutTramRight.end(), 0);

    TramLineManager* tramLineManager = &(TramLineManager::instance());
    for (int i=0; i<_inter->numNext(); i++)
    {
        tramLineManager
            ->getNumTramConnectors
            (_inter, i,
             &_numInTramLeft[i],   &_numOutTramLeft[i],
             &_numInTramCenter[i], &_numOutTramCenter[i],
             &_numInTramRight[i],  &_numOutTramRight[i]);
    }
}

//======================================================================
int IntersectionTramExt::numInTramLeft(int dir) const
{
    return _numInTramLeft[dir];
}

//======================================================================
int IntersectionTramExt::numOutTramLeft(int dir) const
{
    return _numOutTramLeft[dir];
}

//======================================================================
int IntersectionTramExt::numTotalTramLeft(int dir) const
{
    return _numInTramLeft[dir] + _numOutTramLeft[dir];
}
 
//======================================================================
int IntersectionTramExt::numInTramCenter(int dir) const
{
    return _numInTramCenter[dir];
}

//======================================================================
int IntersectionTramExt::numOutTramCenter(int dir) const
{
    return _numOutTramCenter[dir];
}

//======================================================================
int IntersectionTramExt::numTotalTramCenter(int dir) const
{
    return _numInTramCenter[dir] + _numOutTramCenter[dir];
}
 
//======================================================================
int IntersectionTramExt::numInTramRight(int dir) const
{
    return _numInTramRight[dir];
}

//======================================================================
int IntersectionTramExt::numOutTramRight(int dir) const
{
    return _numOutTramRight[dir];
}

//======================================================================
int IntersectionTramExt::numTotalTramRight(int dir) const
{
    return _numInTramRight[dir] + _numOutTramRight[dir];
}

//======================================================================
int IntersectionTramExt::numTotalTramLanes(int dir) const
{
    return (_numInTramLeft[dir] + _numOutTramLeft[dir]
            + _numInTramCenter[dir] + _numOutTramCenter[dir]
            + _numInTramRight[dir] + _numOutTramRight[dir]);
}

//======================================================================
int IntersectionTramExt::tramDirection(const Connector* connector) const
{
    int dir = -1;
    for (int i=0; i<_inter->numNext(); i++)
    {
        BorderTram* borderTram
            = dynamic_cast<BorderTram*>
            (const_cast<Border*>(_inter->border(i)));
        if (!borderTram)
        {
            continue;
        }
        if (borderTram->inPointTram() == connector
            || borderTram->outPointTram() == connector)
        {
            dir = i;
            break;
        }
    }
    return dir;
}

//======================================================================
void IntersectionTramExt::createConnectorsOnLine(AmuLineSegment line,
                                                 int numIn, int numOut,
                                                 int borderId)
{
    string id;
    int idInt1 = 0;
    int sum = numIn + numOut + numTotalTramLanes(borderId);

    AmuVector vec = line.directionVector();
    vec.normalize();

    AmuLineSegment newLine
        (line.pointBegin()
         + vec * _inter->_incSections[borderId]->roadsideWidth(),
         line.pointEnd()
         - vec * _inter->_incSections[borderId]->roadsideWidth());

    for (int i=0; i<numIn; i++)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numTotalTramLeft(borderId) + i;
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _inter->_innerConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
    for (int i=0; i<numOut; i++)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numTotalTramLeft(borderId) + numIn
            + numTotalTramCenter(borderId) + i;
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _inter->_innerConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }

    // 路面電車コネクタ
    if (numInTramLeft(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = 0;
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
    else if (numInTramCenter(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numIn;
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
    else if (numInTramRight(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numIn + numOut;
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }

    if (numOutTramLeft(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numInTramLeft(borderId);
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
    else if (numOutTramCenter(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numIn + numInTramCenter(borderId);
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
    else if (numOutTramRight(borderId)>0)
    {
        id = AmuConverter::itos(borderId*100+idInt1,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        idInt1++;
        int n = numIn + numOut + numInTramRight(borderId);
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(2*n+1, 2*(sum-n)-1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: IntersectionTramExt"
                 << "::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerTramConnectors[id]
            = ObjManager::createConnector(tmpPoint.x(),
                                          tmpPoint.y(),
                                          tmpPoint.z());
    }
}

//======================================================================
bool IntersectionTramExt::createTramLanesInIntersection()
{
    TramLineManager* tramLineManager
        = &(TramLineManager::instance());

    for (int i=0; i<_inter->numNext(); i++)
    {
        BorderTram* borderFrom
            = dynamic_cast<BorderTram*>
            (const_cast<Border*>(_inter->border(i)));

        if (borderFrom==NULL)
        {
            continue;
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 中心サブセクション
        for (int j=0; j<_inter->numNext(); j++)
        {
            if (!(borderFrom->inPointTram()))
            {
                break;
            }

            if (i==j)
            {
                continue;
            }

            BorderTram* borderTo
                = dynamic_cast<BorderTram*>
                (const_cast<Border*>(_inter->border(j)));
            if (borderTo==NULL
                || borderTo->outPointTram()==NULL)
            {
                continue;
            }

            if (tramLineManager->isTramLineConnected(_inter, i, j))
            {
                int idIntBegin
                    = i*100 + _inter->numIn(i) + _inter->numOut(i);
                int idIntEnd
                    = j*100 + _inter->numIn(j) + _inter->numOut(j)
                    + _numInTramLeft[j]
                    + _numInTramCenter[j]
                    + _numInTramRight[j];
                int idInt
                    = idIntBegin * 10000 + idIntEnd;

                createTramLane(idInt,
                               edgeTramConnector(idIntBegin),
                               edgeTramConnector(idIntEnd),
                               true);
            }
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 横断歩道
        if (_inter->crosswalkWidth(i)!=0)
        {
            int idInt1 = _inter->numIn(i) + _inter->numOut(i);
            if (borderFrom->inPointTram())
            {
                int idIntBegin
                    = 1*1000 + i*100 + idInt1;
                int idIntEnd
                    = i*100  + idInt1;
                int idInt
                    = idIntBegin * 10000 + idIntEnd;
                idInt1++;
                createTramLane
                    (idInt,
                     borderFrom->inPointTram(),
                     _innerTramConnectors
                     [AmuConverter::itos
                      (idIntEnd, NUM_FIGURE_FOR_CONNECTOR_LOCAL)],
                     false);
            }
            if (borderFrom->outPointTram())
            {
                int idIntBegin
                    = i*100  + idInt1;
                int idIntEnd
                    = 1*1000 + i*100 + idInt1;
                int idInt
                    = idIntBegin * 10000 + idIntEnd;
                cout << _inter->_id << ", " << idInt << endl;
                createTramLane
                    (idInt,
                     _innerTramConnectors
                     [AmuConverter::itos
                      (idIntBegin, NUM_FIGURE_FOR_CONNECTOR_LOCAL)],
                     borderFrom->outPointTram(),
                     false);
            }
        }
    }

    return true;
}

//======================================================================
bool IntersectionTramExt::createTramLanesInODNode()
{
    BorderTram* border0
        = dynamic_cast<BorderTram*>
        (const_cast<Border*>(_inter->border(0)));
    BorderTram* border1
        = dynamic_cast<BorderTram*>
        (const_cast<Border*>(_inter->border(1)));
    if (border0==NULL
        || border1==NULL)
    {
        return true;
    }
    if (border0->inPointTram()
        && border1->outPointTram())
    {
        int idIntBegin
            = border0->numIn() + border0->numOut();
        int idIntEnd
            = 100 + border1->numIn() + border1->numOut()
            + _numOutTramLeft[0]
            + _numOutTramCenter[0]
            + _numOutTramRight[0];
        int idInt
        = idIntBegin * 10000 + idIntEnd;
    
        createTramLane(idInt,
                       border0->inPointTram(),
                       border1->outPointTram(),
                       true);
    }
    if (border1->inPointTram()
        && border0->outPointTram())
    {
        int idIntBegin
            = 100 + border1->numIn() + border1->numOut();
        int idIntEnd
            = border0->numIn() + border0->numOut()
            + _numInTramLeft[0]
            + _numInTramCenter[0]
            + _numInTramRight[0];
        int idInt
            = idIntBegin * 10000 + idIntEnd;
    
        createTramLane(idInt,
                       border1->inPointTram(),
                       border0->outPointTram(),
                       true);
    }
    return true;
}

//======================================================================
void IntersectionTramExt::createTramLane(int idInt,
                                         const Connector* from,
                                         const Connector* to,
                                         bool setLaneBegEnd)
{
    string id;
    id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);

    TramLaneInIntersection* lane
        = new TramLaneInIntersection(id, from, to, _inter);
    lane
        ->setSpeedLimit(GVManager::getNumeric("TRAM_SPEED_LIMIT"));
    _inter->addLane(lane, true);
}

//======================================================================
const Connector* IntersectionTramExt::edgeTramConnector(int idInt)
{
    // 境界方向
    int dir = idInt/100;

    if (_inter->crosswalkWidth(dir) != 0)
    {
        // 接続元の境界に横断歩道が設置されている - 内部コネクタ
        return _innerTramConnectors
            [AmuConverter::itos(idInt, NUM_FIGURE_FOR_CONNECTOR_LOCAL)];
    }
    else
    {
        // 横断歩道が設置されていない
        BorderTram* borderTram
            = dynamic_cast<BorderTram*>
            (const_cast<Border*>(_inter->border(dir)));
        if (!borderTram)
        {
            return NULL;
        }
        else
        {
            int idInt2 = idInt%100
                - (_inter->numIn(dir) + _inter->numOut(dir));
            return borderTram->connector(idInt2);
        }
    }
}

#endif //INCLUDE_TRAMS

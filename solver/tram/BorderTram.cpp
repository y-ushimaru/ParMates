/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "BorderTram.h"
#include "IntersectionTramExt.h"
#include "../Connector.h"

using namespace std;

//======================================================================
BorderTram::BorderTram(const AmuPoint begin, const AmuPoint end,
                       int din, int dout,
                       IntersectionTramExt* tramExt, int dir,
                       double roadsideWidth,
                       bool isReverse)
    : Border(begin, end, roadsideWidth)
{
    _inPointTram = NULL;
    _outPointTram = NULL;
    _inPoints.resize(din);
    _outPoints.resize(dout);
    if (!isReverse)
    {
        _createConnectors(tramExt, dir);
    }
    else
    {
        _createConnectorsReverse(tramExt, dir);
    }
}

//======================================================================
BorderTram::~BorderTram()
{
}

//======================================================================
const Connector* BorderTram::inPointTram()
{
    return _inPointTram;
}

//======================================================================
const Connector* BorderTram::outPointTram()
{
    return _outPointTram;
}

//======================================================================
const Connector* BorderTram::connector(int idInt)
{
    switch (idInt%100)
    {
    case 0:
        if (_inPointTram)
        {
            return _inPointTram;
        }
    case 1:
        return _outPointTram;
    default:
        break;
    }

    return NULL;
}

//======================================================================
void BorderTram::_createConnectors(IntersectionTramExt* tramExt,
                                   int dir)
{
    double sum = _inPoints.size()
        + _outPoints.size()
        + tramExt->numTotalTramLanes(dir);

    for(unsigned int i = 0; i<_inPoints.size(); i++)
    {
        int n = tramExt->numTotalTramLeft(dir) + i;
        _inPoints[i] = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    for(unsigned int i = 0; i<_outPoints.size(); i++)
    {
        int n = tramExt->numTotalTramLeft(dir)
            + _inPoints.size()
            + tramExt->numTotalTramCenter(dir) + i;
        _outPoints[i] = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    // 路面電車コネクタ
    if (tramExt->numInTramLeft(dir)>0)
    {
        int n = 0;
        _inPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numInTramCenter(dir)>0)
    {
        int n = _inPoints.size();
        _inPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numInTramRight(dir)>0)
    {
        int n = _inPoints.size() + _outPoints.size();
        _inPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    if (tramExt->numOutTramLeft(dir)>0)
    {
        int n = tramExt->numInTramLeft(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numOutTramCenter(dir)>0)
    {
        int n = _inPoints.size()
            + tramExt->numInTramCenter(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numOutTramRight(dir)>0)
    {
        int n = _inPoints.size()
            + _outPoints.size()
            + tramExt->numInTramRight(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
}

//======================================================================
void BorderTram::_createConnectorsReverse(IntersectionTramExt* tramExt,
                                          int dir)
{
    double sum = _inPoints.size()
        + _outPoints.size()
        + tramExt->numTotalTramLanes(dir);

    for(unsigned int i = 0; i<_inPoints.size(); i++)
    {
        int n = tramExt->numTotalTramRight(dir) + i;
        _inPoints[i] = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    for(unsigned int i = 0; i<_outPoints.size(); i++)
    {
        int n = tramExt->numTotalTramRight(dir)
            + _inPoints.size()
            + tramExt->numTotalTramCenter(dir) + i;
        _outPoints[i] = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    // 路面電車コネクタ
    if (tramExt->numOutTramRight(dir)>0)
    {
        int n = 0;
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numOutTramCenter(dir)>0)
    {
        int n = _inPoints.size();
        _inPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numOutTramLeft(dir)>0)
    {
        int n = _inPoints.size() + _outPoints.size();
        _inPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }

    if (tramExt->numInTramRight(dir)>0)
    {
        int n = tramExt->numOutTramRight(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numInTramCenter(dir)>0)
    {
        int n = _inPoints.size()
            + tramExt->numOutTramCenter(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
    else if (tramExt->numInTramLeft(dir)>0)
    {
        int n = _inPoints.size()
            + _outPoints.size()
            + tramExt->numOutTramLeft(dir);
        _outPointTram = _createInnerConnector(2*n+1, 2*(sum-n)-1);
    }
}

#endif //INCLUDE_TRAMS

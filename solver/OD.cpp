/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Conf.h"
#include "OD.h"
#include <iostream>
#include <vector>

using namespace std;

//======================================================================
OD::OD():_start(""),_goal("")
{
    _lastPassedStopPoint = -1;
}

//======================================================================
OD::~OD()
{
}

//======================================================================
bool OD::setValue(const std::string& start, const std::string& goal)
{
    if(start.size() == NUM_FIGURE_FOR_INTERSECTION
       && goal.size() == NUM_FIGURE_FOR_INTERSECTION)
    {
        _start = start;
        _goal = goal;
        _stopPoints.clear();
        _lastPassedStopPoint = -1;

        return true;
    }
    clear();
    return false;
}

//======================================================================
bool OD::setValue(const std::string& start, const std::string& goal,
		  const std::vector<std::string>& stopPoints)
{
    if(start.size() != NUM_FIGURE_FOR_INTERSECTION
       || goal.size() != NUM_FIGURE_FOR_INTERSECTION)
    {
        clear();
        return false;
    }

    _start = start;
    _goal = goal;

    vector<string>::const_iterator it = stopPoints.begin();

    while(it != stopPoints.end())
    {
        if((*it).size() != NUM_FIGURE_FOR_INTERSECTION)
        {
            clear();
            return false;
        }

        _stopPoints.push_back(*it);
        it++;
    }
    _lastPassedStopPoint = -1;

    return true;
}

//======================================================================
void OD::setLastPassedStopPoint(const std::string passedInter)
{
    vector<string>::iterator it = _stopPoints.begin();

    // _lastPassedStopPoint以降にpassedInterがみつかれば
    // それを_lastPassedStopPointに指定する。
    int i = _lastPassedStopPoint;
    if(i < 0) i = 0;

    for(unsigned int i = _lastPassedStopPoint + 1;
        i < _stopPoints.size();
        i++)
    {
        if(_stopPoints[i].compare(passedInter) == 0)
        {
            _lastPassedStopPoint = i;
            break;
        }
    }
}

//======================================================================
const std::string& OD::start() const
{
    return _start;
}

//======================================================================
const std::string& OD::goal() const
{
    return _goal;
}

//======================================================================
const vector<string>* OD::stopPoints() const
{
    return &_stopPoints;
}

//======================================================================
int OD::lastPassedStopPoint() const
{
    return _lastPassedStopPoint;
}
//======================================================================
void OD::clear()
{
    _start = "";
    _goal = "";
    _stopPoints.clear();
    _lastPassedStopPoint = -1;
}

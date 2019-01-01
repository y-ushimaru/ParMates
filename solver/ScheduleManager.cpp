/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "ScheduleManager.h"
#include "SpeedLimitCell.h"
#include "Section.h"
#include "Lane.h"

#include <iostream>

using namespace std;

multimap<ulint, const SpeedLimitCell*> ScheduleManager::_speedLimitList;

//======================================================================
void ScheduleManager::deleteAll()
{
    for (map<ulint, const SpeedLimitCell*>::iterator itc
             = _speedLimitList.begin();
         itc != _speedLimitList.end();
         itc++)
    {
        delete (*itc).second;
    }
    _speedLimitList.erase(_speedLimitList.begin(),
                          _speedLimitList.end());
}

//======================================================================
void ScheduleManager::addSpeedLimitCell(ulint time,
                                        const SpeedLimitCell* cell)
{
    _speedLimitList.insert(
        pair<ulint, const SpeedLimitCell*>(time, cell));
}

//======================================================================
void ScheduleManager::renewSpeedLimit()
{
    while (true)
    {
        if (_speedLimitList.empty())
        {
            break;
        }
        if ((*_speedLimitList.begin()).first>TimeManager::time())
        {
            break;
        }

        // 現時点で有効なセル
        const SpeedLimitCell* activeCell
            = (*_speedLimitList.begin()).second;

        const Section* section = activeCell->section();
        bool isUp = activeCell->isUp();
        double limit = activeCell->speedLimit();

        const RMAPLAN* lanes = section->lanes();
        for (CITRMAPLAN itl = lanes->begin();
             itl != lanes->end();
             itl++)
        {
            if (section->isUp((*itl).second) == isUp)
            {
                (*itl).second->setSpeedLimit(limit);
            }
        }

        // 必要のなくなったセルを消去する
        delete activeCell;
        _speedLimitList.erase(_speedLimitList.begin());
    }
}

//======================================================================
void ScheduleManager::print()
{
    cout << "*** Speed Limit List ***" << endl;
    for (multimap<ulint, const SpeedLimitCell*>::const_iterator itc
             = _speedLimitList.begin();
         itc != _speedLimitList.end();
         itc++)
    {
        cout << "Time: " << (*itc).first
             << "/ Section: " << (*itc).second->section()->id()
             << ", Up: " << (*itc).second->isUp()
             << ", SpeedLimit: " << (*itc).second->speedLimit()
             << endl;
    }
    cout << endl;
}

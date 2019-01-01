/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "GeneratingTable.h"
#include "GeneratingTableCell.h"
#include "Conf.h"
#include "TimeManager.h"
#include "GVManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <algorithm>

using namespace std;

//######################################################################
// GeneratingTableCellのソートに用いるクラス
class GTCellLess
{
public:
    /// GTCellの比較関数
    /**
     * 左辺のbegin（開始時刻）が右辺より小さければ真を返す．
     * 即ち，GTCellは開始時刻が早い順に整列される．
     */
    bool operator()(const GeneratingTableCell* rl,
                    const GeneratingTableCell* rr) const
    {
        return (rl->begin() < rr->begin());
    }
};

//######################################################################
GeneratingTable::GeneratingTable()
{
    _table.clear();
    _activatedTable.clear();
}

//======================================================================
GeneratingTable::~GeneratingTable()
{
    for (unsigned int i=0; i<_table.size(); i++)
    {
        delete _table[i];
    }
    _table.clear();
    for (unsigned int i=0; i<_activatedTable.size(); i++)
    {
        delete _activatedTable[i];
    }
    _activatedTable.clear();
}

//======================================================================
void GeneratingTable::addGTCell(GeneratingTableCell* cell)
{
    _table.push_back(cell);
}

//======================================================================
void GeneratingTable::createGTCell
(ulint begin, ulint end, double volume, int vehicleType,
 const std::vector<std::string>& gates)
{
    GeneratingTableCell* cell = new GeneratingTableCell;
    cell->setValue(begin, end, volume, vehicleType, gates);
    _table.push_back(cell);
}

//======================================================================
void GeneratingTable::extractActiveGTCells
(std::vector<const GeneratingTableCell*>* result_GTCells)
{
    // _tableは開始時刻でソートされているので，先頭から要素を調査する
    while (_table.size()>0)
    {
        if (_table.front()->begin() <= TimeManager::time())
        {
            // _tableの先頭から要素を取り除き_activatedTableに追加する
            GeneratingTableCell* activatedGTCell = _table.front();
            _activatedTable.push_back(activatedGTCell);
            result_GTCells->push_back(activatedGTCell);
            _table.pop_front();
        }
        else
        {
            break;
        }
    } 
}

//======================================================================
void GeneratingTable::extractActiveGTCellsAllAtOnce
(std::vector<const GeneratingTableCell*>* result_GTCells)
{
    // _tableは開始時刻でソートされているので，先頭から要素を調査する
    while (_table.size()>0)
    {
        // _tableの先頭から要素を取り除き_activatedTableに追加する
        GeneratingTableCell* activatedGTCell = _table.front();
        _activatedTable.push_back(activatedGTCell);
        result_GTCells->push_back(activatedGTCell);
        _table.pop_front();
    } 
}

//======================================================================
void GeneratingTable::getValidGTCells
(const std::string& intersectionId,
 std::vector<const GeneratingTableCell*>* result_GTCells) const
{
    for(deque<GeneratingTableCell*>::const_iterator where=_table.begin();
        where!=_table.end();
        where++)
    {
        if((*where)->origin().compare(intersectionId)==0)
        {
            (*result_GTCells).push_back((*where));
        }
    }
}

//======================================================================
const GeneratingTableCell* GeneratingTable::validGTCell
(const std::string& intersectionId) const
{
    for(deque<GeneratingTableCell*>::const_iterator where=_table.begin();
        where!=_table.end();
        where++)
    {
        if((*where)->origin().compare(intersectionId)==0)
        {
            return (*where);
        }
    }
    return NULL;
}

//======================================================================
void GeneratingTable::sortGTCells()
{
    sort(_table.begin(), _table.end(), GTCellLess());
}

//======================================================================
void GeneratingTable::print() const
{
    cout << "Table Unused:" << endl;
    for (unsigned int i=0; i<_table.size(); i++)
    {
        _table[i]->print();
    }
    cout << "Table Activated:" << endl;
    for (unsigned int i=0; i<_activatedTable.size(); i++)
    {
        _activatedTable[i]->print();
    }
}

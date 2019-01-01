/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "GeneratingTableCell.h"
#include "Conf.h"
#include "TimeManager.h"
#include "GVManager.h"
#include "AmuStringOperator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <algorithm>

using namespace std;

//======================================================================
GeneratingTableCell::GeneratingTableCell():
    _begin(0), _end(0), _volume(0),
    _isPaired(false),
    _isOriginGrouped(false), _isDestinationGrouped(false){}

//======================================================================
bool GeneratingTableCell::setValue(ulint begin, ulint end,
                                   double volume, int vehicleType,
                                   const vector<string>& gates)
{
    if (begin <= end && volume >= 0)
    {
        _begin = begin;
        _end = end;
        _volume = volume;
        _vehicleType = vehicleType;
    }
    else
    {
        _begin = 0;
        _end = 0;
        _gates.clear();
        return false;
    }
    for (unsigned int i=0; i<gates.size(); i++)
    {
        _gates.push_back(gates[i]);
    }

    return true;
}

//======================================================================
void GeneratingTableCell::print() const
{
    cout << "beginTime: " << _begin
         << ", endTime: " << _end
         << ", volume: " << _volume << endl;
    cout << "origin: " << origin()
         << ", destination: " << destination() << endl;
    cout << "gates: ";

    if (_gates.size()==2)
    {
        cout << "none";
    }
    else
    {
        for (unsigned int i=1; i<_gates.size()-1; i++)
        {
            cout << _gates[i] << " ";
        }
    }
    cout << endl;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <cassert>
#include <algorithm>
#include "SignalControlData.h"
#include "Conf.h"

using namespace std;

const int NUM_MAX_SPLIT_INT = NUM_MAX_SPLIT;
const ulint NUM_MAX_SPLIT_ULINT = NUM_MAX_SPLIT;

//======================================================================
SignalControlData::SignalControlData()
{
    _split.resize(NUM_MAX_SPLIT_ULINT, 0);
    _begin = 0;
    _end   = 0;
    _cycle = 0;
}

//======================================================================
SignalControlData::SignalControlData(ulint begin, ulint end, ulint cycle,
				     std::vector<ulint>& split)
{
    _begin = begin;
    _end   = end;
    _cycle = cycle; 
    assert(split.size() == NUM_MAX_SPLIT_ULINT);
    _split.resize(NUM_MAX_SPLIT_ULINT);
    copy(split.begin(), split.end(), _split.begin());
}
//======================================================================
SignalControlData::~SignalControlData(){}

//======================================================================
ulint SignalControlData::begin() const
{
    return _begin;
}

//======================================================================
ulint SignalControlData::end() const
{
    return _end;
}

//======================================================================
ulint SignalControlData::cycle() const
{
    return _cycle;
}

//======================================================================
ulint SignalControlData::split(int splitNum) const
{
    assert(splitNum >= 0);
    assert(splitNum < NUM_MAX_SPLIT_INT);
    return _split[splitNum];
}


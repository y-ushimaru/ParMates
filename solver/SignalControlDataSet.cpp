/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <assert.h>
#include <iostream>
#include "SignalControlDataSet.h"

using namespace std;

//======================================================================
SignalControlDataSet::SignalControlDataSet(){}

//======================================================================
SignalControlDataSet::SignalControlDataSet(
    std::vector<SignalControlData>& dataSet)
{
    assert(dataSet.size() != 0);
    _dataSet.resize(dataSet.size());
    copy(dataSet.begin(), dataSet.end(), _dataSet.begin());
    _index = _dataSet.begin();
}

//======================================================================
SignalControlDataSet::~SignalControlDataSet(){}

//======================================================================
SignalControlDataSet &SignalControlDataSet::operator=(
    const SignalControlDataSet& dat)
{
    if (this == &dat)
    {
        return *this;
    }
    assert(dat._dataSet.size() != 0);
    _dataSet.resize(dat._dataSet.size());
    copy(dat._dataSet.begin(), dat._dataSet.end(), _dataSet.begin());
    _index = _dataSet.begin();
    return *this;
}

//======================================================================
const SignalControlData& SignalControlDataSet::aspectData(ulint time) const
{
    if (time < (*_index).end() && time >= (*_index).begin())
    {
        // 現在の_indexの指すSignalControllDataの時間内にいる場合
        // 処理は必要ない。
    }
    else
    {
        // 現在のindexを移動する必要がある場合
        // 範囲内に納まっているところを探す。
        _index = lower_bound(_dataSet.begin(),
                             _dataSet.end(),
                             time,
                             SignalControlDataSetCmp());
        if (_index == _dataSet.end())
        {
            _index--;
        }
        else
        {
            if ((*_index).end() <= time)
            { 
                cerr << "aspectData: index time is too big! : time = " << time << endl;
            }
            assert((*_index).end() > time);
        }
        if((*_index).begin() > time) 
        {
            cerr << "aspectData: index time is too small! : time = " << time << endl;
        }
        assert((*_index).begin() <= time);
    }
    return *_index;
}

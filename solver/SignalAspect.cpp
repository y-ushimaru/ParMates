/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <cassert>
#include "SignalAspect.h"

using namespace std;

//======================================================================
SignalAspect::SignalAspect(){}

//======================================================================
SignalAspect::SignalAspect(std::vector<Triplet> state)
{
    bool result = setSignalState(state);
    if (!result)
    {
        assert(0);
    }
}

//======================================================================
SignalAspect::~SignalAspect(){}

//======================================================================
bool SignalAspect::setSignalState(std::vector<Triplet> state)
{
    bool result = false;
    if (_numSignalIsValid(state.size()))
    {
        _state.resize(state.size());
        copy(state.begin(), state.end(), _state.begin());
        result = true;
    }
    return result;
}

//======================================================================
bool SignalAspect::isReady()
{
    return _numSignalIsValid(_state.size());
}

//======================================================================
int SignalAspect::numDirections() const
{
    return _state.size();
}

//======================================================================
bool SignalAspect::_numSignalIsValid(const int numSignalElement)
{
    bool result = false;
    // 交差点の分岐数に対応
    if(numSignalElement >= 2
       && numSignalElement <= 10)
    {
        result = true;
    } 
    return result;
}

//======================================================================
bool SignalAspect::_isValidRequest(const int direction) const
{
    if(direction>=0
       && direction<static_cast<signed int>(_state.size()))
    {
        return true;
    }
    cerr << "ERROR: invalid direction " << direction << endl;
    return false;
}

//======================================================================
SignalMainState SignalAspect::mainColor(const int direction) const
{
    assert(_isValidRequest(direction));
    return _state[direction].main;
}

//======================================================================
SignalSubState SignalAspect::subColor(const int direction) const
{
    assert(_isValidRequest(direction));
    return _state[direction].sub;
}

//======================================================================
SignalWalkerState SignalAspect::walkerColor(const int direction) const
{
    assert(_isValidRequest(direction));
    return _state[direction].walker;
}

//======================================================================
Triplet SignalAspect::colors(const int direction) const
{
    assert(this->_isValidRequest(direction));
    return _state[direction];
}

//======================================================================
void SignalAspect::crt() const
{
    for(int i=0; i<(int)_state.size();i++)
    {
        cout << "  direction = " << i 
             << " : main = " << mainColor(i) 
             << " : sub = " << subColor(i) 
             << " : walker = "<< walkerColor(i) << endl;
    }
}

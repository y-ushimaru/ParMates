/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Route.h"
#include "Intersection.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

//======================================================================
Route::Route()
{
    _lastPassedIntersectionIndex = 0;
    _lastPassedGateIndex         = 0;
    _gates.clear();
    _route.clear();
}

//======================================================================
Route::~Route(){}

//======================================================================
void Route::setGates(vector<const Intersection*>* gates)
{
    _gates.clear();
    _gates.insert(_gates.end(), gates->begin(), gates->end());
}

//======================================================================
void Route::addGate(const Intersection* gate)
{
    _gates.push_back(gate);
}

//======================================================================
const vector<const Intersection*>* Route::gates() const
{
    return &_gates;
}

//======================================================================
int Route::lastPassedGateIndex() const
{
    return _lastPassedGateIndex;
}

//======================================================================
void Route::setLastPassedGate(const Intersection* gate)
{
    for (unsigned int i=_lastPassedGateIndex+1; i<_gates.size(); i++)
    {
        if (_gates[i]==gate)
        {
            _lastPassedGateIndex = i;
            break;
        }
    }
}

//======================================================================
void Route::setLastPassedGateIndex(int lastPassedGateIndex)
{
  _lastPassedGateIndex = lastPassedGateIndex;
}


//======================================================================
const Intersection* Route::start() const
{
    return _gates[0];
}

//======================================================================
const Intersection* Route::goal() const
{
    return _gates[_gates.size()-1];
}

//======================================================================
void Route::resetRoute()
{
    _route.clear();
}

//======================================================================
void Route::addIntersection(const Intersection* inter)
{
    _route.push_back(inter);
}

//======================================================================
const vector<const Intersection*>* Route::route() const
{
    return &_route;
}

//======================================================================
int Route::lastPassedIntersectionIndex() const
{
    return _lastPassedIntersectionIndex;
}

//======================================================================
void Route::setLastPassedIntersection(const Intersection* from,
                                      const Intersection* last)
{
    if (!from
        || _lastPassedIntersectionIndex<0)
    {
        setLastPassedIntersection(last);
        return;
    }

    for (unsigned int i=_lastPassedIntersectionIndex+1;
         i<_route.size();
         i++)
    {
        if (_route[i-1]==from && _route[i]==last)
        {
            _lastPassedIntersectionIndex = i;
            break;
        }
    }
    setLastPassedGate(last);
}

//======================================================================
void Route::setLastPassedIntersection(const Intersection* last)
{
    for (unsigned int i=_lastPassedIntersectionIndex+1;
         i<_route.size();
         i++)
    {
        if (_route[i]==last)
        {
            _lastPassedIntersectionIndex = i;
            break;
        }
    }
    setLastPassedGate(last);
}

//======================================================================
void Route::setLastPassedIntersectionIndex(int lastPassedIntersectionIndex)
{
  _lastPassedIntersectionIndex = lastPassedIntersectionIndex;
}

//======================================================================
void Route::resetLastPassedIntersectionIndex()
{
    _lastPassedIntersectionIndex = -1;
}

//======================================================================
void Route::popBack()
{
    _route.pop_back();
}

//======================================================================
const Intersection* Route::next(const Intersection* inter) const
{
    assert(inter);

    const Intersection* result = NULL;
    unsigned int offset;
    if(_lastPassedIntersectionIndex < 1)
    {
        offset = 0;
    }
    else if (_lastPassedIntersectionIndex
             < static_cast<int>(_route.size()))
    {
        offset = _lastPassedIntersectionIndex - 1;
    }
    else
    {
        // ここに到達することはないはずだが...
        offset = 0;
    }

    for (unsigned int i=offset; i<_route.size()-1; i++)
    {
        if (_route[i] == inter)
        {
            result = _route[i+1];
            break;
        }
    }

    return result;
}

//======================================================================
const Intersection* Route::next(const Intersection* rear,
                                const Intersection* front) const
{
    assert(rear && front);

    const Intersection* result = NULL;
    unsigned int offset;
    if(_lastPassedIntersectionIndex < 1)
    {
        offset = 0;
    }
    else if (_lastPassedIntersectionIndex
             < static_cast<int>(_route.size()))
    {
        offset = _lastPassedIntersectionIndex - 1;
    }
    else
    {
        // ここに到達することはないはずだが...
        offset = 0;
    }

    for (unsigned int i=offset; i<_route.size()-2; i++)
    {
        if (_route[i] == rear
            && _route[i+1] == front)
        {
            result = _route[i+2];
            break;
        }
    }

    return result;
}

//======================================================================
int Route::size() const
{
    return _route.size();
}

//======================================================================
void Route::print(std::ostream& out) const
{
    out << "Gate : lastly passed gate index = "
        << _lastPassedGateIndex << endl;
    for (int i=0; i< static_cast<signed int>(_gates.size()); i++)
    {
        if (i == _lastPassedGateIndex)
        {
            out << "\t" << _gates[i]->id() << "<<" << endl;
        }
        else
        {
            out << "\t" << _gates[i]->id() << endl;
        }
    }

    out << "Route: lastly passed intersection index = "
        << _lastPassedIntersectionIndex << endl;
    for(int i = 0; i < static_cast<signed int>(_route.size()); i++)
    {
        if (i == _lastPassedIntersectionIndex)
        {
            out << "\t" << _route[i]->id() << "<<" << endl;
        }
        else
        {
            out << "\t" << _route[i]->id() << endl;
        }
    }
}

#ifdef NODEF
//======================================================================
void Route::push(const Intersection* isec)
{
    if(isec != NULL)
    {
        _route.push_back(isec);
    }
}

//======================================================================
void Route::push(std::vector<const Intersection*>& isec_vec)
{
    if(isec_vec.size() == 0)
    {
        return;
    }

    // 最後の交差点が重複しないように気を付けながら末尾に追加
    if(_route.size() == 0)
    {
        _route.insert(_route.end(), isec_vec.begin(), isec_vec.end());
    }
    else
    {
        vector<const Intersection*>::iterator it = _route.end();
        vector<const Intersection*>::iterator it_in = isec_vec.begin();

        it--; // 最後尾の交差点を取得
        // 最後尾と先頭を比較して同一だったら追加開始位置をひとつずらす。
        if((*it)->id().compare((*it_in)->id()) == 0)
        {
            it_in++;
        }
        _route.insert(_route.end(), it_in, isec_vec.end());
    }
}

//======================================================================
const Intersection* Route::pop()
{
    const Intersection* result = NULL;
    if(! _route.empty())
    {
        result = _route.back();
        _route.pop_back();
    }
    return result;
}

//======================================================================
const Intersection* Route::back()
{
    return _route.back();
}

//======================================================================
const Intersection* Route::prev(const Intersection* isec) const
{
    const Intersection* result = NULL;
    vector<const Intersection*>::const_iterator where;
    where = find(_route.begin(), _route.end(), isec);
    if (where != _route.begin())
    {
        where--;
        result = (*where);
    }

    return result;
}

//======================================================================
vector<const Intersection*>* Route::route() const
{
    return &_route;
}

//======================================================================
const Intersection* Route::inter(int num) const
{
    const Intersection* result = NULL;

    if(num < static_cast<signed int>(_route.size()))
    {
        result = _route[num];
    }

    return result;
}

//======================================================================
double Route::cost() const
{
    return _cost;
}

//======================================================================
void Route::setCost(double c)
{
    if(c >= 0.0)
    {
        _cost = c;
    }
}
#endif

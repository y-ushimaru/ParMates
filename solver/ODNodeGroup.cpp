/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "ODNodeGroup.h"
#include "ODNode.h"
#include "Random.h"
#include <iomanip>

using namespace std;

//======================================================================
ODNodeGroup::ODNodeGroup()
{
    _rnd  = Random::randomGenerator();
    _type = UNDEFINED;
    _sumTotal = 0;
}

//======================================================================
ODNodeGroup::ODNodeGroup(const string& id)
{
    _id   = id;
    _rnd  = Random::randomGenerator();
    _type = UNDEFINED;
    _sumTotal = 0;
}

//======================================================================
ODNodeGroup::~ODNodeGroup()
{
    for (unsigned int i=0; i<_odSolos.size(); i++)
    {
        delete _odSolos[i];
    }
    _odSolos.clear();

    for (unsigned int i=0; i<_odPairs.size(); i++)
    {
        delete _odPairs[i];
    }
    _odPairs.clear();

    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
const string& ODNodeGroup::id() const
{
    return _id;
}

//======================================================================
void ODNodeGroup::addODSolo(ODNode* node, double weight)
{
    WeightedODSolo* solo = new WeightedODSolo;
    solo->node   = node;
    solo->weight = weight;
    _odSolos.push_back(solo);
    _sumTotal += weight;

    if (_type == UNDEFINED)
    {
        _type = SOLO;
    }
    else if (_type == PAIR)
    {
        cerr << "WARNING: ODSolo added to ODPair container."
             << endl;
    }
}

//======================================================================
void ODNodeGroup::getODSolo(ODNode** result_node)
{
    double rnd  = Random::uniform(_rnd, 0.0, _sumTotal);
    *result_node = NULL;

    for (unsigned int i=0; i<_odSolos.size(); i++)
    {
        if (rnd <= _odSolos[i]->weight)
        {
            *result_node = _odSolos[i]->node;
            break;
        }
        rnd -= _odSolos[i]->weight;
    }
    if (!(*result_node))
    {
        *result_node = _odSolos[0]->node;
    }
}

//======================================================================
void ODNodeGroup::addODPair(ODNode* start, ODNode* goal, double weight)
{
    WeightedODPair* pair = new WeightedODPair;
    pair->start  = start;
    pair->goal   = goal;
    pair->weight = weight;
    _odPairs.push_back(pair);
    _sumTotal += weight;

    if (_type == UNDEFINED)
    {
        _type = PAIR;
    }
    else if (_type == SOLO)
    {
        cerr << "WARNING: ODPair added to ODSolo container."
             << endl;
    }
}

//======================================================================
void ODNodeGroup::getODPair(ODNode** result_start,
                            ODNode** result_goal)
{
    double rnd  = Random::uniform(_rnd, 0.0, _sumTotal);
    *result_start = NULL;
    *result_goal  = NULL;

    for (unsigned int i=0; i<_odPairs.size(); i++)
    {
        if (rnd <= _odPairs[i]->weight)
        {
            *result_start = _odPairs[i]->start;
            *result_goal  = _odPairs[i]->goal;
            break;
        }
        rnd -= _odPairs[i]->weight;
    }
    if (!(*result_start) || !(*result_goal))
    {
        *result_start = _odPairs[0]->start;
        *result_goal  = _odPairs[0]->goal;
    }
}

//======================================================================
void ODNodeGroup::print() const
{
    cout << "Group ID: " << _id
         << ", Group Type: ";
    switch(_type)
    {
    case UNDEFINED:
        cout << "UNDEFINED";
        break;
    case SOLO:
        cout << "SORO";
        break;
    case PAIR:
        cout << "PAIR";
        break;
    }
    cout << endl;
    cout << "Members:" << endl;

    if (_type==SOLO)
    {
        for (unsigned int i=0; i<_odSolos.size(); i++)
        {
            cout << "  " << setw(6) << _odSolos[i]->weight
                 << ", " << _odSolos[i]->node->id() << endl;
        }
    }
    else if (_type==PAIR)
    {
        for (unsigned int i=0; i<_odPairs.size(); i++)
        {
            cout << "  " << setw(6) << _odPairs[i]->weight
                 << ", " << _odPairs[i]->start->id()
                 << ", " << _odPairs[i]->goal->id() << endl;
        }
    }
}

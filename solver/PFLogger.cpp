/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "PFLogger.h"
#include "PFSearchNodeBase.h"
#include "RoutingNode.h"
#include <iostream>

using namespace std;

//======================================================================
PFLogger::PFLogger()
{
    _snodeId2RoutingNode.clear();
    _log.clear();
}

//======================================================================
PFLogger::~PFLogger()
{
    _snodeId2RoutingNode.clear();

    for (unsigned int i=0; i<_log.size(); i++)
    {
        delete _log[i];
    }
    _log.clear();
}

//======================================================================
void PFLogger::addSnodeId2RoutingNode(const string& snodeId,
                                      const RoutingNode* rnode)
{
    _snodeId2RoutingNode.insert(make_pair(snodeId, rnode));
}

//======================================================================
void PFLogger::clearSnodeId2RoutingNode()
{
    _snodeId2RoutingNode.clear();
}

//======================================================================
bool PFLogger::isEmpty() const
{
    return _log.empty();
}

//======================================================================
unsigned int PFLogger::logSize() const
{
    return _log.size();
}

//======================================================================
void PFLogger::reset()
{
    _snodeId2RoutingNode.clear();
    _log.clear();
}

//======================================================================
void PFLogger::addLog(PFSearchNodeBase* snode)
{
    PFSearchNodeBase* newNode = new PFSearchNodeBase;
    newNode->setId(snode->id());
    newNode->setRoutingNode(snode->routingNode());
    newNode->setPrevId(snode->prevId());
    newNode->setCost(snode->cost());
    newNode->setLabel(snode->label());
    _log.push_back(newNode);
}

//======================================================================
const vector<PFSearchNodeBase*>* PFLogger::log() const
{
    return &_log;
}

//======================================================================
const map<const std::string, const RoutingNode*>*
PFLogger::snodeId2RoutingNode() const
{
    return &_snodeId2RoutingNode;
}

//======================================================================
void PFLogger::printLog() const
{
    cout << "*** Pathfinder Log ***" << endl;
    for (unsigned int i=0; i<_log.size(); i++)
    {
        cout << i << ": "
             << _log[i]->id() << " <- "
             << _log[i]->prevId() << "("
             << _log[i]->cost() << ")/"
             << _log[i]->label() << endl;
    }
    cout << endl;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "Conf.h"
#include "Intersection.h"
#include "AmuLineSegment.h"
#include "AmuVector.h"
#include "AmuConverter.h"
#include "Section.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cassert>

using namespace std;

//======================================================================
RoutingNode::RoutingNode(const Section* sect, bool isUp)
    : _sect(sect),
      _isUp(isUp)
{
    _coord = sect->center();

    /*
     *_coordは可視化上の位置を示す（経路探索の計算には用いられない）．
     * 点が重ならないよう，isUpの真偽で位置をずらす．
     */
    AmuVector vec
        = AmuVector(sect->intersection(false)->center(),
                    sect->intersection(true)->center());
    vec.normalize();

    if (isUp)
    {
        vec.revoltXY(M_PI_2);
    }
    else
    {
        vec.revoltXY(-M_PI_2);
    }
    _coord += vec*1.5;

    /*
    if (isUp)
    {
        _rank = sect->upWidth();
    }
    else
    {
        _rank = sect->downWidth();
    }
    */
    _rank = max(sect->upWidth(), sect->downWidth());

    _subId
        = sect->intersection(!isUp)->id()
        + sect->intersection(isUp)->id();
   
    _networkRank = 0; // 初期値

    _id = AmuConverter::itos(_networkRank, NUM_FIGURE_FOR_ROUTING_LAYER)
        + _subId;

    _inlinks.clear();
    _outlinks.clear();
    _upwardLink = NULL;
    _downwardLink = NULL;

    _upperNode  = NULL;
    _lowerNode  = NULL;
    _lowestNode = NULL;

    _whitelistedVehicleTypes.clear();
    _blacklistedVehicleTypes.clear();
    _routingProbability.clear();
}

//======================================================================
RoutingNode::~RoutingNode()
{
}

//====================================================================== 
string RoutingNode::id() const
{
    return _id;
}

//======================================================================
string RoutingNode::subId() const
{
    return _subId;
}

//======================================================================
unsigned int RoutingNode::rank() const
{
    return _rank;
}

//======================================================================
void RoutingNode::setRank(unsigned int rank)
{
    _rank = rank;
}

//======================================================================
unsigned int RoutingNode::networkRank() const
{
    return _networkRank;
}

//======================================================================
void RoutingNode::setNetworkRank(unsigned int networkRank)
{
    _networkRank = networkRank;
    _id = AmuConverter::itos(_networkRank, NUM_FIGURE_FOR_ROUTING_LAYER)
        + _subId;
}

//======================================================================
const Section* RoutingNode::section() const
{
    return _sect;
}

//======================================================================
bool RoutingNode::isUp() const
{
    return _isUp;
}

//======================================================================
const AmuPoint RoutingNode::point() const
{
    return _coord;
}

//======================================================================
void RoutingNode::addInLink(RoutingLink* link)
{
    _inlinks.push_back(link);
}

//======================================================================
const vector<RoutingLink*>* RoutingNode::inLinks() const
{
    return &_inlinks;
}

//======================================================================
RoutingLink* RoutingNode::inLink(const RoutingNode* prev) const
{
    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        if (_inlinks[i]->beginNode()==prev)
        {
            return _inlinks[i];
        }
    }
    return NULL;
}

//======================================================================
void RoutingNode::getInLinks
(unsigned int rank,
 vector<const RoutingLink*>* result_links) const
{
    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        if (_inlinks[i]->beginNode()->rank()>=rank)
        {
            result_links
                ->push_back(_inlinks[i]);
        }
    }
}

//======================================================================
void RoutingNode::getInNodes
(unsigned int rank,
 vector<const RoutingNode*>* result_nodes) const
{
    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        if (_inlinks[i]->beginNode()->rank()>=rank)
        {
            result_nodes
                ->push_back(_inlinks[i]->beginNode());
        }
    }
}

//======================================================================
const RoutingNode* RoutingNode::straightInNode() const
{
    const RoutingNode* result = NULL;
    double minAngle = 4; //angleは-PIからPIまで

    const AmuVector outvec
        (_lowestNode->section()->intersection(!_isUp)->center(),
         _lowestNode->section()->intersection(_isUp)->center());

    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        const AmuVector invec
            (_inlinks[i]->beginNode()->point(),
             _inlinks[i]->endNode()->point());
        double angle = outvec.calcAngle(invec);
        if (fabs(angle)<minAngle)
        {
            minAngle = angle;
            result = _inlinks[i]->beginNode();
        }
    }
    return result;
}

//======================================================================
int RoutingNode::indegree(unsigned int rank) const
{
    int indeg = 0;
    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        if (_inlinks[i]->beginNode()->rank()>=rank)
        {
                indeg++;
        }
    }
    return indeg;
}

//======================================================================
int RoutingNode::indegreeInSameNetworkRank() const
{
    int indeg = 0;
    for (unsigned int i=0; i<_inlinks.size(); i++)
    {
        if (_inlinks[i]->beginNode()->networkRank()==_networkRank)
        {
                indeg++;
        }
    }
    return indeg;
}

//======================================================================
void RoutingNode::addOutLink(RoutingLink* link)
{
    _outlinks.push_back(link);
}

//======================================================================
const vector<RoutingLink*>* RoutingNode::outLinks() const
{
    return &_outlinks;
}

//======================================================================
RoutingLink* RoutingNode::outLink(const RoutingNode* next) const
{
    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        if (_outlinks[i]->endNode()==next)
        {
            return _outlinks[i];
        }
    }
    return NULL;
}

//======================================================================
void RoutingNode::addUpwardLink(RoutingLink* link)
{
    _upwardLink = link;
}

//======================================================================
RoutingLink* RoutingNode::upwardLink() const
{
    return _upwardLink;
}

//======================================================================
void RoutingNode::addDownwardLink(RoutingLink* link)
{
    _downwardLink = link;
}

//======================================================================
RoutingLink* RoutingNode::downwardLink() const
{
    return _downwardLink;
}

//======================================================================
void RoutingNode::getOutLinks
(unsigned int rank,
 vector<const RoutingLink*>* result_links) const
{
    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        if (_outlinks[i]->endNode()->rank()>=rank)
        {
            result_links
                ->push_back(_outlinks[i]);
        }
    }
}

//======================================================================
void RoutingNode::getOutNodes
(unsigned int rank,
 vector<const RoutingNode*>* result_nodes) const
{
    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        if (_outlinks[i]->endNode()->rank()>=rank)
        {
            result_nodes
                ->push_back(_outlinks[i]->endNode());
        }
    }
}

//======================================================================
const RoutingNode* RoutingNode::straightOutNode() const
{
    const RoutingNode* result = NULL;
    double minAngle = 4; //angleは-PIからPIまで

    const AmuVector invec
        (_lowestNode->section()->intersection(!_isUp)->center(),
         _lowestNode->section()->intersection(_isUp)->center());

    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        const AmuVector outvec
            (_outlinks[i]->beginNode()->point(),
             _outlinks[i]->endNode()->point());
        double angle = invec.calcAngle(outvec);
        if (fabs(angle)<minAngle)
        {
            minAngle = angle;
            result = _outlinks[i]->endNode();
        }
    }

    return result;
}

//======================================================================
int RoutingNode::outdegree(unsigned int rank) const
{
    int outdeg = 0;
    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        if (_outlinks[i]->endNode()->rank()>=rank)
        {
            outdeg++;
        }
    }
    return outdeg;
}

//======================================================================
int RoutingNode::outdegreeInSameNetworkRank() const
{
    int outdeg = 0;
    for (unsigned int i=0; i<_outlinks.size(); i++)
    {
        if (_outlinks[i]->endNode()->networkRank()==_networkRank)
        {
            outdeg++;
        }
    }
    return outdeg;
}

//======================================================================
void RoutingNode::setUpperNode(RoutingNode* node)
{
    _upperNode = node;
}

//======================================================================
RoutingNode* RoutingNode::upperNode() const
{
    return _upperNode;
}

//======================================================================
void RoutingNode::setLowerNode(RoutingNode* node)
{
    _lowerNode = node;
}

//======================================================================
RoutingNode* RoutingNode::lowerNode() const
{
    return _lowerNode;
}

//======================================================================
void RoutingNode::setLowestNode(RoutingNode* node)
{
    _lowestNode = node;
}

//======================================================================
RoutingNode* RoutingNode::lowestNode() const
{
    return _lowestNode;
}

//======================================================================
void RoutingNode::setProperty()
{
    const vector<string>* wl = _sect->whitelistedVehicleTypes(_isUp);
    for (unsigned int i=0; i<wl->size(); i++)
    {
        _whitelistedVehicleTypes.push_back(wl->at(i));
    }

    const vector<string>* bl = _sect->blacklistedVehicleTypes(_isUp);
    for (unsigned int i=0; i<bl->size(); i++)
    {
        _blacklistedVehicleTypes.push_back(bl->at(i));
    }

    const map<VehicleType, double>* rp
        = _sect->routingProbabilities(_isUp);
    map<VehicleType, double>::const_iterator citv;
    for (citv = rp->begin(); citv != rp->end(); citv++)
    {
        _routingProbability[(*citv).first] = (*citv).second;
    }
}

//======================================================================
const vector<string>* RoutingNode::whitelistedVehicleTypes() const
{
    return &_whitelistedVehicleTypes;
}

//======================================================================
const vector<string>* RoutingNode::blacklistedVehicleTypes() const
{
    return &_blacklistedVehicleTypes;
}

//======================================================================
const map<VehicleType, double>* RoutingNode::routingProbability() const
{
    return &_routingProbability;
}

//======================================================================
bool RoutingNode::mayPass(VehicleType type)
{
    stringstream ss;
    ss.str("");
    ss << type;
    
    if (!(_whitelistedVehicleTypes.empty()))
    {
        // ホワイトリスト登録済み - ホワイトリストを優先する
        vector<string>::iterator its
            = find(_whitelistedVehicleTypes.begin(),
                   _whitelistedVehicleTypes.end(),
                   ss.str());
        if (its != _whitelistedVehicleTypes.end())
        {
            return true;
        }
        return false;
    }
    else
    {
        // ホワイトリストに何も登録がない - ブラックリストに従う
        vector<string>::iterator its
            = find(_blacklistedVehicleTypes.begin(),
                   _blacklistedVehicleTypes.end(),
                   ss.str());
        if (its != _blacklistedVehicleTypes.end())
        {
            return false;
        }
        return true;
    }
}

//======================================================================
double RoutingNode::routingProbability(VehicleType type) const
{
    double result = 1.0;
    map<VehicleType, double>::const_iterator citv
        = _routingProbability.find(type);
    if (citv != _routingProbability.end())
    {
        result = (*citv).second;
    }
    return result;
}

//======================================================================
void RoutingNode::print() const
{
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "Section.h"
#include "Intersection.h"
#include "AmuConverter.h"
#include "Conf.h"
#include <iostream>
#include <cassert>

using namespace std;

//======================================================================
RoutingNetwork::RoutingNetwork()
{
    _nodes.clear();
    _links.clear();
    _upperNetwork = NULL;
    _lowerNetwork = NULL;
    _maxNodeRank = 0;
}

//======================================================================
RoutingNetwork::~RoutingNetwork()
{
    // ノードのdelete&clear
    for (ITRNMAPN itn = _nodes.begin(); itn != _nodes.end(); itn++)
    {
        delete (*itn).second;
    }
    _nodes.clear();

    // リンクのdelete&clear
    for (unsigned int i=0; i < _links.size(); i++)
    {
        delete _links[i];
    }
    _links.clear();
}

//======================================================================
const RNMAPN* RoutingNetwork::nodes() const
{
    return &_nodes;
}

//======================================================================
RoutingNode* RoutingNetwork::node(string& id) const
{
    CITRNMAPN itn = _nodes.find(id);
    if (itn != _nodes.end())
    {
        return (*itn).second;
    }
    else
    {
        return NULL;
    }
}

//======================================================================
void RoutingNetwork::addNode(RoutingNode* node)
{
    _nodes.insert(make_pair(node->id(), node));
}

//======================================================================
RoutingNode* RoutingNetwork::convertS2N(const Section* sect,
                                        bool isUp) const
{
    // IDベースで検索
    string id = sect->id();
    string beginId
        = id.substr(0, NUM_FIGURE_FOR_INTERSECTION);
    string endId
        = id.substr(NUM_FIGURE_FOR_INTERSECTION,
                    NUM_FIGURE_FOR_INTERSECTION);
    string nodeId;
    if (isUp)
    {
        nodeId
            = AmuConverter::itos(_networkRank,
                                 NUM_FIGURE_FOR_ROUTING_LAYER)
            + beginId + endId;
    }
    else
    {
        nodeId
            = AmuConverter::itos(_networkRank,
                                 NUM_FIGURE_FOR_ROUTING_LAYER)
            + endId + beginId;
    }
    
    CITRNMAPN citn = _nodes.find(nodeId);
    if (citn != _nodes.end())
    {
        return (*citn).second;
    }
    else
    {
        cout << "RoutingNode:" << nodeId << " not found!!" << endl;
        cout << " Section:" << sect->id() << endl;
        cout << " isUp:" << isUp << endl;
        cout << " (begin, end): ("
             << beginId << ", " << endId << ")" << endl;
        return NULL;
    }
}

//======================================================================
const vector<RoutingLink*>* RoutingNetwork::links() const
{
    return & _links;
}

//======================================================================
RoutingLink* RoutingNetwork::link(string& id) const
{
    for (unsigned int i=0; i<_links.size(); i++)
    {
        if (_links[i]->id() == id)
        {
            return _links[i];
        }
    }
    return NULL;
}

//======================================================================
RoutingLink* RoutingNetwork::link(RoutingNode* begin,
                                  RoutingNode* end) const
{
    for (unsigned int i=0; i<_links.size(); i++)
    {
        if (_links[i]->beginNode() == begin
            && _links[i]->endNode() == end)
        {
            return _links[i];
        }
    }
    return NULL;
}

//======================================================================
void RoutingNetwork::addLink(RoutingLink* link)
{
    _links.push_back(link);
}

//======================================================================
RoutingLink* RoutingNetwork::convertI2L(const Intersection* via,
                                        const Intersection* from,
                                        const Intersection* to) const
{
    for (unsigned int i=0; i<_links.size(); i++)
    {
        if (_links[i]->intersection() == via
            && _links[i]->beginNode()->section()
            ->anotherIntersection(via) == from
            && _links[i]->endNode()->section()
            ->anotherIntersection(via) == to )
        {
            return _links[i];
        }
    }

    cerr
        << "RoutingNetwork::convertI2L(via:" << via->id()
        << ", from:" << from->id()
        << ", to:" << to->id() << " failed." << std::endl;
    return NULL;
}

//======================================================================
bool RoutingNetwork::setInitialCosts()
{
    for (unsigned int i=0; i<_links.size(); i++)
    {
        _links[i]->setInitialCosts();
    }
    return true;
}

//======================================================================
bool RoutingNetwork::renewCosts()
{
    for (unsigned int i=0; i<_links.size(); i++)
    {
        _links[i]->renewCosts();
    }
    return true;
}

//======================================================================
void RoutingNetwork::setNetworkRank(unsigned int rank)
{
    _networkRank = rank;
}

//======================================================================
unsigned int RoutingNetwork::networkRank() const
{
    return _networkRank;
}

//======================================================================
void RoutingNetwork::setUpperNetwork(RoutingNetwork* upperNetwork)
{
    _upperNetwork = upperNetwork;
}

//======================================================================
RoutingNetwork* RoutingNetwork::upperNetwork() const
{
    return _upperNetwork;
}

//======================================================================
void RoutingNetwork::setLowerNetwork(RoutingNetwork* lowerNetwork)
{
    _lowerNetwork = lowerNetwork;
}

//======================================================================
RoutingNetwork* RoutingNetwork::lowerNetwork() const
{
    return _lowerNetwork;
}

//======================================================================
unsigned int RoutingNetwork::maxNodeRank()
{
    if (_maxNodeRank > 0)
    {
        /* _maxNodeRankは調査済 */
    }
    else
    {
        /* _maxNodeRankは未調査 */
        for (CITRNMAPN citn=_nodes.begin();
             citn!= _nodes.end();
             citn++)
        {
            if ((*citn).second->rank() > _maxNodeRank)
            {
                _maxNodeRank = (*citn).second->rank();
            }
        }
    }
    return _maxNodeRank;
}

//======================================================================
void RoutingNetwork::print() const
{
    cout << "RoutingNetwork(RANK: " << _networkRank << ")" << endl;
    for (CITRNMAPN citn = _nodes.begin(); citn != _nodes.end(); citn++)
    {
        (*citn).second->print();
    }
    for (unsigned int i=0; i<_links.size(); i++)
    {
        _links[i]->print();
    }
    cout << endl;
}

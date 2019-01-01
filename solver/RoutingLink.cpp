/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoutingLink.h"

#include "Conf.h"
#include "RoutingNode.h"
#include "Section.h"
#include "Intersection.h"
#include "AmuConverter.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <cfloat>

using namespace std;

//======================================================================
RoutingLink::RoutingLink(const Intersection* inter,
                         const RoutingNode* begin,
                         const RoutingNode* end)
    : _inter(inter),
      _beginNode(begin),
      _endNode(end)
{
    _rank  = 0;
    _subId = AmuConverter::itos(0, NUM_FIGURE_FOR_ROUTING_LINK);
    _id = AmuConverter::itos(_rank, NUM_FIGURE_FOR_ROUTING_LAYER)
        + _subId;

    _upperLink = NULL;
    _lowerNodes.clear();
    _lowerLinks.clear();
    _includedLowestNodes.clear();

    _length     = 0.0;
    _isStraight = false;
    _isLeft     = false;
    _isRight    = false;
    _width      = 0;

    _whitelistedVehicleTypes.clear();
    _blacklistedVehicleTypes.clear();
    _routingProbability.clear();

    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        _costs[i] = 0.0;
    }
}

//======================================================================
RoutingLink::~RoutingLink()
{
}

//======================================================================
bool RoutingLink::isAggregatedLink() const
{
    if (_lowerLinks.size()==0)
    {
        return false;
    }
    else if (_lowerLinks.size()>=2)
    {
        return true;
    }
    else
    {
        // _lowerLinks.size()==1
        return _lowerLinks[0]->isAggregatedLink();
    }
}

//======================================================================
void RoutingLink::getAllAbbrNodes
(vector<const RoutingNode*>* result_nodes) const
{
    if (_lowerNodes.empty())
    {
        if (!_lowerLinks.empty())
        {
            _lowerLinks[0]->getAllAbbrNodes(result_nodes);
        }
    }
    else
    {
        unsigned int i;
        for (i=0; i<_lowerNodes.size(); i++)
        {
            _lowerLinks[i]->getAllAbbrNodes(result_nodes);
            result_nodes->push_back(_lowerNodes[i]);
        }
        // iは_lowerNodes.size()になっている
        _lowerLinks[i]->getAllAbbrNodes(result_nodes);
    }
}

//======================================================================
void RoutingLink::setProperty()
{
    if (_rank==0)
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ランク0->直接求める
        if (!_inter)
        {
            // レイヤ間リンクには_interが設定されていないので省略
            return;
        }

        const Section* sect = _beginNode->section();
        
        //--------------------------------------------------------------
        // 距離[m]
        /*
         * 直前のRoutingNode（RoadMapにおけるSection）に入ってから
         * RoutingLink（RoadMapにおけるIntersection）を出るまでの距離
         */

        // 単路内の距離
        _length = sect->length(); 

        // 交差点内の距離
        int inDir
            = _inter->direction(sect);
        int outDir
            = _inter->direction(_endNode->section());
        _length += _inter->length(inDir, outDir);

        //--------------------------------------------------------------
        // 直進・左折・右折
        if (_inter->relativeDirection(inDir, outDir)==RD_STRAIGHT)
        {
            _isStraight = true;
        }
        else if (_inter->relativeDirection(inDir, outDir)==RD_LEFT)
        {
            _isLeft = true;
        }
        else if (_inter->relativeDirection(inDir, outDir)==RD_RIGHT)
        {
            _isRight = true;
        }

        //--------------------------------------------------------------
        // 道幅
        bool isUp 
            = sect->isUp(sect->anotherIntersection(_inter), _inter);
        if (isUp)
        {
            _width = sect->upWidth();
        }
        else
        {
            _width = sect->downWidth();
        }

        //--------------------------------------------------------------
        /*
         * ランク0のリンク(=Intersection)に
         *_{white,black}listedVehiclesおよび
         * _routingProbabilityが設定されることはない
         */
    }
    else 
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ランク1以上->lowerLinksを集計する
        if (_lowerLinks.empty())
        {
            // レイヤ間リンクには_lowerLinksがないので省略
            return;
        }

        for (unsigned int i=0; i<_lowerLinks.size(); i++)
        {
            // 距離
            _length += _lowerLinks[i]->length();

            // 直進
            _isStraight = (_isStraight || _lowerLinks[i]->isStraight());

            // 左折
            _isLeft = (_isLeft || _lowerLinks[i]->isLeft());

            // 右折
            _isRight = (_isRight || _lowerLinks[i]->isRight());

            // 道幅
            _width = (_width==0?
                      _lowerLinks[i]->width():
                      min(_width, _lowerLinks[i]->width()));
        }

        //--------------------------------------------------------------
        // _{white,black}listedVehiclesおよび_routingProbability

        //--------------------------------------------------------------
        // 下位のノードが持つもの
        for (unsigned int i=0; i<_lowerNodes.size(); i++)
        {
            const vector<string>* wl
                = _lowerNodes[i]->whitelistedVehicleTypes();
            const vector<string>* bl
                = _lowerNodes[i]->blacklistedVehicleTypes();
            const map<VehicleType, double>* rp
                = _lowerNodes[i]->routingProbability();
            
            for (unsigned int j=0; j<wl->size(); j++)
            {
                vector<string>::iterator its
                    = find(_whitelistedVehicleTypes.begin(),
                           _whitelistedVehicleTypes.end(),
                           wl->at(i));
                if (its==_whitelistedVehicleTypes.end())
                {
                    _whitelistedVehicleTypes.push_back(wl->at(i));
                }
            }
            for (unsigned int j=0; j<bl->size(); j++)
            {
                vector<string>::iterator its
                    = find(_blacklistedVehicleTypes.begin(),
                           _blacklistedVehicleTypes.end(),
                           bl->at(i));
                if (its==_blacklistedVehicleTypes.end())
                {
                    _blacklistedVehicleTypes.push_back(bl->at(i));
                }
            }
            for (map<VehicleType, double>::const_iterator citr
                     = rp->begin();
                 citr != rp->end();
                 citr++)
            {
                const map<VehicleType, double>::iterator it
                    = _routingProbability.find((*citr).first);
                if (it==_routingProbability.end())
                {
                    _routingProbability[(*citr).first] = (*citr).second;
                }
            }
        }

        //--------------------------------------------------------------
        // 下位のリンクが持つもの
        for (unsigned int i=0; i<_lowerLinks.size(); i++)
        {
            const vector<string>* wl
                = _lowerLinks[i]->whitelistedVehicleTypes();
            const vector<string>* bl
                = _lowerLinks[i]->blacklistedVehicleTypes();
            const map<VehicleType, double>* rp
                = _lowerLinks[i]->routingProbability();
            
            //----------------------------------------------------------
            // 通行権
            for (unsigned int j=0; j<wl->size(); j++)
            {
                vector<string>::iterator its
                    = find(_whitelistedVehicleTypes.begin(),
                           _whitelistedVehicleTypes.end(),
                           wl->at(i));
                if (its==_whitelistedVehicleTypes.end())
                {
                    _whitelistedVehicleTypes.push_back(wl->at(i));
                }
            }
            for (unsigned int j=0; j<bl->size(); j++)
            {
                vector<string>::iterator its
                    = find(_blacklistedVehicleTypes.begin(),
                           _blacklistedVehicleTypes.end(),
                           bl->at(i));
                if (its==_blacklistedVehicleTypes.end())
                {
                    _blacklistedVehicleTypes.push_back(bl->at(i));
                }
            }

            //----------------------------------------------------------
            // 経路選択確率
            for (map<VehicleType, double>::const_iterator citr
                     = rp->begin();
                 citr != rp->end();
                 citr++)
            {
                const map<VehicleType, double>::iterator it
                    = _routingProbability.find((*citr).first);
                if (it==_routingProbability.end())
                {
                    _routingProbability[(*citr).first] = (*citr).second;
                }
            }

            //----------------------------------------------------------
            //_includedLowestNodes

            // 最初以外は構成リンクの始点を含む
            if (i!=0)
            {
                _includedLowestNodes
                    .push_back(_lowerLinks[i]->beginNode()->lowestNode());
            }
            // _lowerLinks[i]が含んでいたノードを含む
            const vector<const RoutingNode*>* lowestNodes
                = _lowerLinks[i]->includedLowestNodes();
            for (unsigned int j=0; j<lowestNodes->size(); j++)
            {
                _includedLowestNodes
                    .push_back((*lowestNodes)[j]);
            }
        }
    }
}

//======================================================================
bool RoutingLink::mayPass(VehicleType type) const
{
    stringstream ss;
    ss.str("");
    ss << type;
    
    if (!(_whitelistedVehicleTypes.empty()))
    {
        // ホワイトリスト登録済み - ホワイトリストを優先する
        vector<string>::const_iterator its
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
        vector<string>::const_iterator its
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
double RoutingLink::routingProbability(VehicleType type) const
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
void RoutingLink::setInitialCosts()
{
    // 現在は6種のコストが考慮される
    /**
     * コストは実際の値(右左折直進は0/1)を入力し，
     * 各Routerで変換・重み付けする．
     * 後々配列で扱うため，全てdouble型とする．
     * なお，現状では時間コストのみ○分おきに更新する．
     * 時間コストの初期値は距離/最高速度とする．
     *
     * 0:距離[m]
     * 1:時間[sec]
     * 2:直進
     * 3:左折
     * 4:右折
     * 5:道路幅(車線数)
     */
    double costs[6];
    
    // 距離
    costs[VEHICLE_ROUTING_PARAMETER_OF_LENGTH]
        = static_cast<double>(_length);

    // 時間
    /*
     * 120[km/h] = 2000[m/min] = 100/3[m/sec]
     */ 
    costs[VEHICLE_ROUTING_PARAMETER_OF_TIME]
        = static_cast<double>(_length) * 3.0/100.0;

    // 直進
    costs[VEHICLE_ROUTING_PARAMETER_OF_STRAIGHT] = (_isStraight?1.0:0.0);

    // 左折
    costs[VEHICLE_ROUTING_PARAMETER_OF_LEFT]     = (_isLeft?1.0:0.0);

    // 右折
    costs[VEHICLE_ROUTING_PARAMETER_OF_RIGHT]    = (_isRight?1.0:0.0);

    // 道幅
    costs[VEHICLE_ROUTING_PARAMETER_OF_WIDTH]
        = static_cast<double>(_width);

    //--------------------------------------------------------------
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        _costs[i] = costs[i];
    }
}

//======================================================================
void RoutingLink::renewCosts()
{
    /*
     * upwardLinkおよびdownwardLinkは仮想のリンクであるので
     * 更新の対象としない
     */
    if (_linkType != FLAT)
    {
        return;
    }
    
    double time = 0.0;

    if (_rank==0)
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ランク0->直接求める

        const Section* sect = _beginNode->section();

        if (!_inter)
        {
            cout << "inter is null" << _id << endl;
        }
        
        // 交差点内の転回方向
        int inDir
            = _inter->direction(sect);
        int outDir
            = _inter->direction(_endNode->section());

        // 通過時間を取得
        time += _inter->averagePassTimeForGlobal(inDir, outDir);

        // [msec]->[sec]
        time = time/1000.0;

    }
    else
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ランク1以上->lowerLinksを集計する

        double time = 0.0;

        for (unsigned int i=0; i<_lowerLinks.size(); i++)
        {
            time += _lowerLinks[i]
                ->cost(VEHICLE_ROUTING_PARAMETER_OF_TIME);
        }
    }

    _costs[VEHICLE_ROUTING_PARAMETER_OF_TIME] = time;
}

//======================================================================
void RoutingLink::print() const
{
    cout << "RoutingLink ID: " << _id
         << ", Intersectioin: " << (_inter?_inter->id():"none") << endl;
    
    cout << "  LinkType: "; 
    switch(_linkType)
    {
    case FLAT:
        cout << "flat";
        break;
    case UPWARD:
        cout << "upward";
        break;
    case DOWNWARD:
        cout << "downward";
        break;
    default:
        cout << "undefined";
        break;
    }
    cout << endl;    
    
    if (!_lowerLinks.empty())
    {
        cout << "  LowerLinks: ";
        for (unsigned int i=0; i<_lowerLinks.size(); i++)
        {
            cout << _lowerLinks[i]->id() << ", ";
        }
        cout << endl;
    }
    if (!_lowerNodes.empty())
    {
        cout << "  LowerNodes: ";
        for (unsigned int i=0; i<_lowerNodes.size(); i++)
        {
            cout << _lowerNodes[i]->id() << ", ";
        }
        cout << endl;
    }
    if (!_includedLowestNodes.empty())
    {
        cout << "  IncludedLowestNodes: ";
        for (unsigned int i=0; i<_includedLowestNodes.size(); i++)
        {
            cout << _includedLowestNodes[i]->id() << ", ";
        }
        cout << endl;
    }
    cout << "  Costs: ";
    for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        cout << _costs[i] << ", ";
    }
    cout << endl;
    if (!_whitelistedVehicleTypes.empty())
    {
        cout << "  Whitelist: ";
        for (unsigned int i=0; i<_whitelistedVehicleTypes.size(); i++)
        {
            cout << _whitelistedVehicleTypes[i] << ", ";
        }
        cout << endl;
    }
    if (!_blacklistedVehicleTypes.empty())
    {
        cout << "  Blacklist: ";
        for (unsigned int i=0; i<_blacklistedVehicleTypes.size(); i++)
        {
            cout << _blacklistedVehicleTypes[i] << ", ";
        }
        cout << endl;
    }
    if (!_routingProbability.empty())
    {
        cout << "  RoutingProbability: ";
        for (map<VehicleType, double>::const_iterator citt
                 = _routingProbability.begin();
             citt != _routingProbability.end();
             citt++)
        {
            cout << "(" << (*citt).first
                 << ", " << (*citt).second << "), ";
        }
        cout << endl;
    }
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <typeinfo>
#include <map>
#include "RoadMap.h"
#include "AmuPoint.h"
#include "AmuStringOperator.h"
#include "LaneBundle.h"
#include "Intersection.h"
#include "ODNode.h"
#include "RoadEntity.h"
#include "Section.h"
#include "Signal.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "Lane.h"
#include "Vehicle.h"
#include "RoadOccupant.h"
#include "Route.h"
#include "LocalLaneRoute.h"
#include "Parallel.h"
#include "Partitioner.h"
#include "ParallelConf.h"

using namespace std;

//======================================================================
RoadMap::RoadMap(){}

//======================================================================
RoadMap::~RoadMap()
{
    //Intersectionをdelete&eraseする
    ITRMAPI iti = _intersections.begin();
    while (iti!= _intersections.end())
    {
        delete (*iti).second;
        iti++;
    }
    _intersections.erase(_intersections.begin(),
                         _intersections.end());

    //Sectionをdelete&eraseする
    ITRMAPS its = _sections.begin();
    while (its!=_sections.end())
    {
        delete (*its).second;
        its++;
    }
    _sections.erase(_sections.begin(),
                    _sections.end());

    //Signalをdelete&eraseする
    ITRMAPSI itsi = _signals.begin();
    while (itsi!=_signals.end())
    {
        delete (*itsi).second;
        itsi++;
    }
    _signals.erase(_signals.begin(),
                   _signals.end());
}

//======================================================================
const RMAPI* RoadMap::intersections() const
{
    return &_intersections;
}

//======================================================================
vector<ODNode*> RoadMap::odNodes()
{
    vector<ODNode*> vec;
    ITRMAPI iti = _intersections.begin();
    while (iti!=_intersections.end())
    {
        // (*iti).secondの型がODNodeであるかどうかチェックする
        if (dynamic_cast<ODNode*>((*iti).second))
        {
            vec.push_back(dynamic_cast<ODNode*>((*iti).second));
        }
        iti++;
    }
    return vec;
}

//======================================================================
Intersection* RoadMap::intersection(const string& id) const
{
    CITRMAPI iti = _intersections.find(id);
    if (iti != _intersections.end())
    {
        return (*iti).second;
    }
    else
    {
        return 0;
    }
}

//======================================================================
void RoadMap::addIntersection(Intersection* ptInter)
{
    //同一idがないか検査し、重複がなければ追加する
    string id = ptInter->id();
    ITRMAPI iti = _intersections.find(id);
    if (iti == _intersections.end())
    {
        _intersections[id] = ptInter;
        _bundles.push_back(ptInter);
    }
}

//======================================================================
bool RoadMap::checkIntersectionLane()
{
    bool check, result;

    cout << endl << "*** Check Intersection Lane ***" << endl;

    // 全交差点をチェック後、画面を見て修正、ODNode は除く
    result = true;
    ITRMAPI iti = _intersections.begin();
    while (iti!=_intersections.end())
    {
        if (!dynamic_cast<ODNode*>((*iti).second))
        {
            check = (*iti).second->checkLane();
            if (!check)
            {
                cerr << "intersection " << (*iti).second->id()
                     << ": lane connection error." << endl;
                result = false;
            }
        }
        iti++;
    }
    cout << endl;
    return result;
}

//======================================================================
const RMAPS* RoadMap::sections() const
{
    return &_sections;
}

//======================================================================
Section* RoadMap::section(const string& id) const
{
    CITRMAPS its = _sections.find(id);
    if (its != _sections.end())
    {
        return (*its).second;
    }
    else
    {
        return 0;
    }
}

//======================================================================
void RoadMap::addSection(Section* ptSection)
{
    //同一idがないか検査し、重複があればコメントする
    string id = ptSection->id();
    ITRMAPS its= _sections.find(id);
    if (its != _sections.end())
    {
        cout << "Caution: section:" << id << " is duplicated." << endl;
    }
    _sections[id] = ptSection;
    _bundles.push_back(ptSection);
}

//======================================================================
const vector<LaneBundle*>* RoadMap::laneBundles() const
{
    return &_bundles;
}

//======================================================================
void RoadMap::addUsedLaneBundle(LaneBundle* laneBundle)
{
#ifdef _OPENMP
#pragma omp critical (addUsedLaneBundle)
    {
#endif //_OPENMP

        _usedBundles.push_back(laneBundle);

#ifdef _OPENMP
    }
#endif //_OPENMP
}

//======================================================================
const RMAPSI* RoadMap::signals() const
{
    return &_signals;
}

//======================================================================
Signal* RoadMap::signal(const string& id) const
{
    CITRMAPSI itsi = _signals.find(id);
    if (itsi != _signals.end())
    {
        return (*itsi).second;
    }
    else
    {
        return 0;
    }
}

//======================================================================
void RoadMap::addSignal(Signal* ptSignal)
{
    //同一idがないか検査し、重複がなければ追加する
    string id = ptSignal->id();
    ITRMAPSI itsi = _signals.find(id);
    if (itsi == _signals.end())
    {
        _signals[id] = ptSignal;
    }
}

//======================================================================
void RoadMap::renewAgentLine()
{
    for (unsigned int i=0; i<_usedBundles.size(); i++)
    {
        _usedBundles[i]->renewAgentLine();
        if (typeid(*_usedBundles[i])==typeid(ODNode))
        {
            // ODNodeであればdeleteArrivedAgentを予約
            _usedODNodes.push_back(
                dynamic_cast<ODNode*>(_usedBundles[i]));
        }
    }
    _usedBundles.clear();
}

//======================================================================
void RoadMap::renewAgentLineTmp()
{
    for (unsigned int i=0; i<_usedBundles.size(); i++)
    {
        _usedBundles[i]->renewAgentLineTmp();
    }
}
//======================================================================
void RoadMap::region(double& xmin, double& xmax,
		     double& ymin, double& ymax) const
{
    AmuPoint c;
    CITRMAPI iti = _intersections.begin();
    c = (*iti).second->center();
    xmin = c.x();
    xmax = c.x();
    ymin = c.y();
    ymax = c.y();

    while (iti != _intersections.end())
    {
        c = (*iti).second->center();
        if (c.x()<xmin) xmin = c.x();
        if (c.x()>xmax) xmax = c.x();
        if (c.y()<ymin) ymin = c.y();
        if (c.y()>ymax) ymax = c.y();
        iti++;
    }
}

//======================================================================
void RoadMap::deleteArrivedAgents()
{
    // ODノードのレーン上のエージェントを消去する
    for (unsigned int i=0; i<_usedODNodes.size(); i++)
    {
        _usedODNodes[i]->deleteAgent();
    }
    _usedODNodes.clear();
#ifdef INCLUDE_PEDESTRIANS
    // 渡り終えた歩行者を消す
    ObjManager::deleteFinishedPedestrians();
#endif
}

//======================================================================
void RoadMap::writeMapInfo() const
{
    string fNode, fLink;
    GVManager::getVariable("RESULT_NODE_SHAPE_FILE", &fNode);
    GVManager::getVariable("RESULT_LINK_SHAPE_FILE", &fLink);

    ofstream ofsNode(fNode.c_str(), ios::out);
    if (!ofsNode.fail())
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 交差点について
        ofsNode<< _intersections.size() << endl;
        CITRMAPI iti = _intersections.begin();
        while (iti!=_intersections.end())
        {
            // 交差点ID
            ofsNode << (*iti).second->id() << endl;

            // 中心点の座標
            ofsNode << (*iti).second->center().x() << ","
                    << (*iti).second->center().y() << ","
                    << (*iti).second->center().z() << endl;

            // 頂点の個数
            ofsNode << (*iti).second->numVertices() << endl;
            // 頂点の座標
            for (int i=0; i<(*iti).second->numVertices(); i++) {
                const AmuPoint tmpP = (*iti).second->vertex(i);
                ofsNode << tmpP.x() << ","
                        << tmpP.y() << ","
                        << tmpP.z() << endl;
            }
            iti++;
        }
        ofsNode.close();
    }

    ofstream ofsLink(fLink.c_str(), ios::out);
    if (!ofsLink.fail())
    {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 単路について
        ofsLink << _sections.size() << endl;
        CITRMAPS its = _sections.begin();
        while (its!=_sections.end())
        {
            // 単路ID
            ofsLink << (*its).second->id() << endl;

            // 右レーン数（終点ノードからの流入レーン数）
            // 交差点から見た流出レーン数
            Intersection* inter0 = (*its).second->intersection(false);
            const Border* border0
                = inter0->border(inter0->direction((*its).second));
            ofsLink << border0->numOut();
            ofsLink << ",";
            // 左レーン数（始点ノードからの流入レーン数）
            // 交差点から見た流出レーン数
            Intersection* inter1 = (*its).second->intersection(true);
            const Border* border1
                = inter1->border(inter1->direction((*its).second));
            ofsLink << border1->numOut();
            ofsLink << endl;

            // 始点ID, 終点ID
            ofsLink << inter0->id() << "," << inter1->id() << endl;

            // 始点，終点の信号の有無
            if (inter0->signal()) ofsLink << "1";
            else ofsLink << "0";
            ofsLink << ",";
            if (inter1->signal()) ofsLink << "1";
            else ofsLink << "0";
            ofsLink << endl;

            // 中心点(個数は2に固定)
            /* vertices(int)でも良いと思うが，順番が心配なので．*/
            ofsLink << "2" << endl;
            AmuPoint tmpP;
            tmpP = border0->lineSegment()->createInteriorPoint(1,1);
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;
            tmpP = border1->lineSegment()->createInteriorPoint(1,1);
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;

            // 右境界点(個数は2に固定)
            ofsLink << "2" << endl;
            tmpP = border0->lineSegment()->pointBegin();
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;
            tmpP = border1->lineSegment()->pointEnd();
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;

            // 左境界点(個数は2に固定)
            ofsLink << "2" << endl;
            tmpP = border0->lineSegment()->pointEnd();
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;
            tmpP = border1->lineSegment()->pointBegin();
            ofsLink << tmpP.x() << ","
                    << tmpP.y() << ","
                    << tmpP.z() << endl;

            its++;
        }
        ofsLink.close();
    }
}

//======================================================================
void RoadMap::dispIntersections() const
{
    CITRMAPI iti = _intersections.begin();
    cout << endl;
    cout << "*** RoadMap Information ***" << endl;
    while (iti!=_intersections.end())
    {
        (*iti).second->dispMapInfo();
        iti++;
    }
    /*
    CITRMAPS its = _sections.begin();
    while (its!=_sections.end())
    {
        (*its).second->print();
        its++;
    }
    */
}
//======================================================================
void RoadMap::setProcessId()
{
  map<string, int> processId;

  int count = _parallel->partitioner()->partitioningCount();

  stringstream networkStream;
  networkStream << GVManager::getString("NETWORK_GRAPH_FILE")
                << "." << count << ".part." << _parallel->commSize();

  ifstream network(networkStream.str().c_str());
  string str;

  while(network.good())
 {
   getline(network,str);
   AmuStringOperator::getAdjustString(&str);
   if(!str.empty())
   {
     vector<string> tokens;
     AmuStringOperator::getTokens(&tokens, str, ',');

     processId[tokens[0]] = atoi(tokens[1].c_str());
   }
 }

  RMAPI intersections = _intersections;

  map<string, int>::iterator itp = processId.begin();
  while (itp!=processId.end())
  {
    Intersection* inter = intersections[itp->first];

    // プロセスIDをセットする
    inter->setProcessId(itp->second);

    //同時に交差点要素レーンを登録する
    inter->addfactorLanes();

    // 交差点IDとプロセスIDの確認
    // if(_parallel->myId() == inter->processId())
    // cout<<"processor "<<_parallel->myId()<<" : "<<inter->id()<<endl;

    itp++;
  }
  network.close();

  // _sharedSectionとlaneのフラグの初期化
  map<Section*, Intersection*>::iterator its = _sharedSections.begin();
  while(its != _sharedSections.end())
  {
    RMAPLAN* lanes = const_cast<RMAPLAN*>(its->first->lanes());
    ITRMAPLAN itl = lanes->begin();
    while(itl != lanes->end())
    {
      itl->second->setIsShared(false);
      itl++;
    }
    its++;
  }
  _sharedSections.clear();

  ///隣接交差点のIdから共有セクションを決める
  ITRMAPI iti = _intersections.begin();
  while (iti!=_intersections.end())
  {
    Intersection* myInter = (*iti).second;
    int myProcessId = myInter->processId();

    if(myProcessId == _parallel->myId())
    {
      for(int i = 0; i<myInter->numNext(); i++)
      {
        if(myProcessId != myInter->next(i)->processId())
        {
          // sharedSectionに格納する
          _sharedSections[myInter->nextSection(i)]=myInter->next(i);

          // 内部laneに共有フラグを設定する
          RMAPLAN* lanes = const_cast<RMAPLAN*>(myInter->nextSection(i)->lanes());
          ITRMAPLAN itl = lanes->begin();
          while(itl != lanes->end())
          {
            itl->second->setIsShared(true);
            itl++;
          }
          its++;
        }
      }
    }
    iti++;
  }
}

//======================================================================
void RoadMap::getVehicleData(Vehicle* vehicle, int opponentId)
{
  // 隣接領域に移動する車両の送信データを作成する

  // 車種
  _sendData[opponentId].push_back((double)vehicle->type());

  // 交差点ID
  if(vehicle->intersection())
    _sendData[opponentId].push_back(atof(vehicle->intersection()->id().c_str()));
  else
    _sendData[opponentId].push_back(-1);

  // 単路ID
  if(vehicle->section())
    _sendData[opponentId].push_back(atof(vehicle->section()->id().c_str()));
  else
    _sendData[opponentId].push_back(-1);

  // レーンID
  _sendData[opponentId].push_back(atof(vehicle->lane()->id().c_str()));

  // 次のレーンID
  _sendData[opponentId].push_back(atof(vehicle->nextLane()->id().c_str()));

  // 前のレーンID
  if(vehicle->prevLane())
  _sendData[opponentId].push_back(atof(vehicle->prevLane()->id().c_str()));
  else
    _sendData[opponentId].push_back(-1);

  // レーン始点からの距離
  _sendData[opponentId].push_back(vehicle->length());
  assert(vehicle->length());
  assert(vehicle->length()>0);

  // レーン中心線からのずれ
  _sendData[opponentId].push_back(vehicle->error());

  // 速度
  _sendData[opponentId].push_back(vehicle->velocity());

  // 前に所属していた交差点ID
  _sendData[opponentId].push_back(atof(vehicle->prevIntersection()->id().c_str()));

  // 現在特殊な行動（車線変更）をとっているかどうか
  _sendData[opponentId].push_back((double)vehicle->isNotifying());

  // 交差点に侵入するときに一旦停止したかどうか
  _sendData[opponentId].push_back((double)vehicle->hasPaused());

  // 休止時間
  _sendData[opponentId].push_back((double)vehicle->sleepTime());

  // 経路探索パラメータ
  double* params = vehicle->routingParams();
  for(int i = 0; i < VEHICLE_ROUTING_PARAMETER_SIZE; i++)
  {
      _sendData[opponentId].push_back(params[i]);
  }
  // 選好するネットワークランク
  _sendData[opponentId].push_back((double)vehicle->preferredNetworkRank());

  // 大域経路

  /// 最後に通過した交差点のIndex
  _sendData[opponentId].push_back((double)vehicle->route()->lastPassedIntersectionIndex());

  /// 最後に通過したゲートのindex
  _sendData[opponentId].push_back((double)vehicle->route()->lastPassedGateIndex());

  /// 経路本体(1番目のデータは経路数)
  const vector<const Intersection*> route = *(vehicle->route()->route());
  _sendData[opponentId].push_back((double)route.size());
  for(unsigned int k=0; k<route.size();k++)
  {
    _sendData[opponentId].push_back((double)atoi(route[k]->id().c_str()));
  }

  /// 通過すべきゲートのリスト(1番目のデータはゲート数)
  const vector<const Intersection*> gates = *(vehicle->route()->gates());
  _sendData[opponentId].push_back((double)gates.size());
  for(unsigned int k=0; k<gates.size();k++)
  {
    _sendData[opponentId].push_back((double)atoi(gates[k]->id().c_str()));
  }

  // 局所的経路(車両が交差点内に存在するときのみ必要)

  if(vehicle->intersection())
  {
    /// 局所的経路本体(1番目のデータは経路数)
    const std::vector<Lane*> localRoute = vehicle->localRoute()->localRoute();
    _sendData[opponentId].push_back((double)localRoute.size());
    for(unsigned int i = 0;i< localRoute.size();i++)
    {
      //// レーンID
      _sendData[opponentId].push_back(atof(localRoute[i]->id().c_str()));

      //// 親オブジェクトが交差点かどうか
      _sendData[opponentId].push_back((double)vehicle->intersection()->isMyLane(localRoute[i]));

      //// 親オブジェクトID
      _sendData[opponentId].push_back(atof(localRoute[i]->parent()->id().c_str()));
    }

    /// 交差点内経路(1番目のデータは経路数)
    const std::vector<Lane*> lanesInIntersection = *(vehicle->lanesInIntersection());
    _sendData[opponentId].push_back((double)lanesInIntersection.size());
    for(unsigned int i = 0;i< lanesInIntersection.size();i++)
    {
      //// レーンID
      _sendData[opponentId].push_back(atof(lanesInIntersection[i]->id().c_str()));

      //// 親オブジェクトが交差点かどうか
      _sendData[opponentId].push_back((double)vehicle->intersection()->isMyLane(lanesInIntersection[i]));

      //// 親オブジェクトID
      _sendData[opponentId].push_back(atof(lanesInIntersection[i]->parent()->id().c_str()));
    }
  }

  // 送信する車両の識別子
  _sendData[opponentId].push_back(REAL_VEHICLE);
}

//======================================================================
void RoadMap::getVirtualVehicleData(Vehicle* vehicle, int opponentId)
{
  // 隣接領域で参照する車両の送信データを作成する

  // 単路ID
  _sendData[opponentId].push_back(atof(vehicle->section()->id().c_str()));

  // レーンID
  _sendData[opponentId].push_back(atof(vehicle->lane()->id().c_str()));

  // レーン始点からの距離
  _sendData[opponentId].push_back(vehicle->length());

  // 速度
  _sendData[opponentId].push_back(vehicle->velocity());

  // 送信する車両の識別子
  _sendData[opponentId].push_back(VIRTUAL_VEHICLE);

}

//======================================================================
std::vector<std::vector<double> >* RoadMap::sendData()
{
  return &_sendData;
}

//======================================================================
void RoadMap::setParallel(Parallel* parallel)
{
  _parallel = parallel;
}

//======================================================================
int RoadMap::countAllWeight()
{
  double allWeight = 0;

  CITRMAPI iti = _intersections.begin();

  while (iti!=_intersections.end())
  {
    //自分の領域なら処理
    if((*iti).second->processId()==_parallel->myId())
    {
      allWeight += (*iti).second->weight();
    }
    iti++;
  }

  return (int)allWeight;
}

//======================================================================
void RoadMap::weightToInterFactor()
{
  stringstream s;
  s << GVManager::getString("NETWORK_GRAPH_FILE") << ".tmp";

  ofstream fout(s.str().c_str(),ios::app);

  //自領域の交差点要素の重みを書き出し
  CITRMAPI iti = _intersections.begin();

  while (iti!=_intersections.end())
  {
    if((*iti).second->processId()==_parallel->myId())
    {
      fout<<(*iti).second->id()<<" "<<(int)(*iti).second->weight()<<endl;
    }
    iti++;
  }
}

//======================================================================
void RoadMap::decideSendDataInSharedSection()
{

  _sendData.clear();
  _sendData.resize(_parallel->commSize());

  map<Section*, Intersection*>::iterator itm = _sharedSections.begin();
  while(itm != _sharedSections.end())
  {
    // 送信するレーン
    vector<Lane*> lanesTo = itm->first->lanesTo(itm->second);
    vector<Lane*> lanesFrom = itm->first->lanesFrom(itm->second);

    // 送信先プロセスID
    int processId = itm->second->processId();

    //------------------------------------------------------------------
    // 隣接領域に移動するエージェント（lanesTo）
    //------------------------------------------------------------------

    // 車両を取得する
    for(unsigned int i=0;i<lanesTo.size();i++)
    {
      vector<RoadOccupant*>* occupants = lanesTo[i]->agents();

      vector<RoadOccupant*>::iterator ita = occupants->begin();
      while(ita != occupants->end())
      {
        if(dynamic_cast<Vehicle*>(*ita))
        {
          // cout<<"_myId:"<<_parallel->myId()<<" send lane:"<<lanesTo[i]->id()<<endl;
          Vehicle* vehicle = dynamic_cast<Vehicle*>(*ita);
          getVehicleData(vehicle, processId);
        }
        ita++;
      }

      // データを取得後、元の車両を削除する
      if(!occupants->empty())
      {
        lanesTo[i]->setUsed();
      }
      for(unsigned int k=0; k<occupants->size(); k++)
      {
        if(dynamic_cast<Vehicle*>((*occupants)[k]))
        ObjManager::deleteVehicle(dynamic_cast<Vehicle*>((*occupants)[k]));
      }
      lanesTo[i]->deleteAgent();
    }
    //------------------------------------------------------------------
    // 隣接領域の参照エージェント(lanesFrom)
    //------------------------------------------------------------------

    // 車両を取得する
    for(unsigned int i=0;i<lanesFrom.size();i++)
    {
      RoadOccupant* occupants = lanesFrom[i]->tailAgent();

      if(dynamic_cast<Vehicle*>(occupants))
      {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(occupants);
        getVirtualVehicleData(vehicle, processId);
      }
    }
    //------------------------------------------------------------------
    itm++;
  }

  //データの確認
  // for(unsigned int i=0;i<_sendData.size();i++)
  // {
  //   if(_sendData[i].size()==0)
  //   continue;
  //
  //   for(unsigned int j=0;j<_sendData[i].size();j++)
  //   {
  //     cout<<_sendData[i][j]<<" ";
  //   }
  //   cout<<endl;
  // }
}

//======================================================================
void RoadMap::decideSendDataInRepartitioning()
{
  _sendData.clear();
  _sendData.resize(_parallel->commSize());

  // 再分割後プロセスIDを格納する
  map<string, int> afterId;

  // 再分割の回数
  int count = _parallel->partitioner()->partitioningCount();

  stringstream afterPartStream;
  afterPartStream <<  GVManager::getString("NETWORK_GRAPH_FILE")
             << "." << count << ".part." << _parallel->commSize();

  ifstream afterPart(afterPartStream.str().c_str());
  string str;

  while(afterPart.good())
  {
    //cout<<"myId:"<<_parallel->myId()<<" "<<"a"<<endl;
    getline(afterPart,str);
    AmuStringOperator::getAdjustString(&str);
    if(!str.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str, ',');

      afterId[tokens[0]] = atoi(tokens[1].c_str());
    }
  }

  // afterIdの確認
  // map<string, int>::iterator ita = afterId.begin();
  // while(ita != afterId.end())
  // {
  //
  //   cout<<"myId:"<<_parallel->myId()<<" "<<"intersection:"<<ita->first<<" "<<"processId:"<<ita->second<<endl;
  //   ita++;
  // }

  vector<Lane*> lanes;

  ITRMAPI iti = _intersections.begin();
  while (iti!=_intersections.end())
  {
    if((*iti).second->processId() == _parallel->myId() && (*iti).second->processId() != afterId[(*iti).second->id()] &&
    (*iti).second->countAgentsNum() != 0)
    {
      //送信するレーン
      vector<Lane*> lanes = *((*iti).second->interfactorLanes());

      //送信先プロセスID
      int processId = afterId[(*iti).second->id()];

      //車両を取得する
      for(unsigned int i=0;i<lanes.size();i++)
      {

        vector<RoadOccupant*>* occupants = lanes[i]->agents();

        vector<RoadOccupant*>::iterator ita = occupants->begin();

        while(ita !=occupants->end())
        {
            if(dynamic_cast<Vehicle*>(*ita))
            {
              Vehicle* vehicle = dynamic_cast<Vehicle*>(*ita);
              getVehicleData(vehicle, processId);
            }
          // }
          ita++;
        }

        // データを取得後、元の車両を削除する
        if(!occupants->empty())
        {
          lanes[i]->setUsed();
        }

        for(unsigned int k=0; k<occupants->size(); k++)
        {
          if(dynamic_cast<Vehicle*>((*occupants)[k]))
          ObjManager::deleteVehicle(dynamic_cast<Vehicle*>((*occupants)[k]));
        }
        lanes[i]->deleteAgent();
      }
    }
    iti++;
  }

  //データの確認
  // for(unsigned int i=0;i<_sendData.size();i++)
  // {
  //   if(_sendData[i].size()==0)
  //   continue;
  //
  //   for(unsigned int j=0;j<_sendData[i].size();j++)
  //   {
  //     cout<<_sendData[i][j]<<" ";
  //   }
  //   cout<<endl;
  // }
}

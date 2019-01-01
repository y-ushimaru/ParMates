/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Vehicle.h"
#include "Simulator.h"
#include "RoadMap.h"
#include "LaneBundle.h"
#include "Intersection.h"
#include "ODNode.h"
#include "Section.h"
#include "Lane.h"
#include "RoadEntity.h"
#include "TimeManager.h"
#include "Random.h"
#include "RoadMap.h"
#include "RoutingManager.h"
#include "PFBase.h"
#include "PFAStarHierarchy.h"
#include "PFLogger.h"
#include "Route.h"
#include "LocalLaneRoute.h"
#include "LocalLaneRouter.h"
#include "Signal.h"
#include "SignalColor.h"
#include "VirtualLeader.h"
#include "GVManager.h"
#include "AmuPoint.h"
#include "AmuConverter.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/VehiclePedExt.h"
#endif //INCLUDE_PEDESTRIANS

using namespace std;

//======================================================================
Vehicle::Vehicle():_id()
{
    _bodyLength = 4.400;
    _bodyWidth  = 1.830;
    _bodyHeight = 1.315;

    _bodyArticulation = 1;

    _bodyColorR = 1.0;
    _bodyColorG = 0.0;
    _bodyColorB = 0.0;

    _sim          = NULL;
    _roadMap      = NULL;
    _intersection = NULL;
    _prevIntersection = NULL;
    _section      = NULL;
    _lane         = NULL;
    _nextLane     = NULL;
    _prevLane     = NULL;

    _length        = 0;
    _oldLength     = -10;
    _totalLength   = 0;
    _tripLength    = 0;
    _error         = 0;
    _velocity      = 0;
    _velocityHistory.clear();
    _errorVelocity = 0;
    _accel         = 0;
    _vMax          = 60.0/60.0/60.0;
    _startTime     = 0;
    _genTime       = TimeManager::time();

    _rnd = Random::randomGenerator();

    _blinker.setNone();

    _laneShifter.setVehicle(this);

    _strictCollisionCheck
        = (GVManager::getNumeric("STRICT_COLLISION_CHECK") == 1);
    _entryTime     = 0;
    _isNotifying   = false;
    _hasPaused     = false;
    _sleepTime     = 0;
    _isNotifySet   = false;
    _isUnnotifySet = false;

    _route  = new Route();
    _localRouter.setVehicle(this);
    _localRouter.setRoute(_route);
    _localRouter.setLocalRoute(&_localRoute);
    _localRoute.clear();
    _preferredNetworkRank = INT_MAX;

    // parallel
    _calcTime      = 0;

#ifdef INCLUDE_PEDESTRIANS
    _pedExt = new VehiclePedExt(this);
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
void Vehicle::setId(const string& id)
{
    _id = id;
}

//======================================================================
Vehicle::~Vehicle()
{
    if(_route) delete _route;

    if(_intersection) _intersection->eraseWatchedVehicle(this);
    if(_section) _section->eraseWatchedVehicle(this);

    for (int i=0; i<static_cast<signed int>(_leaders.size()); i++)
    {
        delete _leaders[i];
    }
    _leaders.clear();

    Random::releaseRandomGenerator(_rnd);

#ifdef INCLUDE_PEDESTRIANS
    delete _pedExt;
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
const string&  Vehicle::id() const
{
    return _id;
}

//======================================================================
VehicleType Vehicle::type() const
{
    return _type;
}

//======================================================================
void Vehicle::setType(VehicleType type)
{
    _type = type;
}

//======================================================================
double Vehicle::bodyWidth() const
{
    return _bodyWidth;
}

//======================================================================
double Vehicle::bodyLength() const
{
    return _bodyLength;
}

//======================================================================
double Vehicle::bodyHeight() const
{
    return _bodyHeight;
}

//======================================================================
int Vehicle::bodyArticulation() const
{
    return _bodyArticulation;
}

//======================================================================
void Vehicle::setBodySize(double length, double width, double height)
{
    _bodyLength = length;
    _bodyWidth = width;
    _bodyHeight = height;
}

//======================================================================
void Vehicle::setBodyArticulation(int articulation)
{
    _bodyArticulation = articulation;
}

//======================================================================
void Vehicle::setPerformance(double accel, double brake)
{
    assert(accel>0 && brake<0);
    _maxAcceleration = accel*1.0e-6;
    _maxDeceleration = brake*1.0e-6;
}

//======================================================================
void Vehicle::setBodyColor(double r, double g, double b)
{
    _bodyColorR = r;
    _bodyColorG = g;
    _bodyColorB = b;
}
//======================================================================
void Vehicle::getBodyColor(double* result_r,
                           double* result_g,
                           double* result_b) const
{
    *result_r = _bodyColorR;
    *result_g = _bodyColorG;
    *result_b = _bodyColorB;
}

//======================================================================
double Vehicle::length() const
{
    return _length;
}

//======================================================================
double Vehicle::oldLength() const
{
    return _oldLength;
}

//======================================================================
double Vehicle::totalLength() const
{
    return _totalLength;
}

//======================================================================
double Vehicle::tripLength() const
{
    return _tripLength;
}

//======================================================================
double Vehicle::x() const
{
    AmuVector pv(_lane->beginConnector()->point(),
                 _lane->endConnector()->point());
    pv.normalize();

    double x = _lane->beginConnector()->point().x()+ _length*pv.x();

    pv.revoltXY(M_PI_2);
    x += _error * pv.x();

    return x;
}

//======================================================================
double Vehicle::y() const
{
    AmuVector pv(_lane->beginConnector()->point(),
                 _lane->endConnector()->point());
    pv.normalize();

    double y = _lane->beginConnector()->point().y()+ _length*pv.y();

    pv.revoltXY(M_PI_2);
    y += _error * pv.y();

    return y;
}

//======================================================================
double Vehicle::z() const
{
    AmuVector pv(_lane->beginConnector()->point(),
                 _lane->endConnector()->point());
    pv.normalize();

    double z = _lane->beginConnector()->point().z()+ _length*pv.z();

    pv.revoltXY(M_PI_2);
    z += _error * pv.z();

    return z;
}

//======================================================================
LaneBundle* Vehicle::laneBundle() const
{
    assert((!_section && _intersection)
           || (_section && !_intersection));

    if (_section)
    {
        return _section;
    }
    else if (_intersection)
    {
        return _intersection;
    }
    else
    {
        return NULL;
    }
}

//======================================================================
Section* Vehicle::section() const
{
    return _section;
}

//======================================================================
Intersection* Vehicle::intersection() const
{
    return _intersection;
}

//======================================================================
Intersection* Vehicle::prevIntersection() const
{
    return _prevIntersection;
}

//======================================================================
Lane* Vehicle::lane() const
{
    return _lane;
}

//======================================================================
Lane* Vehicle::nextLane() const
{
    return _nextLane;
}

//======================================================================
Lane* Vehicle::prevLane() const
{
    return _prevLane;
}

//======================================================================
bool Vehicle::isAwayFromOriginNode() const
{
    bool check = true;

    if (_route->lastPassedIntersectionIndex()==0
        && _totalLength
        < GVManager::getNumeric("NO_OUTPUT_LENGTH_FROM_ORIGIN_NODE"))
    {
        check = false;
    }
    return check;
}

//======================================================================
double Vehicle::velocity() const
{
    return _velocity;
}

//======================================================================
const AmuVector Vehicle::velocityVector() const
{
    AmuVector dvec = directionVector();
    dvec.normalize();
    return dvec * _velocity;
}

//======================================================================
double Vehicle::error() const
{
    return _error;
}

//======================================================================
double Vehicle::aveVelocity() const
{
    if (_velocityHistory.empty())
    {
        return 1.0;
    }

    double sum = 0.0;
    for (unsigned int i=0; i<_velocityHistory.size(); i++)
    {
        sum += _velocityHistory[i];
    }
    return sum / _velocityHistory.size();
}

//======================================================================
double Vehicle::accel() const
{
    return _accel;
}

//======================================================================
const AmuVector Vehicle::directionVector() const
{
    assert(_lane!=NULL);
    return _lane->directionVector();
}

//======================================================================
void Vehicle::notify()
{
    if (_section)
    {
        _section->addWatchedVehicle(this);
    }
    else
    {
        _intersection->addWatchedVehicle(this);
    }
    _isNotifying = true;
}

//======================================================================
void Vehicle::unnotify()
{
    if (_section)
    {
        _section->eraseWatchedVehicle(this);
    }
    else
    {
        _intersection->eraseWatchedVehicle(this);
    }
    _isNotifying = false;
}

//======================================================================
Blinker Vehicle::blinker() const
{
    return _blinker;
}

//======================================================================
int Vehicle::directionFrom() const
{
    Intersection* inter;
    if (_intersection)
    {
        inter = _intersection;
    }
    else
    {
        inter = _section->intersection(_section->isUp(_lane));
    }
    if (inter)
    {
        const vector<Lane*>* liInter = _localRoute.lanesInIntersection();
        assert(inter->isMyLane((*liInter)[0]));
        return inter->direction((*liInter)[0]->beginConnector());
    }
    else
    {
        return -1;
    }
}

//======================================================================
int Vehicle::directionTo() const
{
    Intersection* inter;
    if (_intersection)
    {
        inter = _intersection;
    }
    else
    {
        inter = _section->intersection(_section->isUp(_lane));
    }
    if (inter)
    {
        const vector<Lane*>* liInter = _localRoute.lanesInIntersection();
        assert(inter->isMyLane((*liInter)[liInter->size()-1]));
        return inter
            ->direction((*liInter)[liInter->size()-1]->endConnector());
    }
    else
    {
        return -1;
    }
}

//======================================================================
VehicleLaneShifter& Vehicle::laneShifter()
{
    return _laneShifter;
}

//======================================================================
bool Vehicle::isSleep() const
{
    return (_sleepTime>0);
}

//======================================================================
void Vehicle::setStartTime(ulint startTime)
{
    _startTime = startTime;
}

//======================================================================
ulint Vehicle::startTime() const
{
    return _startTime;
}

//======================================================================
ulint Vehicle::startStep() const
{
    return _startTime / TimeManager::unit();
}

//======================================================================
ulint Vehicle::genTime() const
{
    return _genTime;
}

//======================================================================
const vector<VirtualLeader*>* Vehicle::virtualLeaders() const
{
    return &_leaders;
}

//======================================================================
void Vehicle::setRoutingParams(vector<double> params)
{
    for (unsigned int i=0;
         i<VEHICLE_ROUTING_PARAMETER_SIZE;
         i++)
    {
        _routingParams[i] = params[i];
    }
}

//======================================================================
void Vehicle::setPreferredNetworkRank(int networkRank)
{
    _preferredNetworkRank = networkRank;
}

//======================================================================
Route* Vehicle::route()
{
    return _route;
}

//======================================================================
bool Vehicle::reroute(const Intersection* rear,
                      const Intersection* front)
{
    bool result = true;

    // 経路選択器を取得
    PFBase* pathfinder = _sim->routingManager()->assignPathfinder();
    PFLogger logger;

    // 経路探索
    pathfinder->setId(_id);
    pathfinder->setVehicleType(_type);
    pathfinder->setWeights(_routingParams);
    if (dynamic_cast<PFAStarHierarchy*>(pathfinder))
    {
        dynamic_cast<PFAStarHierarchy*>(pathfinder)
            ->setPreferredNetworkRank(_preferredNetworkRank);
    }
    pathfinder->setRandomGenerator(_rnd);
    result = pathfinder->search(_route, rear, front);

    // 経路選択器を返却
    _sim->routingManager()->releasePathfinder(pathfinder);

    return result;
}

//======================================================================
const vector<Lane*>* Vehicle::lanesInIntersection() const
{
    return _localRoute.lanesInIntersection();
}

//======================================================================
LocalLaneRoute* Vehicle::localRoute()
{
    return &_localRoute;
}

//======================================================================
RandomGenerator* Vehicle::randomGenerator()
{
    assert(_rnd);
    return _rnd;
}

//======================================================================
void Vehicle::print() const
{
    cout << "--- Vehicle Information ---" << endl;
    cout << "ID: " << _id << ", Type: " << _type << endl;

    // 位置と速度に関するもの
    if (_section!=NULL)
    {
        cout << "Section ID, Lane ID: (" << _section->id();
    }
    else
    {
        cout << "Intersection ID, Lane ID: (" << _intersection->id();
    }
    cout << ", " << _lane->id() << ")" << endl;
    cout << "Length, Error: (" << _length
         << ", " << _error << ")" << endl;
    cout << "Velocity, ErrorVelocity: ("
         << _velocity <<", "<< _errorVelocity << ")" << endl;

    AmuVector vvec = velocityVector();
    cout << "VelocityVector: ("
         << vvec.x() << ", " << vvec.y() << ", " << vvec.z()
         << ")" << endl;
    cout << "Position(x, y, z): ("
         << x() << ", " << y() << ", " << z() << ")"<< endl;

    // 経路探索パラメータ
    cout << "Routing Parameter: (" << _routingParams[0];
    for (unsigned int i=1; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        cout << ", " << _routingParams[i];
    }
    cout << ")" << endl;

    // 選好するネットワークランク
    cout << "PreferredNetworkRank: " << _preferredNetworkRank << endl;

    // 経路に関するもの
    _route->print(cout);
    _localRoute.print();

    // 交錯レーンに関するもの
    if (_section!=NULL)
    {
        Intersection* nextInter
            = _section->intersection(_section->isUp(_lane));
        if (nextInter)
        {
            vector<Lane*> cli;
            vector<Lane*> cls;
            nextInter->collisionLanes(_localRoute.lanesInIntersection(),
                                      &cli, &cls);
            cout << "Collision Lanes in Intersection:" << endl;
            for (unsigned int i=0; i<cli.size(); i++)
            {
                cout << "\t" << cli[i]->id() << endl;
            }
            cout << "Collision Lanes in Section:" << endl;
            for (unsigned int i=0; i<cls.size(); i++)
            {
                cout << "\t" << cls[i]->id() << " of section "
                     << nextInter->nextSection
                    (nextInter->direction
                     (cls[i]->endConnector()))->id()
                     << endl;
            }
        }
    }

    // 車線変更に関するもの
    if (_laneShifter.isActive())
    {
        cout << "Shift Lane:" << endl;
        cout << "  Target Lane  : " << _laneShifter.laneTo()->id() << endl;
        cout << "  Target Length: " << _laneShifter.lengthTo() << endl;
    }

#ifdef VL_DEBUG
    cout << "Virtual Leaders:" << endl;
    for (unsigned int i=0; i<_leaders.size(); i++)
    {
        _leaders[i]->print();
    }
#endif //VL_DEBUG

    cout << endl;
}

//======================================================================
#ifdef INCLUDE_PEDESTRIANS
VehiclePedExt* Vehicle::pedExt()
{
    return _pedExt;
}
#endif //INCLUDE_PEDESTRIANS

//======================================================================
void Vehicle::setStatus(RoadMap* roadMap, std::vector<double>* status)
{
  vector<double> _status = *status;
  string id;

  int count = 0;

  //ロードマップ
  _roadMap = roadMap;

  //車種
  _type = (int)_status[count];
  count++;

  //交差点
  if(_status[count]!=-1)
  {
    id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
    _intersection = _roadMap->intersection(id);
  }
  count++;

  //単路
  if(_status[count]!=-1)
  {
    stringstream str;
    str << setprecision(NUM_FIGURE_FOR_SECTION) << _status[count];
    id = AmuConverter::formatId(str.str(), NUM_FIGURE_FOR_SECTION);
    _section = _roadMap->section(id);
  }
  count++;

  //レーン
  RMAPLAN *lanes;
  if(_section)
  lanes = const_cast<RMAPLAN*>(_section->lanes());
  else
  lanes = const_cast<RMAPLAN*>(_intersection->lanes());

  id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
  _lane = (*lanes)[id];
  _lane->setUsed();
  count++;


  //次に進むレーン
  id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
  const vector<Lane*> nextLanes = *(_lane->nextLanes());
  for (unsigned int i=0; i<nextLanes.size(); i++)
  {
    if(id == nextLanes[i]->id())
    _nextLane = nextLanes[i];
  }
  count++;

  //前にいたレーン
  if(_status[count]!=-1)
  {
    id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
    const vector<Lane*> prevLanes = *(_lane->previousLanes());
    for (unsigned int i=0; i<prevLanes.size(); i++)
    {
      if(id == prevLanes[i]->id())
      _prevLane = prevLanes[i];
    }
  }
  count++;

  //レーン始点からの長さ
  _length = _status[count];
  count++;

  //レーン中心線からのずれ
  _error = _status[count];
  count++;

  //速度
  _velocity = _status[count];
  count++;

  // 前に所属していた交差点ID
  id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
  _prevIntersection = _roadMap->intersection(id);
  count++;

  // 現在特殊な行動（車線変更）をとっているかどうか
  _isNotifying = static_cast<bool>(_status[count]);
  count++;

  // 交差点に侵入するときに一旦停止したかどうか
  _hasPaused = static_cast<bool>(_status[count]);
  count++;

  // 休止時間
  _sleepTime = static_cast<int>(_status[count]);
  count++;

  // 経路探索パラメータ
  for(int i = 0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
  {
    _routingParams[i] = _status[count];
    count++;
  }

  // 選好するネットワークランク
  _preferredNetworkRank =_status[count];
  count++;

  //大域経路

  ///最後に通過した交差点のIndex
  _route->setLastPassedIntersectionIndex((int)_status[count]);
  count++;

  ///最後に通過したゲートのindex
  _route->setLastPassedGateIndex((int)_status[count]);
  count++;

  ///経路本体
  int routesNum = (int)_status[count];
  count++;

  for(int i=0; i<routesNum; i++)
  {
    id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
    _route->addIntersection(_roadMap->intersection(id));
    count++;
  }

  ///通過すべきゲートのリスト
  int gatesNum = (int)_status[count];
  count++;

  for(int i=0; i<gatesNum; i++)
  {
    id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
    _route->addGate(_roadMap->intersection(id));
    count++;
  }

  // 局所的経路(車両が交差点内に存在するときのみ必要)
  if(_intersection)
  {
    /// 局所的経路本体
    int localRouteNum = (int)_status[count];
    count++;

    for(int i=0; i<localRouteNum; i++)
    {
      id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
      count++;

      // 親オブジェクトが交差点の場合
      if(_status[count])
      {
        count++;
        string intersectionId = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
        lanes = const_cast<RMAPLAN*>(_roadMap->intersection(intersectionId)->lanes());
      }
      // 親オブジェクトが単路の場合
      else
      {
        count++;
        stringstream str;
        str << setprecision(NUM_FIGURE_FOR_SECTION) << _status[count];
        string sectionId = AmuConverter::formatId(str.str(), NUM_FIGURE_FOR_SECTION);
        lanes = const_cast<RMAPLAN*>(_roadMap->section(sectionId)->lanes());
      }
      _localRoute.addLocalRoute((*lanes)[id]);
      count++;
    }

    /// 交差点内経路
    int lanesInIntersectionNum = (int)_status[count];
    count++;

    for(int i=0; i<lanesInIntersectionNum; i++)
    {
      id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
      count++;

      // 親オブジェクトが交差点の場合
      if(_status[count])
      {
        count++;
        string intersectionId = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_INTERSECTION);
        lanes = const_cast<RMAPLAN*>(_roadMap->intersection(intersectionId)->lanes());
      }
      // 親オブジェクトが単路の場合
      else
      {
        count++;
        stringstream str;
        str << setprecision(NUM_FIGURE_FOR_SECTION) << _status[count];
        string sectionId = AmuConverter::formatId(str.str(), NUM_FIGURE_FOR_SECTION);
        lanes = const_cast<RMAPLAN*>(_roadMap->section(sectionId)->lanes());
      }
      _localRoute.addLanesInIntersection((*lanes)[id]);
      count++;
    }
  }

  // 車両発生時に必要なその他の処理

  // _localRouterに_routeをセット
  _localRouter.setRoute(_route);

  // レーンを登録
  if(_section)
  _setLane(_section, _lane, _length);
  else
  _setLane(_intersection, _lane, _length);

  // データが正しく構築されたことの確認
  // cout<<"type:"<<_type<<endl
  //     <<"intersection:"<<_intersection<<endl
  //     <<"section:"<<_section<<endl
  //     <<"lane:"<<_lane->id()<<endl
  //     <<"nextLane:"<<_nextLane<<endl
  //     <<"prevLane:"<<_prevLane<<endl
  //     <<"length:"<<_length<<endl
  //     <<"error:"<<_error<<endl
  //     <<"velocity:"<<_velocity<<endl
  //     <<"accel:"<<_accel<<endl
  //     <<"intersectionIndex:"<<_route->lastPassedIntersectionIndex()<<endl
  //     <<"gateIndex:"<<_route->lastPassedGateIndex()<<endl
  //     <<"route:"<<_route->route()->size()<<endl
  //     <<"gate:"<<_route->gates()->size()<<endl;

}

//======================================================================
  bool Vehicle::isNotifying()
{
    return _isNotifying;
}

//======================================================================
  bool Vehicle::hasPaused()
{
    return _hasPaused;
}

//======================================================================
  int Vehicle::sleepTime()
{
    return _sleepTime;
}

//======================================================================
  double* Vehicle::routingParams()
{
    return _routingParams;
}

//======================================================================
  int Vehicle::preferredNetworkRank()
{
    return _preferredNetworkRank;
}

//======================================================================
  double Vehicle::calcTime()
{
    return _calcTime;
}

//======================================================================
  void Vehicle::clearCalcTime()
{
    _calcTime = 0;
}

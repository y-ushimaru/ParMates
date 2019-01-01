/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <typeinfo>
#include "Lane.h"
#include "AmuVector.h"
#include "RoadOccupant.h"
#include "Conf.h"
#include "Vehicle.h"
#include "LaneBundle.h"
#include "RoadEntity.h"
#include "TimeManager.h"

using namespace std;

//######################################################################
/// エージェントのソートに用いるクラス
class AgentLess
{
public:
    /// エージェントの比較関数
    /**
     * 左辺のlength（レーン始点からの距離）が右辺より小さければ真を返す．
     * 即ち，エージェントはレーンの始点からの距離が近い順に整列される．
     */
    bool operator()(const RoadOccupant* rl, const RoadOccupant* rr) const
    {
        return (rl->length() < rr->length());
    }
};

//######################################################################
Lane::Lane(const string& id,
           const Connector* ptBegin, const Connector* ptEnd,
           LaneBundle* parent)
    :_parent(parent),
     _lineSegment(AmuLineSegment(ptBegin->point(), ptEnd->point()))
{
    assert(ptBegin && ptEnd);

    _id = id;
    _beginConnector = ptBegin;
    _endConnector = ptEnd;
    _lastArrivalTime = 0;

//勾配の設定
    double xx, zz;
    xx = sqrt( (_endConnector->x() - _beginConnector->x())
               * (_endConnector->x() - _beginConnector->x())
               + (_endConnector->y() - _beginConnector->y())
               * (_endConnector->y() - _beginConnector->y()) );
    zz = _endConnector->z() - _beginConnector->z();
    _gradient = zz / xx * 100;

    _isUsed = false;
    _tmpAgents.clear();
    _agents.clear();

        //parallel
        _isShared = false;
}

//======================================================================
void Lane::setId(const string& id)
{
    _id = id;
}

//======================================================================
const string& Lane::id() const
{
    return _id;
}

//======================================================================
LaneBundle* Lane::parent() const
{
    return _parent;
}

//======================================================================
bool Lane::setConnection()
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // nextLanesの決定
    vector<Lane*> nextLanes = _parent->lanesFromConnector(_endConnector);

    // _endConnectorがLaneBundleの境界にある場合は隣のLaneBundleにも尋ねる
    if (nextLanes.empty())
    {
        /*
         *_nextLane定義前はLaneBundle::nextBundleを使えない
         */
        if (typeid(*_parent) == typeid(Section))
        {
            // Section内のレーンの場合
            Section* parent
                = dynamic_cast<Section*>(_parent);
            Intersection* nextIntersection
                = parent->intersection(parent->isUp(this));
            if (nextIntersection)
            {
                nextLanes
                    = nextIntersection->lanesFromConnector(_endConnector);
            }
        }
        else
        {
            // Intersection, ODNode内のレーンの場合
            Intersection* parent
                = dynamic_cast<Intersection*>(_parent);
            if (parent->direction(_endConnector) != -1)
            {
                Section* nextSection
                    = parent->nextSection(parent->direction(_endConnector));
                if (nextSection)
                {
                    nextLanes
                        = nextSection->lanesFromConnector(_endConnector);
                }
            }
        }
    }

    if (!nextLanes.empty())
    {
        _nextLanes.insert(_nextLanes.end(),
                          nextLanes.begin(), nextLanes.end());

        // 最も角度の小さなものを探す
        Lane* straightLane = nextLanes[0];
        double minAngle
            = fabs(_lineSegment.directionVector()
                   .calcAngle(nextLanes[0]
                              ->directionVector()));
        double angle;
        for (unsigned int i=1; i<nextLanes.size(); i++)
        {
            angle
                = fabs(_lineSegment.directionVector()
                       .calcAngle(nextLanes[i]->directionVector()));
            if (angle<minAngle)
            {
                straightLane = nextLanes[i];
                minAngle = angle;
            }
        }
        _nextStraightLane = straightLane;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // previousLanesの決定
    vector<Lane*> previousLanes = _parent->lanesToConnector(_beginConnector);

    // _beginConnectorがLaneBundleの境界にある場合は隣のLaneBundleにも尋ねる
    if (previousLanes.empty())
    {
        /*
         *_nextLane定義前はLaneBundle::nextBundleを使えない
         */
        if (typeid(*_parent) == typeid(Section))
        {
            // Section内のレーンの場合
            Section* parent
                = dynamic_cast<Section*>(_parent);
            Intersection* nextIntersection
                = parent->intersection(!(parent->isUp(this)));
            if (nextIntersection)
            {
                previousLanes
                    = nextIntersection->lanesToConnector(_beginConnector);
            }
        }
        else
        {
            // Intersection, ODNode内のレーンの場合
            Intersection* parent
                = dynamic_cast<Intersection*>(_parent);
            if (parent->direction(_beginConnector) != -1)
            {
                Section* nextSection
                    = parent->nextSection(parent->direction(_beginConnector));
                if (nextSection)
                {
                    previousLanes
                        = nextSection->lanesToConnector(_beginConnector);
                }
            }
        }
    }

    if (!previousLanes.empty())
    {
        _previousLanes.insert(_previousLanes.end(),
                              previousLanes.begin(), previousLanes.end());

        // 最も角度の小さなものを探す
        Lane* straightLane = previousLanes[0];
        double minAngle
            = fabs(_lineSegment.directionVector()
                   .calcAngle(previousLanes[0]
                              ->directionVector()));
        double angle;
        for (unsigned int i=1; i<previousLanes.size(); i++)
        {
            angle
                = fabs(_lineSegment.directionVector()
                       .calcAngle(previousLanes[i]
                                  ->directionVector()));
            if (angle<minAngle)
            {
                straightLane = previousLanes[i];
                minAngle = angle;
            }
        }
        _previousStraightLane = straightLane;
    }

    return true;
}

//======================================================================
bool Lane::setCollision()
{
    // LaneInIntersection以外は何もしない
    return true;
}

//======================================================================
const vector<Lane*>* Lane::nextLanes() const
{
    return &_nextLanes;
}

//======================================================================
Lane* Lane::nextLane(int i) const
{
    return _nextLanes[i];
}

//======================================================================
Lane* Lane::nextStraightLane() const
{
    return _nextStraightLane;
}

//======================================================================
const vector<Lane*>* Lane::previousLanes() const
{
    return &_previousLanes;
}

//======================================================================
Lane* Lane::previousLane(int i) const
{
    return _previousLanes[i];
}

//======================================================================
Lane* Lane::previousStraightLane() const
{
    return _previousStraightLane;
}

//======================================================================
void Lane::addSideLane(Lane* lane, bool isLeft)
{
    /*
     * 自身のレーン上のどの区間が相手のレーンに投影できるかを調べる
     * この場合の「投影」の方向は，自身のレーンの方向ベクトルを
     * PI/2[rad]（左側）あるいは-PI/2[rad]（右側）に回転したベクトル
     */
    assert(lane);

    // 区間の始点
    double intervalBegin
        = _lineSegment.calcFootOfPerpendicularLength
        (lane->beginConnector()->point());
    if (intervalBegin < 0.0)
    {
        intervalBegin = 0.0;
    }

    // 区間の終点
    double intervalEnd
        = _lineSegment.calcFootOfPerpendicularLength
        (lane->endConnector()->point());
    if (intervalEnd > _lineSegment.length())
    {
        intervalEnd = _lineSegment.length();
    }

    if (isLeft)
    {
        // _leftLanesに追加する
        _leftLanes[AmuInterval(intervalBegin, intervalEnd)] = lane;
    }
    else
    {
        // _rightLanesに追加する
        _rightLanes[AmuInterval(intervalBegin, intervalEnd)] = lane;
    }
}

//======================================================================
void Lane::getSideLaneLength(double originLength,
                             bool isLeft,
                             Lane** result_sideLane,
                             double* result_length) const
{
}

//======================================================================
const Connector* Lane::beginConnector() const
{
    return _beginConnector;
}

//======================================================================
const Connector* Lane::endConnector() const
{
    return _endConnector;
}

//======================================================================
double Lane::length() const
{
    double length;
    length = _lineSegment.length();
    return length;
}

//======================================================================
const AmuLineSegment Lane::lineSegment() const
{
    return _lineSegment;
}

//======================================================================
const AmuVector Lane::directionVector() const
{
    return _lineSegment.directionVector();
}

//======================================================================
double Lane::speedLimit() const
{
    return _speedLimit;
}

//======================================================================
void Lane::setSpeedLimit(double limit)
{
    _speedLimit = limit;
}

//======================================================================
ulint Lane::lastArrivalTime() const
{
    return _lastArrivalTime;
}

//======================================================================
void Lane::setLastArrivalTime(ulint arrivalTime)
{
    _lastArrivalTime = arrivalTime;
}

//======================================================================
bool Lane::getPosition(double length, AmuPoint* result_point) const
{
    AmuVector dvec = directionVector();
    dvec.normalize();
    *result_point
        = _beginConnector->point() + dvec * length;
    return true;
}

//======================================================================
bool Lane::createIntersectionPoint(const AmuLineSegment& crSLine,
				   AmuPoint* result_point) const
{
    bool result = false;
    result = _lineSegment.createIntersectionPoint(crSLine,
                                                  result_point);
    return result;
}

//======================================================================
AmuPoint Lane::createInteriorPoint(double d0, double d1) const
{
    return _lineSegment.createInteriorPoint(d0, d1);
}

//======================================================================
const AmuPoint Lane::calcNearestPoint(AmuPoint point) const
{
    return _lineSegment.calcNearestPoint(point);
}

//======================================================================
void Lane::setUsed()
{
#ifdef _OPENMP
#pragma omp critical (Lane_setUsed)
    {
#endif //_OPENMP

        if (!_isUsed)
        {
            _isUsed = true;
            _parent->addUsedLane(this);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP
}

//======================================================================
vector<RoadOccupant*>* Lane::agents()
{
    return &_agents;
}

//======================================================================
RoadOccupant* Lane::headAgent()
{
    // _agentsはlengthの小さな(始点に近い)ものから順に並んでいる
    RoadOccupant* head = NULL;
    // cout << "headAgent in " << _id << " at " << _parent->id() << endl;
    if (!_agents.empty())
    {
        head = _agents.back();
    }
    return head;
}

//======================================================================
Vehicle* Lane::headVehicle()
{
    Vehicle* head = NULL;
    if (!_agents.empty())
    {
        vector<RoadOccupant*>::reverse_iterator ita;
        for (ita=_agents.rbegin(); ita!=_agents.rend(); ita++)
        {
            head = dynamic_cast<Vehicle*>(*ita);
            if (head)
            {
                break;
            }
        }
    }
    return head;
}

//======================================================================
RoadOccupant* Lane::tailAgent() const
{
    // _agentsはlengthの小さな(始点に近い)ものから順に並んでいる
    if (!_agents.empty())
    {
        return _agents.front();
    }

    // parallel
    // このレーンが共有レーンなら隣接領域のエージェントも参照する
    if (_isShared && !_virtualAgents.empty())
    {
        return _virtualAgents.front();
    }

    return NULL;
}

//======================================================================
Vehicle* Lane::tailVehicle()
{
    // _agentsはlengthの小さな(始点に近い)ものから順に並んでいる
    Vehicle* tail = NULL;
    if (!_agents.empty())
    {
        vector<RoadOccupant*>::iterator ita;
        for (ita=_agents.begin(); ita!=_agents.end(); ita++)
        {
            tail = dynamic_cast<Vehicle*>(*ita);
            if (tail)
            {
                break;
            }
        }
    }
    return tail;
}

//======================================================================
RoadOccupant* Lane::frontAgent(RoadOccupant* agent)
{
    RoadOccupant* front = NULL;
    std::vector<RoadOccupant*>::iterator ita = find(_agents.begin(),
                                               _agents.end(),
                                               agent);
    if (ita != _agents.end())
    {
        ita++;
        if (ita != _agents.end())
        {
            front = *ita;
        }
    }
    return front;
}

//======================================================================
Vehicle* Lane::frontVehicle(RoadOccupant* agent)
{
    RoadOccupant* front = frontAgent(agent);
    while (front && !(dynamic_cast<Vehicle*>(front)))
    {
        front = frontAgent(front);
    }
    return dynamic_cast<Vehicle*>(front);
}

//======================================================================
void Lane::getFrontAgentFar(RoadOccupant* agent, double threshold,
                            RoadOccupant** result_agent,
                            double* result_distance)
{
    getFrontAgentFar(agent->length(), threshold,
                     result_agent, result_distance);
}

//======================================================================
RoadOccupant* Lane::frontAgent(double length)
{
    RoadOccupant* front = NULL;
    vector<RoadOccupant*>::const_iterator ita;
    for (ita = _agents.begin(); ita!=_agents.end(); ita++)
    {
        if ((*ita)->length()>= length)
        {
            front = (*ita);
            break;
        }
    }
    return front;
}

//======================================================================
Vehicle* Lane::frontVehicle(double length)
{
    RoadOccupant* front = frontAgent(length);
    while (front && !(dynamic_cast<Vehicle*>(front)))
    {
        front = frontAgent(front);
    }
    return dynamic_cast<Vehicle*>(front);
}

//======================================================================
void Lane::getFrontAgentFar(double startLength, double threshold,
                            RoadOccupant** result_agent,
                            double* result_distance)
{
    RoadOccupant* front = frontAgent(startLength);
    double distance = 0;
    if (front)
    {
        // 距離
        distance
            = front->length() - startLength;
    }
    else
    {
        // 下流まで検索する
        double totalLength = length()-startLength;
        if (totalLength < threshold)
        {
            Lane* lookupLane = this;
            while (totalLength < threshold)
            {
                // 次のレーンを取得
                lookupLane = lookupLane->nextStraightLane();
                if (!lookupLane)
                {
                    break;
                }

                // ODノードに到達したら探索を止める
                if (typeid(*(lookupLane->parent()))==typeid(ODNode))
                {
                    break;
                }

                // 最後尾エージェントを検索
                front = lookupLane->tailAgent();
                if (front)
                {
                    distance = totalLength + front->length();
                    break;
                }
                else
                {
                    totalLength += lookupLane->length();
                }
            }
        }
    }
    *result_agent = front;
    *result_distance = distance;
}

//======================================================================
RoadOccupant* Lane::followingAgent(RoadOccupant* agent)
{
    RoadOccupant* follower = NULL;
    vector<RoadOccupant*>::const_iterator ita = find(_agents.begin(),
                                                     _agents.end(),
                                                     agent);
    if (ita!=_agents.begin()
        && ita!=_agents.end())
    {
        ita--;
        follower = *ita;
    }
    return follower;
}

//======================================================================
Vehicle* Lane::followingVehicle(RoadOccupant* agent)
{
    RoadOccupant* follower = followingAgent(agent);
    while (follower && !(dynamic_cast<Vehicle*>(follower)))
    {
        follower = followingAgent(follower);
    }
    return dynamic_cast<Vehicle*>(follower);
}

//======================================================================
void Lane::getFollowingAgentFar(RoadOccupant* agent, double threshold,
                                RoadOccupant** result_agent,
                                double* result_distance)
{
    getFollowingAgentFar(agent->length(), threshold,
                         result_agent, result_distance);
}

//======================================================================
RoadOccupant* Lane::followingAgent(double length)
{
    RoadOccupant* follower = NULL;
    vector<RoadOccupant*>::const_iterator ita;
    for (ita = _agents.begin(); ita!=_agents.end(); ita++)
    {
        if ((*ita)->length()<length)
        {
            follower = *ita;
        }
        else
        {
            break;
        }
    }
    return follower;
}

//======================================================================
Vehicle* Lane::followingVehicle(double length)
{
    RoadOccupant* follower = followingAgent(length);
    while (follower && !(dynamic_cast<Vehicle*>(follower)))
    {
        follower = followingAgent(follower);
    }
    return dynamic_cast<Vehicle*>(follower);
}

//======================================================================
void Lane::getFollowingAgentFar(double startLength, double threshold,
                                RoadOccupant** result_agent,
                                double* result_distance)
{
    RoadOccupant* follower = followingAgent(startLength);
    double distance;
    if (follower)
    {
        // 距離
        distance = startLength - follower->length();
    }
    else
    {
        // 上流まで検索する
        double totalLength = startLength;
        if (totalLength < threshold)
        {
            Lane* lookupLane = this;
            while (totalLength < threshold)
            {
                // 次のレーンを取得
                lookupLane = lookupLane->previousStraightLane();
                if (!lookupLane)
                {
                    break;
                }

                // ODノードに到達したら探索を止める
                if (typeid(*(lookupLane->parent()))==typeid(ODNode))
                {
                    break;
                }

                // 先頭エージェントを検索
                follower = lookupLane->headAgent();
                if (follower)
                {
                    distance
                        = totalLength
                        + lookupLane->length()
                        - follower->length();
                    break;
                }
                else
                {
                    totalLength += lookupLane->length();
                }
            }
        }
    }
    *result_agent = follower;
    *result_distance = distance;
}

//======================================================================
bool Lane::putAgent(RoadOccupant* agent)
{
    bool flag = false;
#ifdef _OPENMP
#pragma omp critical (Lane_putAgent)
    {
#endif //_OPENMP

        vector<RoadOccupant*>::iterator ita = find(_tmpAgents.begin(),
                                                   _tmpAgents.end(),
                                                   agent);
        if (ita == _tmpAgents.end())
        {
            _tmpAgents.push_back(agent);
            flag = true;
        }
        else
        {
            assert(0);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP

    return flag;
}

//======================================================================
void Lane::renewAgentLine()
{
    assert(_isUsed);

    // 2つのvectorの入れ替え
    _agents.swap(_tmpAgents);
    // ソートする
    sort(_agents.begin(), _agents.end(), AgentLess());
    // tmpを空に
    _tmpAgents.clear();
    // _isUsedをクリア
    _isUsed = false;
}

//======================================================================
void Lane::renewAgentLineTmp()
{
    assert(_isUsed);

    // _tmpAgentsに置き換え
    _agents=_tmpAgents;
    // ソートする
    sort(_agents.begin(), _agents.end(), AgentLess());
}

//======================================================================
double Lane::gradient() const
{
    return _gradient;
}

//======================================================================
double Lane::averageVel() const
{
    double vel = 0.0;
    int numVehicles = 0;

    for (unsigned int i=0; i<_tmpAgents.size(); i++)
    {
        if (dynamic_cast<Vehicle*>(_tmpAgents[i])!=NULL)
        {
            numVehicles++;
            vel += _tmpAgents[i]->velocity();
        }
    }

    // 車両がいない場合には制限速度を平均速度とする
    if (numVehicles==0)
    {
        vel = _speedLimit/60.0/60.0;
    }
    else
    {
        vel /= numVehicles;
    }
    return vel;
}

//======================================================================
void Lane::print() const
{
    cout << "--- Lane Information ---" << endl;
    cout << "ID: " << _id << ", Parent ID: " << _parent->id() << endl;
    cout << "Length: " << _lineSegment.length() << endl;

    if (!_nextLanes.empty())
    {
        cout << "NextLanes:" << endl;
        for (unsigned int i=0; i<_nextLanes.size(); i++)
        {
            cout << "  ID: " << _nextLanes[i]->id() << endl;
        }
        cout << "  NextStraightLane:" << _nextStraightLane->id() << endl;
    }
    else
    {
        cout << "NextLanes: none" << endl;
    }

    if (!_previousLanes.empty())
    {
        cout << "PreviousLanes:" << endl;
        for (unsigned int i=0; i<_previousLanes.size(); i++)
        {
            cout << "  ID: " << _previousLanes[i]->id() << endl;
        }
        cout << "  PreviousStraightLane:" << _previousStraightLane->id() << endl;
    }
    else
    {
        cout << "PreviousLanes: none" << endl;
    }
    cout << endl;
}
//======================================================================
void Lane::setIsShared(bool isShared)
{
  _isShared = isShared;
}
//======================================================================
bool Lane::isShared()
{
  return _isShared;
}
//======================================================================
void Lane::deleteAgent()
{
  _agents.clear();
}

//======================================================================
void Lane::deleteVehicle(Vehicle* vehicle)
{
  RoadOccupant* occupant = vehicle;
  vector<RoadOccupant*>::iterator itr = find(_agents.begin(),
                                        _agents.end(),
                                        occupant);

  if (itr!=_agents.end())
  {
      delete *itr;
      _agents.erase(itr);
  }
}

//======================================================================
void Lane::putVirtualAgent(RoadOccupant* agent)
{
  // 前のステップで残っているエージェントをクリアする
  _virtualAgents.clear();

  // エージェントを追加する
  _virtualAgents.push_back(agent);
}

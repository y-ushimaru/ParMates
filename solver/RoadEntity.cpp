/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoadEntity.h"
#include "LaneBundle.h"
#include "Signal.h"
#include "GVManager.h"
#include <cassert>

using namespace std;

//======================================================================
RoadEntity::RoadEntity(const string& id, SubroadFactory type)
{
    _id = id;
    _type = type;
    _signal = NULL;
    _permission = TRAFFIC_NONE;
    _vertices.clear();
    _adjEntities.clear();

    // 何度も参照すると遅いかもしれないのでクラスに入れておく
    // 動的に作られるクラスなので必ず初期化されている
    _strictJudgeInside =
        (GVManager::getNumeric("ROAD_ENTITY_STRICT_JUDGE_INSIDE") == 1.0);
}

//======================================================================
RoadEntity::~RoadEntity() {}

//======================================================================
const AmuPoint RoadEntity::center() const
{
    return _center;;
}

//======================================================================
void RoadEntity::createCenter()
{
    double x=0.0,y=0.0,z=0.0;

    for (int i=0; i<static_cast<signed int>(_vertices.size()); i++)
    {
        x += _vertices[i].x();
        y += _vertices[i].y();
        z += _vertices[i].z();
    }

    _center.setX(x/static_cast<signed int>(_vertices.size()));
    _center.setY(y/static_cast<signed int>(_vertices.size()));
    _center.setZ(z/static_cast<signed int>(_vertices.size()));
}

//======================================================================
int RoadEntity::numVertices() const
{
    return static_cast<int>(_vertices.size());
}

//======================================================================
const AmuPoint RoadEntity::vertex(int i) const
{
    assert (0<=i && i<static_cast<signed int>(_vertices.size()));
    return _vertices[i];
}

//======================================================================
void RoadEntity::addVertex(AmuPoint vertex)
{
    _adjEntities.push_back(NULL);
    _vertices.push_back(vertex);
    // _adjEntityをNULLで埋めておく
}

//======================================================================
const AmuLineSegment RoadEntity::edge(int i) const
{
    return AmuLineSegment(vertex(i), vertex((i+1)%numVertices()));
}

//======================================================================
const AmuLineSegment RoadEntity::edge(RoadEntity* entity) const
{
    return edge(edgeNum(entity));
}

//======================================================================
int RoadEntity::edgeNum(RoadEntity* entity) const
{
    assert(entity);
    int dir = -1;
    for (int i=0; i<static_cast<signed int>(_adjEntities.size()); i++)
    {
        if (_adjEntities[i]==entity)
        {
            dir = i;
            break;
        }
    }
    if (dir==-1)
    {
        cout << "road entity " << _id
             << " of " << _bundle->id()
             << " doesn't connect entity " << entity->id()
             << " of " << entity->laneBundle()->id() << endl;
        assert(0);
    }
    return dir;
}

//======================================================================
bool RoadEntity::isInside(const AmuPoint point) const
{
    // エッジ上の判定の後、指定点と頂点の角度の総和を取る、0 なら外、±2π なら中
    // これだと遅くなる、角度計算が遅そう
    // 中点からの三角形に分割しても正確に分割できるか分からない
    // 一応 z = 0 平面上で処理している
    // 将来的に凹角を完全に排除した方がいいかもしれない
    if (_strictJudgeInside)
    {
        for (unsigned int i = 0; i < _vertices.size(); i++)
        {
            if (edge(i).z0().distance(point) < 1.0e-6)
                return false;
        }
        double angle = 0;
        for (unsigned int i = 0; i < _vertices.size(); i++)
        {
            AmuVector v1(point.z0(), _vertices[i].z0());
            AmuVector v2(point.z0(), _vertices[(i + 1)
                                               % _vertices.size()].z0());
            angle += v1.calcAngle(v2);
        }
        if (fabs(angle) < M_PI)
        {
            return false;
        }
    }
    else
    {
        bool ccw = edge(0).isLeftSide(point);
        for (unsigned int i = 0; i < _vertices.size(); i++)
        {
            if (edge(i).length() > 1.0e-6
                && edge(i).isLeftSide(point) != ccw)
            {
                return false;
            }
        }
    }
    return true;
}

//======================================================================
const string& RoadEntity::id() const
{
    return _id;
}

//======================================================================
void RoadEntity::setLaneBundle(LaneBundle* bundle)
{
    _bundle = bundle;
}

//======================================================================
LaneBundle* RoadEntity::laneBundle() const
{
    return _bundle;
}

//======================================================================
void RoadEntity::setType(SubroadFactory t)
{
    _type = t;
}

//======================================================================
SubroadFactory RoadEntity::type() const
{
    return _type;
}

//======================================================================
void RoadEntity::setPermission(TrafficCategory t, bool initWalkerType)
{
    _permission = t;
    // TRAFFIC_WALKER と initWalkerType を設定すると歩行者種別を初期化する
    // 青信号ならすべての歩行者が通行可能になり、赤信号なら通行不可になる
    if ((t & TRAFFIC_WALKER) && initWalkerType)
    {
        _trafficWalkerTypes[TRAFFIC_SIGNAL_BLUE].clear();
        _trafficWalkerTypes[TRAFFIC_SIGNAL_RED].clear();
        _addWalkerType(TRAFFIC_WALKER_TYPE_ANY, TRAFFIC_SIGNAL_BLUE);
    }
}


//======================================================================
void RoadEntity::setWalkerTypes(const vector<int> &walkerTypes,
                                int signal)
{
    _trafficWalkerTypes[signal].clear();
    addWalkerTypes(walkerTypes, signal);
}

//======================================================================
void RoadEntity::addWalkerTypes(const vector<int> &walkerTypes,
                                int signal)
{
    _permission = (TrafficCategory)(_permission | TRAFFIC_WALKER);
    for (unsigned int i = 0; i < walkerTypes.size(); i++)
    {
        _addWalkerType(walkerTypes[i], signal);
    }
}

//======================================================================
bool RoadEntity::mayPass(TrafficCategory t) const
{
    return _permission & t;
}

//======================================================================
bool RoadEntity::mayPassWalkerType(int walkerType, int signal) const
{
    if (!(_permission & TRAFFIC_WALKER))
    {
        return false;
    }
    else if (_trafficWalkerTypes[signal].find(TRAFFIC_WALKER_TYPE_ANY) !=
             _trafficWalkerTypes[signal].end())
    {
        return true;
    }
    return (_trafficWalkerTypes[signal].find(walkerType) !=
            _trafficWalkerTypes[signal].end());
}

//======================================================================
RoadEntity* RoadEntity::adjEntity(int edge) const
{
    assert(0<=edge
           && edge<static_cast<signed int>(_adjEntities.size()));
    return _adjEntities[edge];
}

//======================================================================
void RoadEntity::addAdjEntity(RoadEntity* entity, int edge)
{
    // 上書きを許さない
    // 一つの辺を共有するサブセクションは一対のはず
    assert(_adjEntities[edge]==NULL);
    _adjEntities[edge] = entity;
}

//======================================================================
Signal* RoadEntity::signal() const
{
    return _signal;
}

//======================================================================
int RoadEntity::signalDirection() const
{
    return _directionInInter;
}

//======================================================================
void RoadEntity::attachSignal(Signal* signal, int dir)
{
    _signal = signal;
    _directionInInter = dir;
}

//======================================================================
void RoadEntity::print() const
{
    cout << "Road Entity:" << _id << " of " << _bundle->id() << endl;
    for (int i=0; i<static_cast<signed int>(_vertices.size()); i++)
    {
        cout << i << ": "
             << "("  << _vertices[i].x()
             << ", " << _vertices[i].y()
             << ", " << _vertices[i].z() << ")" << endl;
    }
    for (int i=0; i<static_cast<signed int>(_adjEntities.size()); i++)
    {
        if (_adjEntities[i]!=NULL)
        {
            cout << _adjEntities[i]->_id << " of "
                 << _adjEntities[i]->_bundle->id() << endl;
        }
    }
}

//======================================================================
void RoadEntity::_addWalkerType(int walkerType, int signal)
{
    // TRAFFIC_WALKER_TYPE_ANY ならそれだけになる
    if (walkerType == TRAFFIC_WALKER_TYPE_ANY)
    {
        _trafficWalkerTypes[signal].clear();
        _trafficWalkerTypes[signal].insert
            (make_pair(walkerType, walkerType));
    }
    else if (_trafficWalkerTypes[signal].find(TRAFFIC_WALKER_TYPE_ANY)
             != _trafficWalkerTypes[signal].end())
    {
        return;
    }
    // 歩行者種別の重複がなければ追加する
    else if (_trafficWalkerTypes[signal].find(walkerType)
             == _trafficWalkerTypes[signal].end())
    {
        _trafficWalkerTypes[signal].insert
            (make_pair(walkerType, walkerType));
    }
}

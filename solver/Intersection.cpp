/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Intersection.h"
#include "RoadMap.h"
#include "Section.h"
#include "Signal.h"
#include "RelativeDirection.h"
#include "RelativeDirectionTable.h"
#include "Lane.h"
#include "LaneInIntersection.h"
#include "Border.h"
#include "AmuConverter.h"
#include "AmuLineSegment.h"
#include "AmuPoint.h"
#include "Connector.h"
#include "RoadEntity.h"
#include "SubIntersection.h"
#include "RoadOccupant.h"
#include "Vehicle.h"
#include "RouteStorage.h"
#include "ParallelConf.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

//#define INTERSECTION_DEBUG

//######################################################################
/// レーンのソートに用いるクラス
class LaneLess
{
public:
    /// レーンの比較関数
    /**
     * 左辺のidが右辺より小さければ真を返す
     */
    bool operator()(const Lane* rl, const Lane* rr) const
    {
        return (rl->id() < rr->id());
    }
};

//======================================================================
Intersection::Intersection(const std::string& id,
                           const std::string& type,
                           RoadMap* parent)
    : LaneBundle(id, parent)
{
    //typeはnumIn,numOutの列を表す
    //要素数が偶数でなければならない
    if ((type.size()%2)!=0)
    {
        cerr << "Error: intersection " << _id << " type is invalid - "
             << type << "." << endl;
        exit(EXIT_FAILURE);
    }

    //1文字ずつ切り出し、intに変換する
    //とりあえずnumIn,numOutは一桁の整数に限定する
    for (unsigned int i=0; i<type.size(); i+=2)
    {
        if (!isdigit(type[i])
            || !isdigit(type[i+1]))
        {
            cerr << "Error: intersection " << _id << " type is invalid - "
                 << type << "." << endl;
            exit(EXIT_FAILURE);
        }
        _numIn.push_back(type[i]-'0');
        _numOut.push_back(type[i+1]-'0');
    }

    //_incSectionsの初期化
    _incSections.reserve(type.size()/2);
    for (unsigned int i=0; i<type.size()/2; i++)
    {
        _incSections.push_back(NULL);
    }

    _signal = NULL;
    _hasStructInfo = false;

    _routeStorage = new RouteStorage(this);

    _weight = 0;

#ifdef INCLUDE_TRAMS
    _tramExt = new IntersectionTramExt(this);
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
    _pedExt = new IntersectionPedExt(this);
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
Intersection::~Intersection()
{
    // _bordersの消去
    vector<Border*>::iterator itb;
    for (itb=_borders.begin(); itb!=_borders.end(); itb++)
    {
        delete *itb;
    }
    _borders.clear();

    // _entitiesの消去
    ITRMAPENT ite;
    for (ite=_entities.begin(); ite!=_entities.end(); ite++)
    {
        delete (*ite).second;
        (*ite).second = NULL;
    }

    // _lanesの消去
    ITRMAPLAN itl;
    for (itl=_lanes.begin(); itl!=_lanes.end(); itl++)
    {
        delete (*itl).second;
        (*itl).second = NULL;
    }

    // 相対方向テーブルの消去
    delete _rdTable;

    // 経路探索結果の格納器の消去
    if (_routeStorage)
    {
        delete _routeStorage;
    }

#ifdef INCLUDE_TRAMS
    delete _tramExt;
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
    delete _pedExt;
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
const AmuPoint Intersection::center() const
{
    return _center;
}

//======================================================================
void Intersection::addCenter(AmuPoint center)
{
    _center = center;
}

//======================================================================
double Intersection::length(int inDir, int outDir) const
{
    assert(inDir>=0  && inDir<static_cast<int>(_next.size()));
    assert(outDir>=0 && outDir<static_cast<int>(_next.size()));

    const AmuPoint ip
        = _borders[inDir]->lineSegment()->createInteriorPoint(1, 1);
    const AmuPoint op
        = _borders[outDir]->lineSegment()->createInteriorPoint(1, 1);

    return ip.distance(op);
}

//======================================================================
const AmuLineSegment Intersection::edgeToNextInter
(const Intersection* inter) const
{
    return edge(_dir2edge[direction(inter)]);
}

//======================================================================
int Intersection::dir2edge(int i) const
{
    assert(i>=0 && i<static_cast<signed int>(_dir2edge.size()));
    return _dir2edge[i];
}

//======================================================================
int Intersection::edge2dir(int i) const
{
    assert(i>=0 && i<static_cast<signed int>(_vertices.size()));
    int result = -1;
    for (int j=0; j<static_cast<signed int>(_dir2edge.size()); j++)
    {
        if (_dir2edge[j]==i)
        {
            result = j;
            break;
        }
    }
    return result;
}

//======================================================================
bool Intersection::checkLane()
{
    const Connector* point;
    vector<Lane*> lanes;

    for (unsigned int i = 0; i < _borders.size(); i++)
    {
        for (int j = 0; j < _numIn[i] + _numOut[i]; j++)
        {
            point = edgeConnector(i * 100 + j);
            assert(point);
            if (j < _numIn[i])
            {
                lanes = lanesFromConnector(point);
            }
            else
            {
                lanes = lanesToConnector(point);
            }
            if (lanes.size() == 0)
            {
                return false;
            }
        }
    }

    return true;
}

//======================================================================
const Connector* Intersection::edgeConnector(int idInt)
{
    if (crosswalkWidth(idInt/100) != 0)
    {
        // 接続元の境界に横断歩道が設置されている - 内部コネクタ
        return connector
            (AmuConverter::itos(idInt, NUM_FIGURE_FOR_CONNECTOR_LOCAL));
    }
    else
    {
        // 横断歩道が設置されていない
        return _borders[idInt/100]->connector(idInt%100);
    }
}

//======================================================================
LaneBundle* Intersection::nextBundle(Lane* lane) const
{
    if (isNextLaneMine(lane))
    {
        return NULL;
    }
    else
    {
        return nextSection(lane);
    }
}

//======================================================================
LaneBundle* Intersection::previousBundle(Lane* lane) const
{
    if (isPreviousLaneMine(lane))
    {
        return NULL;
    }
    else
    {
        return previousSection(lane);
    }
}

//======================================================================
int Intersection::numNext() const
{
    return static_cast<signed int>(_next.size());
}
//======================================================================
void Intersection::setNext(Intersection* ptInter)
{
    // 重複があればコメントする
    if (!ptInter)
    {
        cout << _id << endl;
        assert(0);
    }
    vector<Intersection*>::const_iterator citi = _next.begin();
    while (citi!=_next.end())
    {
        if ((*citi)->id()==ptInter->id()) break;
        citi++;
    }
    if (citi!=_next.end())
    {
        cout << "Caution: intersection " << _id
             << " has duplicated adjacent interseciton " << (*citi)->id()
             << "." << endl;
    }
    _next.push_back(ptInter);
}

//======================================================================
bool Intersection::isReachable(const Intersection* from,
			       const Intersection* to) const
{
    // ODノードではUターン禁止
    if (_next.size()==1 && from==to)
    {
        return false;
    }

    bool result = false;

    int dirFrom = direction(from);
    int dirTo   = direction(to);

    // 流入レーンを取得する
    vector<Lane*> lanesFrom;
    vector<const Connector*> connectors = _borders[dirFrom]->inPoints();
    for (int i=0; i<static_cast<signed int>(connectors.size()); i++)
    {
        vector<Lane*> lanes = lanesFromConnector(connectors[i]);
        for (int j=0; j<static_cast<signed int>(lanes.size()); j++)
        {
            lanesFrom.push_back(lanes[j]);
        }
    }

    for (int i=0; i<static_cast<signed int>(lanesFrom.size()); i++)
    {
        if (isReachable(lanesFrom[i], dirTo))
        {
            result = true;
            break;
        }
    }
    return result;
}

//======================================================================
bool Intersection::isNetworked(const Intersection* start,
			       const Intersection* inter)
{
    bool result = false;
    vector<const Intersection*> alreadySearched;
    for (unsigned int i=0; i<_next.size(); i++)
    {
        if (_next[i]==inter)
        {
            result = true;
            break;
        }
    }
    if (result==false)
    {
        for (unsigned int i=0; i<_next.size() && result==false; i++)
        {
            if (_next[i]==start)
            {
                continue;
            }
            alreadySearched.push_back(this);
            result = _next[i]->isNetworked(&alreadySearched, inter);
        }
    }
    return result;
}

//======================================================================
bool Intersection::isNetworked(
    std::vector<const Intersection*>* alreadySearched,
    const Intersection* inter)
{
    bool result = false;
    for (unsigned int i=0; i<_next.size(); i++)
    {
        if (_next[i]==inter)
        {
            result = true;
            break;
        }
    }
    if (result==false)
    {
        for (unsigned int i=0; i<_next.size() && result==false; i++)
        {
            bool searched = false;
            // 次の４行は循環を防ぐために必要
            for (unsigned int j=0; j < alreadySearched->size(); j++)
            {
                if (_next[i]==(*alreadySearched)[j]) searched = true;
            }
            if(searched) continue;
            alreadySearched->push_back(this);
            result = _next[i]->isNetworked(alreadySearched, inter);
        }
    }
    return result;
}

//======================================================================
Intersection* Intersection::next(int i) const
{
    return _next[i];
}

//======================================================================
Section* Intersection::nextSection(const Intersection* inter) const
{
    int dir = direction(inter);
    if (dir < 0
        || dir >= static_cast<int>(_next.size()))
    {
        return NULL;
    }
    return nextSection(direction(inter));
}

//======================================================================
Section* Intersection:: nextSection(int i) const
{
    return _incSections[i];
}

//======================================================================
Section* Intersection::nextSection(Lane* lane) const
{
    Section* result = NULL;
    int dir = direction(lane->endConnector());
    if (dir!=-1)
    {
        result = _incSections[dir];
    }
    return result;
}

//======================================================================
Section* Intersection::previousSection(Lane* lane) const
{
    Section* result = NULL;
    int dir = direction(lane->beginConnector());
    if (dir!=-1)
    {
        result = _incSections[dir];
    }
    return result;
}

//======================================================================
vector<Border*> Intersection::borders() const
{
    return _borders;
}

//======================================================================
const Border* Intersection::border(int i) const
{
    assert(i>=0 && i<static_cast<signed int>(_borders.size()));
    return _borders[i];
}

//======================================================================
void Intersection::addBorder(Border* border, int edge)
{
    assert(border);
    if (edge<0 || edge >= numVertices())
    {
        cerr << "intersection " << _id << " - addBorder:"
             << "edge number is invalid." << endl;
    }
    else if (_dir2edge.size() != _borders.size())
    {
        cerr << "intersection " << _id << " - addBorder:"
             << "_dir2edge size error." << endl;
    }
    else
    {
        _dir2edge.push_back(edge);
        _borders.push_back(border);
    }
}

//======================================================================
int Intersection::direction(const Intersection* inter) const
{
    //該当する交差点がなければ-1を返す
    int dir = -1;
    for (unsigned int i=0; i<_next.size(); i++)
    {
        if (_next[i]==inter)
        {
            dir = i;
            break;
        }
    }
    return dir;
}

//======================================================================
int Intersection::direction(const Section* section) const
{
    //該当する単路がなければ-1を返す
    for (unsigned int i=0; i<_incSections.size(); i++)
    {
        if (_incSections[i]==section)
        {
            return i;
        }
    }
    return -1;
}

//======================================================================
int Intersection::direction(const RoadEntity* entity) const
{
    int dir = -1;
    if (isMyRoadEntity(entity))
    {
        int idInt = atoi(entity->id().c_str());
        // ID - 10 が辺番号を表す
        if (idInt >= 10)
        {
            dir = edge2dir(idInt - 10);
        }
    }
    return dir;
}

//======================================================================
int Intersection::direction(const Connector* connector) const
{
    //該当するconnectorがなければ-1を返す
    int dir = -1;
    for (unsigned int i=0; i<_next.size(); i++)
    {
        const vector<const Connector*> pointsIn
            = _borders[i]->inPoints();
        vector<const Connector*>::const_iterator iti
            = find(pointsIn.begin(), pointsIn.end(), connector);
        if (iti!=pointsIn.end())
        {
            dir = i;
            break;
        }
        const vector<const Connector*> pointsOut
            = _borders[i]->outPoints();
        vector<const Connector*>::const_iterator ito
            = find(pointsOut.begin(), pointsOut.end(),connector);
        if (ito!=pointsOut.end())
        {
            dir = i;
            break;
        }
    }

#ifdef INCLUDE_TRAMS
    if (dir == -1 && _tramExt)
    {
        dir = _tramExt->tramDirection(connector);
    }
#endif //INCLUDE_TRAMS

    return dir;

}

//======================================================================
vector<int> Intersection::directionsFrom(const Lane* lane) const
{
    vector<int> result;
    if (isPreviousLaneMine(lane))
    {
        const vector<Lane*>* lanes = lane->previousLanes();
        for (unsigned int i=0; i<lanes->size(); i++)
        {
            vector<int> prevResult = directionsFrom((*lanes)[i]);
            result.insert(result.end(),
                          prevResult.begin(),
                          prevResult.end());
        }
    }
    else
    {
        result.push_back(direction(lane->beginConnector()));
    }
    return result;
}

//======================================================================
vector<int> Intersection::directionsTo(const Lane* lane) const
{
    vector<int> result;
    if (isNextLaneMine(lane))
    {
        const vector<Lane*>* lanes = lane->nextLanes();
        for (unsigned int i=0; i<lanes->size(); i++)
        {
            vector<int> nextResult = directionsTo((*lanes)[i]);
            result.insert(result.end(),
                          nextResult.begin(),
                          nextResult.end());
        }
    }
    else
    {
        result.insert(result.end(), direction(lane->endConnector()));
    }
    return result;
}

//======================================================================
int Intersection::oppositeDirection(int direction) const
{
    int dir = -1;
    assert(0<=direction
           && direction<static_cast<signed int>(_next.size()));
    for (int i=0; i<static_cast<signed int>(_next.size()); i++)
    {
        if ((*_rdTable)(direction, i)==RD_STRAIGHT)
        {
            dir = i;
            break;
        }
    }
    return dir;
}

//======================================================================
bool Intersection::isMainLane(const Lane* lane) const
{
    assert(isMyLane(lane));
    // レーンの始点の周回番号は識別番号の10 000 000の位
    int beginRound = (atoi(lane->id().c_str()))/10000000;
    // レーンの終点の周回番号は識別番号の1 000の位
    int endRound   = (atoi(lane->id().c_str())%10000)/1000;

    // 交差点内中心レーンは始点，終点とも周回番号0
    if (beginRound==0 && endRound==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isReachable(const Lane* lane, int dir) const
{
    bool flag = false;
    if (isMyLane(lane))
    {
        if (isNextLaneMine(lane))
        {
            // 次のレーンも交差点内 - 次のレーンを対象にして再帰呼び出し
            const vector<Lane*>* lanes = lane->nextLanes();
            vector<Lane*>::const_iterator itl = lanes->begin();
            while (itl!=lanes->end())
            {
                flag = isReachable((*itl), dir);
                if (flag)
                {
                    break;
                }
                itl++;
            }
        }
        else
        {
            // このレーンの終点で判定する
            if (direction(lane->endConnector())==dir)
            {
                flag = true;
            }
        }
    }

#ifdef INTERSECTION_DEBUG
    if (flag)
    {
        cout << "intersection:" <<_id
             << " lane:" << lane->id()
             << " is reachable to direction:" << dir << endl;
    }
    else
    {
        cout << "intersection:" <<_id
             << " lane:" << lane->id()
             << " is NOT reachable to direction:" << dir << endl;
    }
#endif /*INTERSECTION_DEBUG*/

    return flag;
}

//======================================================================
vector<Lane*> Intersection::mainLanes() const
{
    vector<Lane*> result_lanes;
    CITRMAPLAN itl;
    for (itl=_lanes.begin(); itl!=_lanes.end(); itl++)
    {
        if (isMainLane((*itl).second))
            result_lanes.push_back((*itl).second);
    }
    return result_lanes;
}

//======================================================================
vector<Lane*> Intersection::collisionLanes(Lane* lane) const
{
    assert(isMyLane(lane));

    // mainLaneに限定するのは計算量節約のため
    // 限定せず_lanesにした方が良いかも
    // 歩行者を考慮しない場合は関係ないが。
    vector<Lane*> main_lanes = mainLanes();
    vector<Lane*> result_lanes;

    AmuPoint thisBegin = lane->beginConnector()->point();
    AmuPoint thisEnd = lane->endConnector()->point();

    for (int i=0; i<static_cast<signed int>(main_lanes.size()); i++)
    {
        if (main_lanes[i]==lane)
        {
            continue;
        }
        else
        {
            // 線分の交差判定[(thisBegin,thisEnd)と(thatBegin,thatEnd)]
            AmuPoint thatBegin = main_lanes[i]->beginConnector()->point();
            AmuPoint thatEnd = main_lanes[i]->endConnector()->point();
            double v0
                = (thisBegin.x()-thisEnd.x())*(thatBegin.y()-thisBegin.y())
                + (thisBegin.y()-thisEnd.y())*(thisBegin.x()-thatBegin.x());
            double v1
                = (thisBegin.x()-thisEnd.x())*(thatEnd.y()-thisBegin.y())
                + (thisBegin.y()-thisEnd.y())*(thisBegin.x()-thatEnd.x());
            if (abs(v0)<1.0e-5) v0=0.0;
            if (abs(v1)<1.0e-5) v1=0.0;

            // 交差する場合はv0とv1が異符号
            if (v0*v1<=0) {
                result_lanes.push_back(main_lanes[i]);
                // 前のレーンも含める
                if (isPreviousLaneMine(main_lanes[i]))
                {
                    const vector<Lane*>* prevLanes
                        = main_lanes[i]->previousLanes();
                    result_lanes.insert(result_lanes.end(),
                                        prevLanes->begin(),
                                        prevLanes->end());
                }
            }
        }
    }
    return result_lanes;
}

//======================================================================
void Intersection::collisionLanes(const vector<Lane*>* lanes,
				  vector<Lane*>* result_inter,
				  vector<Lane*>* result_section)
{
    result_inter->clear();
    result_section->clear();

    // 各レーンの交錯レーンの情報を統合する
    for (unsigned int i=0; i<lanes->size(); i++)
    {
        LaneInIntersection* lookupLane
            = dynamic_cast<LaneInIntersection*>((*lanes)[i]);
        if (!lookupLane)
        {
            continue;
        }
        assert(isMyLane(lookupLane));

        vector<Lane*>* veci = lookupLane->collisionLanesInIntersection();
        vector<Lane*>* vecs = lookupLane->collisionLanesInSection();
        result_inter->insert(result_inter->end(),
                             veci->begin(),
                             veci->end());
        result_section->insert(result_section->end(),
                               vecs->begin(),
                               vecs->end());
    }

    // それぞれのvectorから重複を削除する
    sort(result_inter->begin(), result_inter->end(), LaneLess());
    vector<Lane*>::iterator il = unique(result_inter->begin(),
                                        result_inter->end());
    result_inter->erase(il, result_inter->end());

    sort(result_section->begin(), result_section->end(), LaneLess());
    il = unique(result_section->begin(), result_section->end());
    result_section->erase(il, result_section->end());
}

//======================================================================
vector<Lane*> Intersection::collisionLanesFront(Lane* lane,
						double length) const
{
    vector<Lane*> result_lanes;
    vector<Lane*> lanes = collisionLanes(lane);
    for (unsigned int i=0; i<lanes.size(); i++)
    {
        AmuPoint tmpPoint;
        lane->createIntersectionPoint(lanes[i]->lineSegment(),
                                      &tmpPoint);
        double distance
            = lane->lineSegment().pointBegin().distance(tmpPoint);
        if (distance>length)
        {
            result_lanes.push_back(lanes[i]);
        }
    }
    return result_lanes;
}

//======================================================================
RoadEntity* Intersection::pairedEntity(RoadEntity* entity, int edge) const
{
    assert(0<=edge && edge<entity->numVertices());
    AmuLineSegment commonEdge = entity->edge(edge);

    // 同一の交差点内から検索
    CITRMAPENT ite;
    for (ite=_entities.begin(); ite!=_entities.end(); ite++)
    {
        if ((*ite).second==entity)
        {
            continue;
        }
        for (int i=0; i<(*ite).second->numVertices(); i++)
        {
            AmuLineSegment line = (*ite).second->edge(i);
            // 対象の辺と共通した始点終点を持つサブセクションを探して返す
            if (( commonEdge.pointBegin()==line.pointBegin()
                  && commonEdge.pointEnd()==line.pointEnd() )
                || ( commonEdge.pointBegin()==line.pointEnd()
                  && commonEdge.pointEnd()==line.pointBegin()) )
            {
                return (*ite).second;
            }
        }
    }
    // 接続する単路内から検索
    for (unsigned int j=0; j<_incSections.size(); j++)
    {
        if (_incSections[j]==NULL) continue;
        for (ite=_incSections[j]->entities()->begin();
             ite!=_incSections[j]->entities()->end();
             ite++)
        {
            for (int i=0; i< (*ite).second->numVertices(); i++)
            {
                AmuLineSegment line = (*ite).second->edge(i);
                // 対象の辺と共通した始点終点を持つ
                // サブセクションを探して返す
                if (( commonEdge.pointBegin()==line.pointBegin()
                      && commonEdge.pointEnd()==line.pointEnd() )
                    || ( commonEdge.pointBegin()==line.pointEnd()
                         && commonEdge.pointEnd()==line.pointBegin()) )
                {
                    return (*ite).second;
                }
            }
        }
    }
    return NULL;
}

//======================================================================
bool Intersection::isRight(Intersection* in0,
			   Intersection* in1) const
{
    assert(in0 && in1);
    int beginDir = direction(in0);
    int endDir = direction(in1);

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_RIGHT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isRight(const Lane* lane) const
{
    assert(isMyLane(lane));
    //レーンの始点の境界番号は識別番号の1 000 000の位
    int beginDir = (atoi(lane->id().c_str())%10000000)/1000000;
    //レーンの終点の境界番号は識別番号の100の位
    int endDir   = (atoi(lane->id().c_str())%1000)/100;

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_RIGHT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isLeft(Intersection* in0,
                          Intersection* in1) const
{
    assert(in0 && in1);
    int beginDir = direction(in0);
    int endDir = direction(in1);

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_LEFT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isLeft(const Lane* lane) const
{
    assert(isMyLane(lane));
    //レーンの始点の境界番号は識別番号の1 000 000の位
    int beginDir = (atoi(lane->id().c_str())%10000000)/1000000;
    //レーンの終点の境界番号は識別番号の100の位
    int endDir   = (atoi(lane->id().c_str())%1000)/100;

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_LEFT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isStraight(Intersection* in0,
			      Intersection* in1) const
{
    assert(in0 && in1);
    int beginDir = direction(in0);
    int endDir = direction(in1);

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_STRAIGHT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
bool Intersection::isStraight(const Lane* lane) const
{
    assert(isMyLane(lane));
    //レーンの始点の境界番号は識別番号の1 000 000の位
    int beginDir = (atoi(lane->id().c_str())%10000000)/1000000;
    //レーンの終点の境界番号は識別番号の100の位
    int endDir   = (atoi(lane->id().c_str())%1000)/100;

    //相対方向テーブルの要素を参照する
    if ((*_rdTable)(beginDir, endDir)==RD_STRAIGHT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//======================================================================
RelativeDirection Intersection::relativeDirection(Intersection* in0,
						  Intersection* in1) const
{
    assert(in0 && in1);
    int beginDir = direction(in0);
    int endDir = direction(in1);

    if (beginDir<0 || endDir<0)
    {
        return RD_NONE;
    }
    else
    {
        return (*_rdTable)(beginDir, endDir);
    }
}

//======================================================================
RelativeDirection Intersection::relativeDirection(int dir0,
						  int dir1) const
{
    assert(0<=dir0 && dir0<static_cast<signed int>(_borders.size()));
    assert(0<=dir1 && dir1<static_cast<signed int>(_borders.size()));
    return (*_rdTable)(dir0, dir1);
}

//======================================================================
RelativeDirection Intersection::relativeDirection(const Lane* lane) const
{
    assert(isMyLane(lane));
    //レーンの始点の境界番号は識別番号の1 000 000の位
    int beginDir = (atoi(lane->id().c_str())%10000000)/1000000;
    //レーンの終点の境界番号は識別番号の100の位
    int endDir   = (atoi(lane->id().c_str())%1000)/100;

    if (beginDir<0 || endDir<0)
    {
        return RD_NONE;
    }
    else
    {
        return (*_rdTable)(beginDir, endDir);
    }
}

//======================================================================
int Intersection::numIn(int dir) const
{
    assert(0<=dir && dir<static_cast<signed int>(_next.size()));
    return _numIn[dir];
}

//======================================================================
int Intersection::numOut(int dir) const
{
    assert(0<=dir && dir<static_cast<signed int>(_next.size()));
    return _numOut[dir];
}

//======================================================================
void Intersection::attachSignal(Signal* signal)
{
    _signal = signal;
}

//======================================================================
Signal* Intersection::signal() const
{
    return _signal;
}

//======================================================================
Signal::SignalPermission Intersection::permission(int from,
                                                  RelativeDirection rd,
                                                  Vehicle* vehicle) const
{
    SignalMainState mainColor;
    SignalSubState subColor;

    mainColor = _signal->mainColor(from);
    subColor = _signal->subColor(from);

    if (mainColor==SignalColor::yellow())
    {
        mainColor = _signal->prevMainColor(from);
        subColor = _signal->prevSubColor(from);
    }

    Signal::SignalPermission result = Signal::PROHIBITION;
    if (mainColor==SignalColor::blue()
        || mainColor==SignalColor::yellow())
    {
        // mainColor が青（黄は上の処理によりないはず）
        result = Signal::PERMISSION;
    }
    else if (mainColor==SignalColor::yellowblink())
    {
        // mainColor が黄点滅->徐行
        result = Signal::CRAWLING;
    }
    else if (mainColor==SignalColor::redblink())
    {
        // mainColor が赤点滅->一旦停止
        result = Signal::PAUSING;
    }
    else
    {
        // mainColor が赤
        if (rd==RD_STRAIGHT)
        {
            if (subColor==SignalColor::all()
                || subColor==SignalColor::straight()
                || subColor==SignalColor::straightLeft()
                || subColor==SignalColor::straightRight())
            {
                result = Signal::PERMISSION;
            }
            else
            {
                result = Signal::PROHIBITION;
            }
        }
        else if (rd==RD_LEFT)
        {
            if (subColor==SignalColor::all()
                || subColor==SignalColor::left()
                || subColor==SignalColor::straightLeft()
                || subColor==SignalColor::leftRight())
            {
                result = Signal::PERMISSION;
            }
            else
            {
                result = Signal::PROHIBITION;
            }
        }
        else if (rd==RD_RIGHT)
        {
            if (subColor==SignalColor::all()
                || subColor==SignalColor::right()
                || subColor==SignalColor::straightRight()
                || subColor==SignalColor::leftRight())
            {
                result = Signal::PERMISSION;
            }
            else
            {
                result = Signal::PROHIBITION;
            }
        }
        else
        {
            result = Signal::PROHIBITION;
        }
    }
    return result;
}

//======================================================================
RouteStorage* Intersection::routeStorage()
{
    return _routeStorage;
}

//======================================================================
void Intersection::deleteRouteStorage()
{
    delete _routeStorage;
}

//======================================================================
void Intersection::addPassTime(int from, int to, ulint passTime)
{
    int index = from*_next.size()+to;
#ifdef _OPENMP
#pragma omp critical (addPassTime)
    {
#endif //_OPENMP
        _passTime[index].push_back(passTime);
        while (_passTime[index].size()>VEHICLE_PASS_TIME_INTERSECTION)
        {
            _passTime[index].pop_front();
        }
        _passTimeTotal[index] += passTime;
        _numPassedVehicleTotal[index]++;
#ifdef _OPENMP
    }
#endif //_OPENMP
}

//======================================================================
double Intersection::averagePassTime(int from, int to) const
{
    unsigned int index = from*_next.size()+to;

    if (index >= _passTime.size())
    {
        cerr << "WARNING: invalid argument of averagePassTime" << endl;
        cerr << "Intersection: " << _id
             << " - from: " << from << " to: " << to << endl;
        return INFINITY;
    }


    if (_passTime[index].size()==0)
    {
        return 0.0;
    }

    ulint sum = 0;
#ifdef _OPENMP
#pragma omp critical (averagePassTime)
    {
#endif //_OPENMP
        list<ulint>::const_iterator it = _passTime[index].begin();
        while (it!=_passTime[index].end())
        {
            sum += (*it);
            it++;
        }
#ifdef _OPENMP
    }
#endif //_OPENMP
    return static_cast<double>(sum)/_passTime[index].size();
}

//======================================================================
double Intersection::averagePassTimeForGlobal(int from, int to) const
{
    int index = from*_next.size()+to;

    if (_previousNumPassedVehicleTotal[index]==0)
    {
        double length = _center.distance(_next[from]->center());
        return length / (120.0 / 60.0 / 60.0);
    }
    else
    {
        return _previousPassTimeTotal[index]
            / _previousNumPassedVehicleTotal[index];
    }
}

//======================================================================
double Intersection::averagePassTimeForGlobal(int from) const
{
    int sumPassedVehicleTotal = 0;
    double sumPassTimeTotal   = 0;

    for (unsigned int i=0; i<_next.size(); i++)
    {
        sumPassedVehicleTotal
            += _previousNumPassedVehicleTotal[from*_next.size()+i];
        sumPassTimeTotal
            += _previousPassTimeTotal[from*_next.size()+i];
    }

    if (sumPassedVehicleTotal == 0)
    {
        double length = _center.distance(_next[from]->center());
        return length / (120.0 / 60.0 / 60.0);
    }
    else
    {
        return sumPassTimeTotal / sumPassedVehicleTotal;
    }
}

//======================================================================
void Intersection::renewPassTimeForGlobal()
{
    for (unsigned int i=0; i<_next.size()*_next.size(); i++)
    {
        if (_numPassedVehicleTotal[i] > 0)
        {
            _previousPassTimeTotal[i]
                = _passTimeTotal[i];
            _previousNumPassedVehicleTotal[i]
                = _numPassedVehicleTotal[i];
        }
        else
        {
            _previousPassTimeTotal[i]
                = _previousPassTimeTotal[i] * 0.9;
        }
    }
    clearPassTimeForGlobal();
}

//======================================================================
void Intersection::clearPassTimeForGlobal()
{
    for (unsigned int i=0; i<_next.size()*_next.size(); i++)
    {
        _passTimeTotal[i] = 0.0;
        _numPassedVehicleTotal[i] = 0;
    }
}

//======================================================================
void Intersection::dispMapInfo() const
{
    // (ID):(x,y,z)/(numIn numOut)-(IDs of next Intersection)
    cout << _id << ":";
    cout << _center.x() << ","
         << _center.y() << ","
         << _center.z() <<"/";

    vector<int>::const_iterator citni = _numIn.begin();
    vector<int>::const_iterator citno = _numOut.begin();
    while (citni!=_numIn.end() && citno!=_numOut.end())
    {
        cout << (*citni) << (*citno);
        citni++;
        citno++;
    }
    cout << "-";

    vector<Intersection*>::const_iterator citi = _next.begin();
    while (citi!=_next.end())
    {
        cout << (*citi)->id() << ",";
        citi++;
    }
    cout << endl;
}

//======================================================================
void Intersection::printDetail(bool odNode) const
{
    // ODNode かどうかは、ここでは取れない
    cout << "--- Intersection Information ---" << endl;
    cout << "ID: " << _id << ", ODNode: " << odNode
         << ", Position: " << _center.x() << ","
         << _center.y() << ","
         << _center.z() << endl;
    cout << "Next Intersection ID: ";
    for (unsigned int i = 0; i < _next.size(); i++)
    {
        if (i != 0)
        {
            cout << ", ";
        }
        cout << _next[i]->id();
    }
    cout << endl;

    if (!odNode)
    {
        cout << "Relative Direction:" << endl;
        for (unsigned int i=0; i<_borders.size(); i++)
        {
            cout << "  ";
            for (unsigned int j=0; j<_borders.size(); j++)
            {
                if (j != 0)
                {
                    cout << ", ";
                }
                if      ((*_rdTable)(i,j) == RD_BACK)
                {
                    cout << "t";
                }
                else if ((*_rdTable)(i,j) == RD_RIGHT)
                {
                    cout << "r";
                }
                else if ((*_rdTable)(i,j) == RD_STRAIGHT)
                {
                    cout << "s";
                }
                else if ((*_rdTable)(i,j) == RD_LEFT)
                {
                    cout << "l";
                }
                else
                {
                    cout << "?";
                }
            }
            cout << endl;
        }

        cout << "Lane Connection:" << endl;
        for (unsigned int i=0; i<_laneBegEnd.size(); i++)
        {
            if (i % 2 == 0)
            {
                cout << "  ";
            }
            cout << _laneBegEnd[i];
            if (i % 2 == 0)
            {
                cout << ", ";
            }
            else
            {
                cout << endl;
            }
        }
    }
    cout << "Vertex:" << endl;
    for (int i=0; i<numVertices(); i++)
    {
        cout << "  " << _streetVertices[i].x() - _center.x() << ", "
             << _streetVertices[i].y() - _center.y() << ", "
             << _streetVertices[i].z() - _center.z() << endl;
    }

    cout << "Cost:" << endl;
    for (unsigned int from=0; from<_next.size(); from++)
    {
        for (unsigned int to=0; to<_next.size(); to++)
        {
            cout << "  from:" << _next[from]->id()
                 << ", to:" << _next[to]->id()
                 << ", - " << averagePassTimeForGlobal(from, to) << endl;
        }

    }

    cout << endl;
}

//======================================================================
#ifdef INCLUDE_TRAMS
IntersectionTramExt* Intersection::tramExt()
{
    return _tramExt;
}

#endif //INCLUDE_TRAMS

//======================================================================
#ifdef INCLUDE_PEDESTRIANS
IntersectionPedExt* Intersection::pedExt()
{
    return _pedExt;
}

#endif //INCLUDE_PEDESTRIANSS

//======================================================================
void Intersection::setProcessId(int processId)
{
    _processId = processId;
}

//======================================================================
int Intersection::processId()
{
    return _processId;
}

//======================================================================
void Intersection::addfactorLanes()
{
  _interfactorLanes.clear();

  //交差点要素レーンを登録する
  ITRMAPLAN itl = _lanes.begin();
  while(itl!= _lanes.end())
    {
      _interfactorLanes.push_back((*itl).second);
      itl++;
    }
  //自分(交差点)に向かう車線上のレーンを登録
  for(unsigned int i=0; i < _next.size(); i++)
    {
      //境界番号i方向の単路を取得
      Section* sec = _incSections[i];
      //単路sec方向の境界オブジェクトを取得
      Border* bd = _borders[direction(sec)];
      //流入コネクタを取得
      vector<const Connector*> incns = bd->inPoints();
      for(unsigned int j=0; j < incns.size(); j++)
        {
          const Connector* cn = incns[j];
          //隣接交差点のレーンになるまで遡ってレーンを登録する
          vector<Lane*> ln = sec->lanesToConnector(cn);
          for(unsigned int k=0; k < ln.size(); k++)
            {
              //再帰関数 車線上のレーンを登録
              addfactorsectionLanes(ln[k],sec);
            }
        }
    }
}
//======================================================================
void Intersection::addfactorsectionLanes(Lane* lane, Section* section)
{
  //まず登録
  _interfactorLanes.push_back(lane);
  //前のレーンを取得
  const vector<Lane*>* prelanes = lane->previousLanes();
  for(unsigned int i=0; i < prelanes->size(); i++)
    {
      Lane* prelane = lane->previousLane(i);
      //このレーンが交差点内or既に登録されてるレーンなら終了
      if(prelane->parent() == section && !addedfactorLanes(prelane))
        {
          addfactorsectionLanes(prelane,section);
        }
    }
}
//======================================================================
bool Intersection::addedfactorLanes(Lane* lane)
{
  //交差点要素レーン群に既に登録されていないか
  bool flag = false;
  vector<Lane*>::iterator itl = _interfactorLanes.begin();
  while(itl != _interfactorLanes.end())
    {
      if((*itl) == lane) break;
      itl++;
    }
  if(itl != _interfactorLanes.end())
    {
      flag = true;
    }
  return flag;
}

//======================================================================
vector<Lane*>* Intersection::interfactorLanes()
{
  return &_interfactorLanes;
}

//======================================================================
double Intersection::countAgentsNum()
{
  double agentsNum = 0;
  //交差点要素のエージェント数をカウントする
  for(unsigned int i=0; i < _interfactorLanes.size(); i++)
  {
     agentsNum += (double)_interfactorLanes[i]->agents()->size();
  }

  return agentsNum;
}

//======================================================================
double Intersection::countCalcTime()
{
  double calcTime = 0;
  //交差点要素のエージェント計算時間を合計する
  for(unsigned int i=0; i < _interfactorLanes.size(); i++)
  {
    vector<RoadOccupant*>* occupants = _interfactorLanes[i]->agents();
    for(unsigned int j = 0; j < occupants->size(); j++)
    {
      if(dynamic_cast<Vehicle*>((*occupants)[j]))
      {
          calcTime += dynamic_cast<Vehicle*>((*occupants)[j])->calcTime();
      }
    }
  }
  return calcTime;  
}

//======================================================================
void Intersection::addWeight(double weight)
{
  // 交差点要素の重みに加算する
  _weight += weight;
}

//======================================================================
double Intersection::weight()
{
  // ODノードの場合には重みをつけて返す
  if(dynamic_cast<ODNode*>(this))
  {
    return _weight + ODNODE_WEIGHT;
  }

  return _weight;
}

//======================================================================
void Intersection::clearWeight()
{
  _weight = 0;
}


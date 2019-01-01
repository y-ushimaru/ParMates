/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LaneBundle.h"
#include "Section.h"
#include "Intersection.h"
#include "SubSection.h"
#include "Conf.h"
#include "Border.h"
#include "Lane.h"
#include "LaneInSection.h"
#include "AmuConverter.h"
#include "AmuPoint.h"
#include "AmuLineSegment.h"
#include "RoadEntity.h"
#include "Border.h"
#include "ObjManager.h"
#include "GVManager.h"
#include <cassert>
#include <cstdlib>
#ifdef INCLUDE_TRAMS
#include "tram/SectionTramExt.h"
#endif //INCLUDE_TRAMS

using namespace std;

//======================================================================
bool Section::setStructInfo(double laneWidth, double roadsideWidth,
                            double sidewalkWidthRight,
                            double sidewalkWidthLeft,
                            const vector<int> &streetTrafficWalkerTypes,
                            bool setDefault, int autoSidewalkLane)
{
    // 設定済みなら無視する
    if (_hasStructInfo)
    {
        return true;
    }

    if (laneWidth          <= 0 || roadsideWidth     <  0 ||
        sidewalkWidthRight <  0 || sidewalkWidthLeft <  0 )
    {
        return false;
    }
    if ((sidewalkWidthRight == 0 && sidewalkWidthLeft != 0) ||
        (sidewalkWidthRight != 0 && sidewalkWidthLeft == 0))
    {
        return false;
    }

    _laneWidth           = laneWidth;
    _roadsideWidth       = roadsideWidth;
    _sidewalkWidthRight  = sidewalkWidthRight;
    _sidewalkWidthLeft   = sidewalkWidthLeft;

    for (unsigned int i = 0; i < streetTrafficWalkerTypes.size(); i++)
    {
        if (streetTrafficWalkerTypes[i] != TRAFFIC_WALKER_TYPE_ANY
            && (streetTrafficWalkerTypes[i] < 0
                || streetTrafficWalkerTypes[i] > 99))
        {
            return false;
        }
        _streetTrafficWalkerTypes.push_back(streetTrafficWalkerTypes[i]);
    }

    // 既定値、自動設定なしなら歩道なし
    // 自動設定ありで車線が多ければ両側の歩道を歩く、それ以外は車道を歩く
    if (setDefault)
    {
        if (autoSidewalkLane < 0)
        {
            _sidewalkWidthRight = _sidewalkWidthLeft = 0;
        }
        else
        {
            if (_numIn[0] + _numOut[0] < autoSidewalkLane &&
                _numIn[1] + _numOut[1] < autoSidewalkLane)
            {
                _sidewalkWidthRight = _sidewalkWidthLeft = 0;
                _streetTrafficWalkerTypes.clear();
                _streetTrafficWalkerTypes.push_back
                    (TRAFFIC_WALKER_TYPE_ANY);
            }
        }
    }

    _hasStructInfo = true;
    return true;
}

//======================================================================
bool Section::create()
{
    bool result = false;

    // 設定ファイルが存在しない場合
    result = createFromTemplate();

    return result;
}

//======================================================================
void Section::connect()
{
    int dir;

    dir =_adjInter[0]->direction(_adjInter[1]);
    _adjInter[0]->setIncidentSection(dir, this);
    _numIn[0] = _adjInter[0]->numOut(dir);
    _numOut[0] = _adjInter[0]->numIn(dir);
  
    dir = _adjInter[1]->direction(_adjInter[0]);
    _adjInter[1]->setIncidentSection(dir, this);
    _numIn[1] = _adjInter[1]->numOut(dir);
    _numOut[1] = _adjInter[1]->numIn(dir);
}

//======================================================================
bool Section::createFromTemplate()
{
    // 頂点配列の生成
    bool isVertexCreated = createDefaultVertices();
    if (!isVertexCreated)
    {
        cerr << "section:" << _id
             << ": vertices have not been successfully created." << endl;
        return false;
    }

    // サブセクションの生成
    bool isSubsectionCreated = createDefaultSubsections();
    if (!isSubsectionCreated)
    {
        cerr << "section:" << _id
             << ": subsections have not been successfully created." << endl;
    }

    // レーンの生成
    bool isLaneCreated = createDefaultLanes();
    if (!isLaneCreated)
    {
        cerr << "section:" << _id
             << ": lanes have not been successfully created." << endl;
    }

    return (isVertexCreated && isSubsectionCreated && isLaneCreated);
}

//======================================================================
bool Section::createDefaultVertices()
{
    double x = 0.0, y = 0.0, z = 0.0;
    for (int i=0; i<2; i++)
    {
        AmuLineSegment edge
            = _adjInter[i]->edgeToNextInter(_adjInter[(i+1)%2]);
        x += edge.pointBegin().x();
        y += edge.pointBegin().y();
        z += edge.pointBegin().z();
        addVertex(edge.pointBegin());
        x += edge.pointEnd().x();
        y += edge.pointEnd().y();
        z += edge.pointEnd().z();
        addVertex(edge.pointEnd());
    }
    x /= static_cast<signed int>(_vertices.size());
    y /= static_cast<signed int>(_vertices.size());
    z /= static_cast<signed int>(_vertices.size());
    _center.setXYZ(x, y, z);
    return true;
}

//======================================================================
bool Section::createDefaultSubsections()
{
    // 車道サブセクションを"0*"、
    // 歩道サブセクションのうち_adjInter[0]->_adjInter[1]方向を見て
    // 右側を"1*"、左側を"2*"で表す

    // デフォルトでは直線単路に限る
    // 片側1車線の単路には歩道を作成しない
    // 辺番号0と辺番号2が交差点に接するようにする
  
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車道
    SubSection* ptSubS = new SubSection("00", SUBROAD_TYPE_STREET);
    ptSubS->setLaneBundle(this);

    // 通行権の設定
    if (_streetTrafficWalkerTypes.size() == 0)
    {
        ptSubS->setPermission(TRAFFIC_VEHICLE_ANY);
    }
    else
    {
        ptSubS->setPermission(TRAFFIC_ANY);
        ptSubS->setWalkerTypes(_streetTrafficWalkerTypes);
    }

    int dir[2] = {-1,-1};
    for (int i=0; i<2; i++)
    {
        dir[i] = _adjInter[i]->direction(_adjInter[(i+1)%2]);
    }
    ptSubS->addVertex
        (_adjInter[0]->border(dir[0])->lineSegment()->pointBegin());
    ptSubS->addVertex
        (_adjInter[0]->border(dir[0])->lineSegment()->pointEnd());
    ptSubS->addVertex
        (_adjInter[1]->border(dir[1])->lineSegment()->pointBegin());
    ptSubS->addVertex
        (_adjInter[1]->border(dir[1])->lineSegment()->pointEnd());
    ptSubS->createCenter();
    _entities["00"] = ptSubS;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 歩道
    AmuLineSegment edge[2];
    edge[0] = _adjInter[0]->edgeToNextInter(_adjInter[1]);
    edge[1] = _adjInter[1]->edgeToNextInter(_adjInter[0]);

    if (sidewalkWidth(_adjInter[0], false)!=0)
    {
        // 歩道1（右側）
        SubSection* ptSubS1 = new SubSection("10", SUBROAD_TYPE_SIDEWALK);
        ptSubS1->setLaneBundle(this);
        ptSubS1->setPermission(TRAFFIC_WALKER);
        ptSubS1->addVertex
            (edge[0].pointBegin());
        ptSubS1->addVertex
            (_adjInter[0]->border(dir[0])->lineSegment()->pointBegin());
        ptSubS1->addVertex
            (_adjInter[1]->border(dir[1])->lineSegment()->pointEnd());
        ptSubS1->addVertex
            (edge[1].pointEnd());
        ptSubS1->createCenter();
        _entities["10"] = ptSubS1;
    }

    if (sidewalkWidth(_adjInter[0], true)!=0)
    {
        // 歩道2 (左側)
        SubSection* ptSubS2 = new SubSection("20", SUBROAD_TYPE_SIDEWALK);
        ptSubS2->setLaneBundle(this);
        ptSubS2->setPermission(TRAFFIC_WALKER);
        ptSubS2->addVertex
            (_adjInter[0]->border(dir[0])->lineSegment()->pointEnd());
        ptSubS2->addVertex
            (edge[0].pointEnd());
        ptSubS2->addVertex
            (edge[1].pointBegin());
        ptSubS2->addVertex
            (_adjInter[1]->border(dir[1])->lineSegment()->pointBegin());
        ptSubS2->createCenter();
        _entities["20"] = ptSubS2;
    }

    return true;
}

//======================================================================
bool Section::createDefaultLanes()
{

    for (int i=0; i<2; i++)
    {
        const Border* borderBegin = _adjInter[i]
            ->border(_adjInter[i]->direction(_adjInter[(i+1)%2]));
        const Border* borderEnd = _adjInter[(i+1)%2]
            ->border(_adjInter[(i+1)%2]->direction(_adjInter[i]));

        if (_numIn[i]==_numOut[(i+1)%2])
        {
            // 流入点と流出点の数が等しい場合は
            // 境界上のコネクタを単純に結ぶ
            for (int j=0; j<_numIn[i]; j++)
            {
                int idIntBegin = i*100+_numIn[i]+_numOut[i]-1-j;
                int idIntEnd = ((i+1)%2)*100+j;
                int idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = borderBegin->connector(_numIn[i]+_numOut[i]-1-j);
                const Connector* pointEnd
                    = borderEnd->connector(j);
                assert(pointBegin && pointEnd);

                string id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                _lanes[id] = new LaneInSection(id, pointBegin, pointEnd, this);
                _lanes[id]->setSpeedLimit
                    (GVManager::getNumeric("SPEED_LIMIT_SECTION"));
            }
        }
        else if (_numIn[i]+1==_numOut[(i+1)%2])
        {
            // 流出点が流入点よりひとつ多い場合は
            // 内部コネクタを作って右折レーンを設置する
            for (int j=0; j<_numIn[i]-1; j++)
            {
                int idIntBegin = i*100+_numIn[i]+_numOut[i]-1-j;
                int idIntEnd = ((i+1)%2)*100+j;
                int idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = borderBegin->connector(_numIn[i]+_numOut[i]-1-j);
                const Connector* pointEnd
                    = borderEnd->connector(j);
                assert(pointBegin && pointEnd);

                string id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                _lanes[id] = new LaneInSection(id, pointBegin, pointEnd, this);
                _lanes[id]->setSpeedLimit
                    (GVManager::getNumeric("SPEED_LIMIT_SECTION"));
            }
            //最内のレーンの作成
            {
                const Connector* pointBegin
                    = borderBegin->connector(_numOut[i]);
                const Connector* pointEnd1
                    = borderEnd->connector(_numOut[(i+1)%2]-2);
                const Connector* pointEnd2
                    = borderEnd->connector(_numOut[(i+1)%2]-1);
                AmuLineSegment line(pointBegin->point(),
                                    pointEnd1->point());
                double length = line.length();
	
                double rightTurnLaneLength
                    = GVManager::getNumeric("RIGHT_TURN_LANE_LENGTH");

                if (length<=rightTurnLaneLength)
                {
                    // 単路が十分に長くない場合は流入点から分岐
                    int idIntBegin = i*100+_numOut[i];
                    int idIntEnd = ((i+1)%2)*100+_numOut[(i+1)%2]-2;
                    int idInt = idIntBegin*10000+idIntEnd;
                    string id
                        = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                    _lanes[id] = new LaneInSection(id, pointBegin, pointEnd1, this);
                    _lanes[id]->setSpeedLimit
                        (GVManager::getNumeric("SPEED_LIMIT_SECTION"));

                    idIntEnd++;
                    idInt = idIntBegin*10000+idIntEnd;
                    id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                    _lanes[id] = new LaneInSection(id, pointBegin, pointEnd2, this);
                    _lanes[id]->setSpeedLimit
                        (GVManager::getNumeric("SPEED_LIMIT_SECTION"));
                }
                else
                {
                    // 十分に長ければ内部ポイントを作成して分岐
                    const AmuPoint tmpPoint
                        = line.createInteriorPoint
                        (length-rightTurnLaneLength,
                         rightTurnLaneLength);
                    string pointId
                        = AmuConverter::itos
                        (9000+i, NUM_FIGURE_FOR_CONNECTOR_LOCAL);
                    _innerConnectors[pointId]
                        = ObjManager::createConnector(tmpPoint.x(),
                                                      tmpPoint.y(),
                                                      tmpPoint.z());
                    int idIntBegin = i*100+_numOut[i];
                    int idIntEnd = 9000+i;
                    int idInt = idIntBegin*10000+idIntEnd;
                    string id = AmuConverter::itos(idInt,
                                                   NUM_FIGURE_FOR_LANE);
                    _lanes[id] = new LaneInSection(id,
                                                   pointBegin,
                                                   connector(pointId),
                                                   this);
                    _lanes[id]->setSpeedLimit
                        (GVManager::getNumeric("SPEED_LIMIT_SECTION"));

                    idIntBegin = 9000+i;
                    idIntEnd = ((i+1)%2)*100+_numOut[(i+1)%2]-2;
                    idInt = idIntBegin*10000+idIntEnd;
                    id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                    assert(connector(pointId));
                    _lanes[id] = new LaneInSection(id,
                                                   connector(pointId),
                                                   pointEnd1,
                                                   this);
                    _lanes[id]->setSpeedLimit
                        (GVManager::getNumeric("SPEED_LIMIT_SECTION"));

                    idIntEnd++;
                    idInt = idIntBegin*10000+idIntEnd;
                    id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
                    _lanes[id] = new LaneInSection(id,
                                                   connector(pointId),
                                                   pointEnd2,
                                                   this);
                    _lanes[id]->setSpeedLimit
                        (GVManager::getNumeric("SPEED_LIMIT_SECTION"));
                }
            }
        }
    }

#ifdef INCLUDE_TRAMS
    _tramExt->createTramLanesInSection();
#endif //INCLUDE_TRAMS

    return true;
}

//======================================================================
void Section::addLane(Lane* lane)
{
    _lanes[lane->id()] = lane;
}

//======================================================================
bool Section::createSubnetwork()
{
    ITRMAPENT ite;
    for (ite=_entities.begin(); ite!=_entities.end(); ite++)
    {
        for (int i=0;
             i<static_cast<signed int>((*ite).second->numVertices());
             i++) {
            RoadEntity* adjEntity = pairedEntity((*ite).second, i);
            if (adjEntity!=NULL) {
                (*ite).second->addAdjEntity(adjEntity, i);
            }
        }
    }
    return true;
}

//======================================================================
double Section::laneWidth() const
{
    // 交差点形状詳細指定をすると上下で変わる可能性がある
    // 車線変更に影響があるが、現状は一定の値にしている
    assert(_hasStructInfo);
    return _laneWidth;
}

//======================================================================
double Section::roadsideWidth() const
{
    assert(_hasStructInfo);
    return _roadsideWidth;
}

//======================================================================
double Section::sidewalkWidth(Intersection* inter, bool leftSide) const
{
    assert(_hasStructInfo || inter==_adjInter[0] || inter==_adjInter[1]);
    if ((inter == _adjInter[0]&&leftSide==true)
        || (inter == _adjInter[1]&&leftSide==false))
    {
        //_adjInter[0]から見て左側＝_adjInter[1]から見て右側
        return _sidewalkWidthLeft;
    }
    else
    {
        //_adjInter[0]から見て右側＝_adjInter[1]から見て左側    
        return _sidewalkWidthRight;
    }
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LaneInSection.h"
#include "Section.h"
#include "Conf.h"
#include <iostream>
#include <cassert>

using namespace std;

//======================================================================
LaneInSection::LaneInSection(const std::string& id,
              const Connector* ptBegin, const Connector* ptEnd,
              LaneBundle* parent)
    : Lane(id, ptBegin, ptEnd, parent)
{
}

//======================================================================
bool LaneInSection::setConnection()
{
    Lane::setConnection();

    Section* parent = dynamic_cast<Section*>(_parent);
    assert(parent);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // leftLanesの決定
    Lane* leftLane = _sideLane(true);

    if (leftLane)
    {
        addSideLane(leftLane, true);
        /*
         * 始点がずれている場合には相手から自分が見えないことがあるので
         * 相手が見つかった時点で相手にも自分を登録してもらう
         * mapへの登録で重複は避けられる
         */
        leftLane->addSideLane(this, false);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // rightLanesの決定
    Lane* rightLane = _sideLane(false);

    if (rightLane)
    {
        addSideLane(rightLane, false);
        /*
         * 始点がずれている場合には相手から自分が見えないことがあるので
         * 相手が見つかった時点で相手にも自分を登録してもらう
         * mapへの登録で重複は避けられる
         */
        rightLane->addSideLane(this, true);
    }

    return true;
}

//======================================================================
void LaneInSection::getSideLaneLength(double originLength,
                                      bool isLeft,
                                      Lane** result_sideLane,
                                      double* result_length) const
{
    if (originLength > _lineSegment.length())
    {
        return;
    }

    // 探索線分の作成
    AmuVector searchVector = directionVector();
    searchVector.normalize();

    // 探索線分の始点
    AmuPoint beginPoint
        = _beginConnector->point()
        + searchVector * originLength;
    if (isLeft)
    {
        searchVector.revoltXY(M_PI_2);
    }
    else
    {
        searchVector.revoltXY(-M_PI_2);
    }
    AmuLineSegment searchLine
        (beginPoint,
         beginPoint + searchVector * SEARCH_SIDE_LANE_LINE_LENGTH);
    
    // 横レーンの取得
    AmuPoint crsPoint;
    if (isLeft)
    {
        for (map<AmuInterval, Lane*>::const_iterator cil
                 = _leftLanes.begin();
             cil != _leftLanes.end();
             cil++)
        {
            if ((*cil).first.isIncluded(originLength))
            {
                if((*cil).second
                    -> createIntersectionPoint(searchLine,
                                               &crsPoint))
                {
                    *result_sideLane
                        = (*cil).second;
                    *result_length
                        = (*cil).second->beginConnector()->point()
                        .distance(crsPoint);
                    return;
                }
            }
        }
    }
    else
    {
        for (map<AmuInterval, Lane*>::const_iterator cil
                 = _rightLanes.begin();
             cil != _rightLanes.end();
             cil++)
        {
            if ((*cil).first.isIncluded(originLength))
            {
                if((*cil).second
                    -> createIntersectionPoint(searchLine,
                                               &crsPoint))
                {
                    *result_sideLane
                        = (*cil).second;
                    *result_length
                        = (*cil).second->beginConnector()->point()
                        .distance(crsPoint);
                    return;
                }
            }
        }
    }
}

//======================================================================
Lane* LaneInSection::_sideLane(bool isLeft) const
{
    Section* parent = dynamic_cast<Section*>(_parent);
    assert(parent);

    // 探索線分の作成
    AmuVector searchVector = directionVector();
    searchVector.normalize();
    // 線分の始点を_beginConnectorから少しだけずらす
    /*
     * 始点を共有する車線を除外するため
     */
    AmuPoint beginPoint
        = _beginConnector->point()
        + searchVector * 1.0e-6;
    if (isLeft)
    {
        searchVector.revoltXY(M_PI_2);
    }
    else
    {
        searchVector.revoltXY(-M_PI_2);
    }
    AmuLineSegment searchLine
        (beginPoint,
         beginPoint + searchVector * SEARCH_SIDE_LANE_LINE_LENGTH);

    // 同じレーン束オブジェクト内のレーン（自分を含む）
    const RMAPLAN* lanes = _parent->lanes();

    // 候補レーンの集合（自分を除き，自分と同じ方向）
    vector<Lane*> candidateLanes;
    bool isThisUp = parent->isUp(this);

    for (CITRMAPLAN cil = lanes->begin();
         cil != lanes->end();
         cil++)
    {
        if ((*cil).second==this)
        {
            continue;
        }
        if (parent->isUp((*cil).second) == isThisUp)
        {
            candidateLanes.push_back((*cil).second);
        }
    }

    // 探索のための線分と候補レーンとの交点を求め，最も近いものを取得
    double minDistance = INFINITY;
    Lane* resultLane = NULL;
    for (unsigned int i=0; i<candidateLanes.size(); i++)
    {
        // 交点の一時保管変数
        AmuPoint crsPoint;
        bool isIntersect
            = candidateLanes[i]->createIntersectionPoint(searchLine,
                                                         &crsPoint);
        if (isIntersect)
        {
            double distance
                = beginPoint.distance(crsPoint);
            // 距離が小さいものを選ぶ
            if(distance < minDistance)
            {
                resultLane = candidateLanes[i];
                minDistance = distance;
            }
        }
    }

    return resultLane;
}

//======================================================================
void LaneInSection::print() const
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

    if (!_leftLanes.empty())
    {
        cout << "LeftLanes:" << endl;
        for (map<AmuInterval, Lane*>::const_iterator cil = _leftLanes.begin();
             cil != _leftLanes.end();
             cil++)
        {
            cout << "  [" << (*cil).first.lower()
                 << ", "  << (*cil).first.upper()
                 << "] - ID: " << (*cil).second->id() << endl;
        }
    }
    else
    {
        cout << "LeftLanes: none" << endl;
    }

    if (!_rightLanes.empty())
    {
        cout << "RightLanes:" << endl;
        for (map<AmuInterval, Lane*>::const_iterator cil = _rightLanes.begin();
             cil != _rightLanes.end();
             cil++)
        {
            cout << "  [" << (*cil).first.lower()
                 << ", "  << (*cil).first.upper()
                 << "] - ID: " << (*cil).second->id() << endl;
        }
    }
    else
    {
        cout << "RightLanes: none" << endl;
    }
    cout << endl;
}

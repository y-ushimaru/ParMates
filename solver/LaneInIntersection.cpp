/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LaneInIntersection.h"
#include "Intersection.h"
#include "Section.h"
#include "Conf.h"
#include <iostream>
#include <cassert>

using namespace std;

//======================================================================
LaneInIntersection::LaneInIntersection(const std::string& id,
                                       const Connector* ptBegin,
                                       const Connector* ptEnd,
                                       LaneBundle* parent)
    : Lane(id, ptBegin, ptEnd, parent)
{
}

//======================================================================
bool LaneInIntersection::setCollision()
{
    Intersection* parent = dynamic_cast<Intersection*>(_parent);
    assert(parent);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // collisionLanesの決定
    _collisionLanesInIntersection.clear();
    _collisionLanesInSection.clear();

    const RMAPLAN* lanes = parent->lanes();
    CITRMAPLAN itl;
    // レーンが交点を持つかどうか判定する
    for (itl = lanes->begin(); itl != lanes->end(); itl++)
    {
        if ((*itl).second==this
            && (find(_collisionLanesInIntersection.begin(),
                     _collisionLanesInIntersection.end(),
                     (*itl).second)
                != _collisionLanesInIntersection.end()))
        {
            // 自分自身か既に登録済みであれば無視
            continue;
        }

        AmuPoint tmpPoint;
        vector<Lane*> lookupLanes;
        if (this->createIntersectionPoint
            ((*itl).second->lineSegment(), &tmpPoint))
        {
            // 交点があれば登録
            _collisionLanesInIntersection.push_back((*itl).second);
            
            // 上流にさかのぼる
            const vector<Lane*>* previousLanes
                = (*itl).second->previousLanes();
            lookupLanes.insert(lookupLanes.end(),
                               previousLanes->begin(),
                               previousLanes->end());

            while (!lookupLanes.empty())
            {
                if (parent->isMyLane(lookupLanes[0]))
                {
                    // 交差点内レーンの場合
                    // 自分自身か既に登録済みであれば無視
                    if (lookupLanes[0]==this
                        && (find(_collisionLanesInIntersection.begin(),
                                 _collisionLanesInIntersection.end(),
                                 lookupLanes[0])
                            != _collisionLanesInIntersection.end()))
                    {
                        // 何もしない
                    }
                    else
                    {
                        // 新たに登録する
                        _collisionLanesInIntersection
                            .push_back(lookupLanes[0]);
                        // 更に上流レーンに展開する
                        const vector<Lane*>* morePreviousLanes
                            = lookupLanes[0]->previousLanes();
                        lookupLanes.insert
                            (lookupLanes.end(),
                             morePreviousLanes->begin(),
                             morePreviousLanes->end());
                    }
                }
                else
                {
                    // 単路部内レーンの場合
                    // 既に登録済みであれば無視
                    if (find(_collisionLanesInSection.begin(),
                             _collisionLanesInSection.end(),
                             lookupLanes[0])
                        != _collisionLanesInSection.end())
                    {
                        // 何もしない
                    }
                    else
                    {
                        _collisionLanesInSection
                            .push_back(lookupLanes[0]);
                        // これ以上展開しない
                    }
                }
                // 先頭の要素の処理が終わったので削除する
                lookupLanes.erase(lookupLanes.begin());
                if (lookupLanes.empty())
                {
                    break;
                }
            }
        }
    }
    return true;
}

//======================================================================
vector<Lane*>* LaneInIntersection::collisionLanesInIntersection()
{
    return &_collisionLanesInIntersection;
}

//======================================================================
vector<Lane*>* LaneInIntersection::collisionLanesInSection()
{
    return &_collisionLanesInSection;
}

//======================================================================
void LaneInIntersection::print() const
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

    if (!_collisionLanesInIntersection.empty())
    {
        cout << "CollisionLanesInIntersection:" << endl;
        for (unsigned int i=0; i<_collisionLanesInIntersection.size(); i++)
        {
            cout << "  ID: " << _collisionLanesInIntersection[i]->id() << endl;
        }
    }
    else
    {
        cout << "CollisionLanesInIntersection: none" << endl;
    }

    if (!_collisionLanesInSection.empty())
    {
        cout << "CollisionLanesInSection:" << endl;
        for (unsigned int i=0; i<_collisionLanesInSection.size(); i++)
        {
            cout << "  ID: " << _collisionLanesInSection[i]->id()
                 << " in Section: "
                 << _collisionLanesInSection[i]->parent()->id()
                 << endl;
        }
    }
    else
    {
        cout << "CollisionLanesInSection: none" << endl;
    }
    cout << endl;
}

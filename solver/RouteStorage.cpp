/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RouteStorage.h"
#include "RouteComponent.h"
#include "RouteComponentKeyBase.h"
#include "RoutingManager.h"
#include "Intersection.h"
#include "Random.h"
#include "GVManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <climits>

using namespace std;

//######################################################################
/// 経路要素の消去条件の判定に用いるクラス
class ComponentErasable
{
public:
    /// 経路要素が削除可能であればtrueを返す
    bool operator()(const RouteComponent* component) const
    {
        return !(component->isStatic());
    }
};

//######################################################################
RouteStorage::RouteStorage()
{
    _components.clear();
    _componentsAdded.clear();
    _componentsReordered.clear();
    _rnd = Random::randomGenerator();
    _isUsed = false;
}

//======================================================================
RouteStorage::RouteStorage(Intersection* inter)
{
    _inter = inter;
    _components.clear();
    _componentsAdded.clear();
    _componentsReordered.clear();
    _rnd = Random::randomGenerator();
    _isUsed = false;
}

//======================================================================
Intersection* RouteStorage::intersection() const
{
    return _inter;
}

//======================================================================
RouteStorage::~RouteStorage()
{
    for(deque<RouteComponent*>::iterator itr = _components.begin();
        itr != _components.end();
        itr++)
    {
        if (*itr)
        {
            delete (*itr);
        }
    }
    _components.clear();

    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
void RouteStorage::addRouteComponentDirectly
(const RouteComponentKeyBase* key,
 int count,
 const std::vector<const Intersection*>* route)
{
    RouteComponent* component = new RouteComponent();
    component->setComponent(key, route);
    component->setCount(count);
 
    /*
     * 固定サイズとするため，新たな要素をpush_backする前に
     * リストの先頭の要素をpop_frontする
     */
    if(_components.size()
       > static_cast<unsigned int>
       (GVManager::getNumeric("VEHICLE_CACHE_ROUTING_SIZE")))
    {
        delete _components[0];
        _components.pop_front();
    }

    // 最後にpush_back
    _components.push_back(component);
}

//======================================================================
void RouteStorage::addRouteComponent
(const std::string& userId,
 RoutingManager* routingManager,
 const RouteComponentKeyBase* key,
 const std::vector<const Intersection*>* route)
{
    // 同じ結果が既に格納されていないかチェック
    for (deque<RouteComponent*>::iterator itr=_components.begin();
         itr != _components.end();
         itr++)
    {
        if ((*itr)->isEqualComponent
            (const_cast<RouteComponentKeyBase*>(key), route))
        {
            // 同じ結果が見つかった場合，使用回数をカウントアップ
            (*itr)->countUp();
            return;
        }
    }

    // 同じ結果が見つからなかった場合
    RouteComponent* component = new RouteComponent();
    component->setComponent(key, route);
    component->setCount(1);
    
#ifdef _OPENMP
#pragma omp critical (addRouteComponent)
    {
#endif //_OPENMP

        // いったん_componentsAddedに格納・ソート
        /*
         * renewRouteComponentでまとめて更新する．
         */    
        _componentsAdded.insert(make_pair(userId, component));
        if (!_isUsed)
        {
            _isUsed = true;
            routingManager->addUsedRouteStorage(this);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP    
}

//======================================================================
bool RouteStorage::searchRouteComponent
(const std::string& userId,
 RoutingManager* routingManager,
 RouteComponentKeyBase* key,
 std::vector<const Intersection*>* result_route)
{
    vector<RouteComponent*> candidates;
    vector<int> candCounts;
    int sumCounts = 0;
    candidates.clear();
    candCounts.clear();

    for (deque<RouteComponent*>::iterator itr=_components.begin();
         itr != _components.end();
         itr++)
    {
        if ((*itr)->hasEqualKey(key))
        {
            candidates.push_back(*itr);
            candCounts.push_back((*itr)->count());
            sumCounts += (*itr)->count();
        }
    }
    if (candidates.empty())
    {
        // 条件を満たす結果が見つからなかった
        return false;
    }

    if (sumCounts > INT_MAX/2)
    {
        sumCounts = 0;
        // シミュレーションを繰り返すとcountがあふれるため補正
        for (unsigned int i=0; i<candidates.size(); i++)
        {
            candidates[i]->divideCountBy(2);
            candCounts[i] = candidates[i]->count();
            sumCounts += candidates[i]->count();
        }
    }
    
    // コピーする候補の決定
    int rnd = Random::uniform(_rnd, sumCounts);
    int index = 0;
    while (true)
    {
        if (rnd<candCounts[index])
        {
            break;
        }
        else
        {
            rnd -= candCounts[index];
            index++;
        }
    }
    candidates[index]->countUp();

    // 結果のコピー
    const vector<const Intersection*>*
        route = candidates[index]->route();
    if (route->size()<2)
    {
        // 正しい結果ではない
        return false;
    }
    result_route->clear();
    result_route->resize(route->size());
    copy(route->begin(), route->end(), result_route->begin());
    
#ifdef _OPENMP
#pragma omp critical (searchRouteComponent)
    {
#endif //_OPENMP

        // 選択された要素を保持しrenewRouteComponentでまとめて更新する

        // 重複チェック
        /*
         * 同時に経路再探索を行った場合など，
         * 同一のポインタが2回登録されることを防ぐ
         * (メモリ二重開放の抑止)
         */
        map<string, RouteComponent*, less<string> >::iterator itr;
        bool isFound = false;
        for (itr=_componentsReordered.begin();
             itr!=_componentsReordered.end();
             itr++)
        {
            if ((*itr).second == candidates[index])
            {
                isFound = true;
                break;
            }
        }
        if (!isFound)
        {
            _componentsReordered.insert(make_pair(userId,
                                                  candidates[index]));
        }

        if (!_isUsed)
        {
            _isUsed = true;
            routingManager->addUsedRouteStorage(this);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP

    return true;
}

//======================================================================
void RouteStorage::renewRouteComponent()
{
    // 必ず_componentsReordered -> _componentsAddedの順で処理する

    // 結果を格納するdeque
    deque<RouteComponent*> renewComponents;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // _componentsReorderedの処理
    /*
     * 重複を確認しつつ新しいコンテナへコピー
     */
    deque<RouteComponent*>::iterator itr1;
    map<string, RouteComponent*, less<string> >::iterator itr2;

    for (itr1=_components.begin(); itr1!=_components.end(); itr1++)
    {
        bool isComponentFound = false;
        for (itr2=_componentsReordered.begin();
             itr2!=_componentsReordered.end();
             itr2++)
        {
            if ((*itr1)==(*itr2).second)
            {
                isComponentFound = true;
                break;
            }
        }

        // _componentReorderedに見つかった場合は後で登録される
        if (isComponentFound)
        {
        }
        // 見つからなかった場合はこの時点で登録する
        else
        {
            renewComponents.push_back(*itr1);
        }
    }
    // 最後に_componentsReorderedの要素を追加する
    for (itr2=_componentsReordered.begin();
         itr2!=_componentsReordered.end();
         itr2++)
    {
        renewComponents.push_back((*itr2).second);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // _componentsAddedの処理
    for (itr2=_componentsAdded.begin();
         itr2!=_componentsAdded.end();
         itr2++)
    {
        renewComponents.push_back((*itr2).second);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // _componentsのサイズが大きくなりすぎたら先頭から削除
    while (renewComponents.size()
           > static_cast<unsigned int>
           (GVManager::getNumeric("VEHICLE_CACHE_ROUTING_SIZE")))
    {
        delete renewComponents[0];
        renewComponents.pop_front();
    }

    _components.swap(renewComponents);

    // 一時的な格納をクリア
    _componentsAdded.clear();
    _componentsReordered.clear();
    _isUsed = false;
}

//======================================================================
void RouteStorage::removeDynamicComponent()
{
    deque<RouteComponent*>::iterator itr
        = remove_if(_components.begin(), _components.end(),
                    ComponentErasable());
    _components.erase(itr, _components.end());
    // print();
}

//======================================================================
void RouteStorage::print(ostream& out) const
{
    for (deque<RouteComponent*>::const_iterator itr=_components.begin();
         itr != _components.end();
         itr++)
    {
        (*itr)->print(out);
    }
}

//======================================================================
void RouteStorage::printStatic(ostream& out) const
{
    for (deque<RouteComponent*>::const_iterator itr=_components.begin();
         itr != _components.end();
         itr++)
    {
        if ((*itr)->isStatic())
        {
            (*itr)->print(out);
        }
    }
}

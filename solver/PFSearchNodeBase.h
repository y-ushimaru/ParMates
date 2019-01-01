/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_SEARCH_NODE_BASE_H__
#define __PF_SEARCH_NODE_BASE_H__

#include <string>

class RoutingNode;

// 経路探索中のノード情報を格納する基底クラス
class PFSearchNodeBase
{
public:
    PFSearchNodeBase(){};
    virtual ~PFSearchNodeBase(){};

protected:    
    /// ノードの識別番号
    std::string _id;

public:
    /// ノードの識別番号をセットする
    void setId(const std::string& id)
    {
        _id = id;
    }

    /// ノードの識別番号を返す
    std::string id() const
    {
        return _id;
    }

protected:
    /// 経路探索ネットワーク上のノード
    RoutingNode* _routingNode;

public:
    /// 経路探索ネットワーク上のノードをセットする
    void setRoutingNode(RoutingNode* node)
    {
        _routingNode = node;
    }
        
    /// 経路探索ネットワーク上のノードを返す
    RoutingNode* routingNode() const
    {
        return _routingNode;
    }

protected:
    /// 前のノードの識別番号
    std::string _prevId;

public:
    /// 前のノードの識別番号をセットする
    void setPrevId(const std::string prevId)
    {
        _prevId = prevId;
    }
    
    /// 前のノードの識別番号を返す
    std::string prevId() const
    {
        return _prevId;
    }

protected:
    /// スタートからノードまでの距離
    double _distance;

public:
    /// スタートからノードまでの距離をセットする
    void setDistance(double distance)
    {
        _distance = distance;
    }
    
    /// スタートからノードまでの距離を返す
    double distance() const
    {
        return _distance;
    }

protected:
    /// スタートからノードまでの実コスト
    double _cost;

public:
    /// スタートからノードまでの実コストをセットする
    void setCost(double cost)
    {
        _cost = cost;
    }
    
    /// スタートからノードまでの実コストを返す
    double cost() const
    {
        return _cost;
    }

public:
    /// 探索状況を示すラベル
    enum SearchLabel
    {
        UNREACHED,    //< 未探索
        LABELED,      //< 探索済
        UPPERLABELED, //< 上位ノード探索済
        SCANNED       //< 確定   
    };

protected:
    /// ノードの探索状況
    SearchLabel _label;

public:
    /// ノードの探索状況をセットする
    void setLabel(SearchLabel label)
    {
        _label = label;
    }
    
    /// ノードの探索状況を返す
    SearchLabel label() const
    {
        return _label;
    }
};

#endif //__PF_SEARCH_NODE_BASE_H__

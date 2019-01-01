/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_DIJKSTRA_H__
#define __PF_DIJKSTRA_H__

#include <map>
#include <string>
#include "PFBase.h"
#include "PFSearchNodeBase.h"

class RoutingManager;

struct SearchNode;

/// ダイクストラ法を用いた大域的経路探索器
/**
 * PFBaseクラスの派生クラス
 *
 * @ingroup Routing
 */
class PFDijkstra : public PFBase
{
public:
    PFDijkstra(RoutingManager* routingManager);
    PFDijkstra(RoutingManager* routingManager,
               const RoutingNetwork* routingNetwork);
    virtual ~PFDijkstra();

protected:
    /// 経路のRoutingNodeの集合を@p result_nodesに格納する
    /**
     * 目的地のIntersectionに接続するSectionは
     * 複数ある可能性があるため，goalNodeはvector．
     */
    virtual void _search(std::vector<RoutingNode*>* result_nodes);

    /// 探索状態を元に戻す
    /**
     * 探索器の状態を元に戻す_resetPFStatusとは異なる
     */
    virtual void _resetRoutingStatus();

    /// 探索中のノード情報を格納するクラス
    class PFDSearchNode : public PFSearchNodeBase
    {
    public:
        PFDSearchNode(){};
        virtual ~PFDSearchNode(){};
    };

private:
    /// 探索中の情報を保持するコンテナ
    std::map<std::string,
             PFDSearchNode*,
             std::less<std::string> > _snodes;

public:
    // SearchNodeの比較関数
    static bool isPrior(PFDSearchNode* lhs, PFDSearchNode* rhs);
};

#endif //__PF_DIJKSTRA_H__

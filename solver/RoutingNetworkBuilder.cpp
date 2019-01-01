#include "RoutingNetworkBuilder.h"
#include "RoadMap.h"
#include "Section.h"
#include "Intersection.h"
#include "RoutingManager.h"
#include "RoutingNetwork.h"
#include "RoutingNode.h"
#include "RoutingLink.h"
#include "AmuConverter.h"
#include "Conf.h"
#include <iostream>
#include <cassert>
#include <climits>

using namespace std;

//======================================================================
RoutingNetworkBuilder::RoutingNetworkBuilder()
{
    _currentRoutingNetwork = NULL;
    _routingNetworkContainer = NULL;
    _numNodes = 0;
    _numLinks = 0;
}

//======================================================================
RoutingNetworkBuilder::~RoutingNetworkBuilder()
{
    if (!_currentRoutingNetwork)
    {
        delete _currentRoutingNetwork;
    }
}

//======================================================================
void RoutingNetworkBuilder::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

//======================================================================
void RoutingNetworkBuilder::setRoutingNetworkContainer
(vector<RoutingNetwork*>* container)
{
    _routingNetworkContainer = container;
}

//======================================================================
bool RoutingNetworkBuilder::buildRoutingNetwork(unsigned int networkRank)
{
    cout << "Build RoutingNetwork[" << networkRank << "]" << endl;
    if (networkRank==0)
    {
        buildRoutingLineGraph();
    }
    else
    {
        buildRoutingHigherGraph(networkRank);
    }
    assert(_currentRoutingNetwork);

    return true;
}

//======================================================================
bool RoutingNetworkBuilder::buildRoutingLineGraph()
{
    /*
     * ランク0のグラフを作成する
     * これはIntersectionをノード，Sectionをリンクと見立てた
     * RoadMapに対する線グラフ (Line Graph) である．
     */
    if (_currentRoutingNetwork)
    {
        delete _currentRoutingNetwork;
    }
    _currentRoutingNetwork = new RoutingNetwork();
    _currentRoutingNetwork->setNetworkRank(0);

    bool result = true;

    /*
     * 最下層(ランク0)のネットワークは省略を行わない
     */

    // ノード生成
    result = _buildRoutingNodes();
    if (!result)
    {
        cerr << "ERROR: create routing nodes failed." << endl;
        return false;
    }

    // リンク生成
    result = _buildRoutingLinks();
    if (!result)
    {
        cerr << "ERROR: create routing links failed." << endl;
        return false;
    }

    return result;
}

//======================================================================
bool RoutingNetworkBuilder::buildRoutingHigherGraph
(unsigned int networkRank)
{
    // ランクn (n>=1) のグラフを作成する
    if (_currentRoutingNetwork)
    {
        delete _currentRoutingNetwork;
    }
    _currentRoutingNetwork = new RoutingNetwork();
    _currentRoutingNetwork->setNetworkRank(networkRank);

    bool result = true;

    // ノード生成
    result = _buildRoutingHigherNodes(networkRank);
    if (!result)
    {
        cerr << "ERROR: create routing nodes failed." << endl;
        return false;
    }

    // リンク生成
    result = _buildRoutingHigherLinks(networkRank);
    if (!result)
    {
        cerr << "ERROR: create routing links failed." << endl;
        return false;
    }

    // ノード昇格
    /*
     * ネットワーク情報をもとに，1つ上位のネットワークに
     * 含めるべきノードを求め，rankを上げる
     */
    result = _promoteRoutingNodes(networkRank);
    if (!result)
    {
        cerr << "ERROR: promote nodes failed." << endl;
        return false;
    }

    return result;
}

//======================================================================
bool RoutingNetworkBuilder::buildInterlayerLinks(unsigned int networkRank)
{
    bool result = true;

    /*
     * すべてのレイヤ間リンクは，始点側のネットワークランクに
     * 属するものとする
     */
    /*
     * 全階層のネットワークが生成されてからの処理であるので
     * _currentRoutingNetworkは使用しない
     */ 

    RoutingNetwork* network
        = (*_routingNetworkContainer)[networkRank];
    RoutingNetwork* lowerNetwork
        = network->lowerNetwork();

    const RNMAPN* nodes = network->nodes();
    CITRNMAPN citn;

    //--------------------------------------------------------------
    // 下位→上位へのリンク (upwardLink)
    for (citn = nodes->begin(); citn != nodes->end(); citn++)
    {
        RoutingNode* upperNode = (*citn).second->upperNode();
        if (upperNode)
        {
            RoutingLink* upwardLink
                = new RoutingLink(NULL, (*citn).second, upperNode);
            upwardLink->setRank(networkRank);
            upwardLink->setLinkType(RoutingLink::UPWARD);
            
            // サブIDの付与
            string beginId = (*citn).second->subId();
            string endId   = upperNode->subId();
            string subId
                = beginId.substr(0, NUM_FIGURE_FOR_INTERSECTION)
                + endId.substr(NUM_FIGURE_FOR_INTERSECTION,
                               NUM_FIGURE_FOR_INTERSECTION);
            upwardLink->setSubId(subId);

            // リンクを登録する
            network->addLink(upwardLink);
            (*citn).second->addOutLink(upwardLink);
            (*citn).second->addUpwardLink(upwardLink);
            upperNode->addInLink(upwardLink);
        }
    }

    if (!lowerNetwork)
    {
        return result;
    }

    //--------------------------------------------------------------
    // 上位→下位へのリンク (downwardLink)
    for (citn = nodes->begin(); citn != nodes->end(); citn++)
    {
        RoutingNode* lowerNode
            = (*citn).second->lowerNode();
        assert(lowerNode);

        /*
         * 下位ノードがdownwardLinkを持つ場合か，
         * networkRankとnetworkRank-1で出次数が異なる場合か，
         * 下流リンクのうちいずれかが集約リンクである場合のみ
         * 下位ノードへのリンクを生成
         */
        bool isLinkBuilt = false;
        if (!isLinkBuilt)
        {
            if (lowerNode->downwardLink())
            {
                isLinkBuilt = true;
            }
        }
        if (!isLinkBuilt)
        {
            if ((*citn).second->outdegreeInSameNetworkRank()
                != lowerNode->outdegreeInSameNetworkRank())
            {
                isLinkBuilt = true;
            }
        }
        if (!isLinkBuilt)
        {
            const vector<RoutingLink*>* outlinks
                = (*citn).second->outLinks();
            for (unsigned int i=0; i<outlinks->size(); i++)
            {
                if ((*outlinks)[i]->isAggregatedLink())
                {
                    isLinkBuilt = true;
                    break;
                }
            }
        }
            
        // フラグが立った場合のみリンク生成
        if (isLinkBuilt)
        {
            RoutingLink* downwardLink
                = new RoutingLink(NULL, (*citn).second, lowerNode);
            downwardLink->setRank(networkRank);
            downwardLink->setLinkType(RoutingLink::DOWNWARD);

            // サブIDの付与
            string beginId = (*citn).second->subId();
            string endId   = lowerNode->subId();
            string subId
                = beginId.substr(0, NUM_FIGURE_FOR_INTERSECTION)
                + endId.substr(NUM_FIGURE_FOR_INTERSECTION,
                               NUM_FIGURE_FOR_INTERSECTION);
            downwardLink->setSubId(subId);
                
            // リンクを登録する
            network->addLink(downwardLink);
            (*citn).second->addOutLink(downwardLink);
            (*citn).second->addDownwardLink(downwardLink);
            lowerNode->addInLink(downwardLink);
        }
    }

    return result;
}

//======================================================================
bool RoutingNetworkBuilder::setNodeProperties(unsigned int networkRank)
{
    const RNMAPN* nodes
        = (*_routingNetworkContainer)[networkRank]->nodes();

    for (CITRNMAPN itn = nodes->begin(); itn != nodes->end(); itn++)
    {
        (*itn).second->setProperty();
    }
    return true;
}

//======================================================================
bool RoutingNetworkBuilder::setLinkProperties(unsigned int networkRank)
{
    const vector<RoutingLink*>* links
        = (*_routingNetworkContainer)[networkRank]->links();
     
    for (unsigned int i=0; i<links->size(); i++)
    {
        (*links)[i]->setProperty();
    }
    return true;
}

//======================================================================
bool RoutingNetworkBuilder::_buildRoutingNodes()
{
    const RMAPS* sections = _roadMap->sections();
    unsigned int numSections  = sections->size();
    unsigned int numProcessed = 0;

    for (CITRMAPS its = sections->begin();
         its != sections->end();
         its++)
    {
        if (_numNodes == ULONG_MAX)
        {
            cerr << "ERROR: number of routing nodes reached ULONG_MAX("
                 << ULONG_MAX << ")" << endl;
            exit(EXIT_FAILURE);
        }

        // 上り方向
        if ((*its).second->lanesWithDirection(true).size())
        {
            RoutingNode* node
                = new RoutingNode((*its).second,
                                  true);
            node->setNetworkRank(0);
            node->setLowestNode(node);
            _currentRoutingNetwork->addNode(node);
            _numNodes++;
        }

        // 下り方向
        if ((*its).second->lanesWithDirection(false).size())
        {
            RoutingNode* node
                = new RoutingNode((*its).second,
                                  false);
            node->setNetworkRank(0);
            node->setLowestNode(node);
            _currentRoutingNetwork->addNode(node);
            _numNodes++;
        }
        numProcessed++;
        if (numProcessed%1000 == 0)
        {
            cout << "  Node: " << numProcessed
                 << "/" << numSections << " processed." << endl;
        }

    }
    return true;
}

//======================================================================
bool RoutingNetworkBuilder::_promoteRoutingNodes(unsigned int networkRank)
{
    // ランク1以上のネットワークが対象
    if (networkRank<2)
    {
        return true;
    }

    bool result = true;
    vector<RoutingNode*> promotedNodes;

    const RNMAPN* nodes = _currentRoutingNetwork->nodes();
    for (CITRNMAPN citn = nodes->begin();
         citn != nodes->end();
         citn++)
    {
        const RoutingNode* target = (*citn).second;

        // 上位ネットワークに含まれないノードは起点の対象外
        if (target->rank()<networkRank+1)
        {
            continue;
        }

        //--------------------------------------------------------------
        // 下流方向へ拡張
        bool isPromoted = false;

        if (target->straightOutNode()
            && target->straightOutNode()->rank()<=networkRank)
        {
            // 直進下流のノードのランクが低い場合は起点の候補
            isPromoted = true;
        }
        else
        {
            // 上位ネットワークにおける入次数，出次数を前もって取得
            /*
             * indegree, outdegreeはnodeのランクで判別
             */
            int indeg  = target->indegree(networkRank+1);
            int outdeg = target->outdegree(networkRank+1);
            
            // 入次数が出次数より高くなければ起点にならない
            /*
             * 縮約対象のノードはindeg==outdeg==1であるのでここで除ける
             */
            if (indeg > outdeg)
            {
                isPromoted = true;
            }
        }

        /*
         * 自分を通って直進するネットワークのノードを次々に取得し，
         * 指定ホップまでの間に始点と上位ネットワークのランクを持つ
         * ノードに到達したら昇格
         */
        vector<RoutingNode*> candidates;
        if (isPromoted)
        {
            isPromoted = false;
            for (int loop=0; loop<20; loop++)
            {
                const RoutingNode* nextTarget
                    = target->straightOutNode();
                
                if (!nextTarget)
                {
                    break;
                }
                
                // 最初のターゲットは起点で上位ノードが必ず存在するため除外
                if (loop!=0)
                {
                    candidates.push_back
                        (const_cast<RoutingNode*>(target));
                }
                
                // 上位ノードが存在するノードに到達したら終了
                if (nextTarget->rank()>=networkRank+1)
                {
                    isPromoted = true;
                    break;
                }
                
                target = nextTarget;
            }
        }
        
        if (isPromoted)
        {
            // ノード昇格のための登録
            for (unsigned int i=0; i<candidates.size(); i++)
            {
                promotedNodes.push_back(candidates[i]);
            }
        }
    }

    // ノードの昇格
    for (unsigned int i=0; i<promotedNodes.size(); i++)
    {
        RoutingNode* prmNode = promotedNodes[i];
        if (prmNode->rank()==networkRank+1)
        {
            // 既に昇格済み
            continue;
        }
        while (prmNode)
        {
            prmNode->setRank(networkRank+1);
            prmNode = prmNode->lowerNode();
        }
    }

    return result;
}

//======================================================================
bool RoutingNetworkBuilder::_buildRoutingHigherNodes
(unsigned int networkRank)
{
    assert(networkRank > 0
           && (*_routingNetworkContainer)[networkRank-1]);
    
    /*
     * 指定したランクより1だけ低いランクのノードの集合から
     * 条件を満たすノードを抽出し指定したランクに登録する
     */
    const RNMAPN* lowerNodes
        = ((*_routingNetworkContainer)[networkRank-1])->nodes();

    for (CITRNMAPN citn = lowerNodes->begin();
         citn != lowerNodes->end();
         citn++)
    {
        RoutingNode* targetNode = (*citn).second;

        // 対象ノードのランクが低い場合は上位ネットワークに追加しない
        if (targetNode->rank() < networkRank)
        {
            continue;
        }

        // 指定ランク以上のノードからの入次数
        int indeg = targetNode->indegree(networkRank);

        // 指定ランク以上のノードへの出次数
        int outdeg = targetNode->outdegree(networkRank);

        /*
         * 入次数，出次数がともに0の場合は上位ネットワークに追加しない
         */
        if ((indeg == 0 && outdeg == 0))
        {
            continue;
        }
        /*
         * 入次数，出次数がともに1の場合は上位ネットワークに追加しない
         */
        else if (indeg == 1 && outdeg == 1)
        {
            continue;
        }

        // 除外されなかったノードを上位のネットワークに登録する
        RoutingNode* upperNode = new RoutingNode(targetNode->section(),
                                                 targetNode->isUp());
        upperNode->setNetworkRank(networkRank);

        // 上層のノードに下層のノードを登録する
        upperNode->setLowerNode(targetNode);
        upperNode->setLowestNode(targetNode->lowestNode());

        // 下層のノードに上層のノードを登録する
        targetNode->setUpperNode(upperNode);

        _currentRoutingNetwork->addNode(upperNode);
    }

    return true;
}

//======================================================================
bool RoutingNetworkBuilder::_buildRoutingLinks()
{
    int numLinks = 0;

    const RNMAPN* nodes = _currentRoutingNetwork->nodes();
    unsigned int numNodes     = nodes->size();
    unsigned int numProcessed = 0;

    for (CITRNMAPN itn = nodes->begin(); itn != nodes->end(); itn++)
    {
        RoutingNode* beginNode = (*itn).second;
        const Section* thisSection = beginNode->section();

        Intersection* via
            = thisSection->intersection(beginNode->isUp());

        if (via->numNext()==1)
        {
            // ODノードはリンク化しない
            continue;
        }
        else
        {
            /*
             * 次数2以上の場合はviaの先に必ずRoutingNodeがある
             */
            for (int j=0; j<via->numNext(); j++)
            {
                Section* nextSection
                    = via->nextSection(j);
                const Intersection* from
                    = thisSection
                    ->anotherIntersection(via);
                const Intersection* to
                    = nextSection
                    -> anotherIntersection(via);
                if (!(via->isReachable(from, to)))
                {
                    // 交差点内で到達不可能な場合はリンクしない
                    continue;
                }
                
                bool isUp
                    = nextSection->isUp(via, to);
                RoutingNode* endNode
                    = _currentRoutingNetwork
                    -> convertS2N(nextSection, isUp);
                
                RoutingLink* link
                    = new RoutingLink(via,
                                      beginNode,
                                      endNode);
                link->setRank(0);
                link->setLinkType(RoutingLink::FLAT);

                // サブIDの付与
                string beginId = beginNode->subId();
                string endId   = endNode->subId();
                string subId
                    = beginId.substr(0, NUM_FIGURE_FOR_INTERSECTION)
                    + endId.substr(NUM_FIGURE_FOR_INTERSECTION,
                                   NUM_FIGURE_FOR_INTERSECTION);
                link->setSubId(subId);

                numLinks++;
                _currentRoutingNetwork->addLink(link);

                beginNode->addOutLink(link);
                endNode->addInLink(link);
            }
        }
        numProcessed++;
        if (numProcessed%1000 == 0)
        {
            cout << "  Link: " << numProcessed
                 << "/" << numNodes << "  processed." << endl;
        }
    }
    return true;
}

//======================================================================
bool RoutingNetworkBuilder::_buildRoutingHigherLinks
(unsigned int networkRank)
{
    int numLinks = 0;

    assert(networkRank > 0
           && (*_routingNetworkContainer)[networkRank-1]);

    const RNMAPN* nodes = _currentRoutingNetwork->nodes();
    for (CITRNMAPN citn = nodes->begin();
         citn != nodes->end();
         citn++)
    {
        assert((*citn).second->lowerNode());

        // inlinkとoutlinkのどちらかのみの処理でよい
        const vector<RoutingLink*>* lowerOutLinks
            = (*citn).second->lowerNode()->outLinks();
        if(lowerOutLinks->empty())
        {
            continue;
        }

        for (unsigned int i=0; i<lowerOutLinks->size(); i++)
        {
            assert((*lowerOutLinks)[i]);
            RoutingLink* lowerOutLink = (*lowerOutLinks)[i];
            const RoutingNode* nextNode = lowerOutLink->endNode();
            
            if (nextNode->rank()<networkRank)
            {
                /*
                 * 隣接ノード自体のランクが低い
                 * -> 何もしない
                 */
                continue;
            }
            else if (nextNode->upperNode()==NULL)
            {
                /*
                 * 隣接ノード自体のランクは高いが上位ノードがない
                 * -> 縮約されているため，接続先を探す
                 */
                vector<const RoutingNode*> abbrNodes;
                vector<const RoutingLink*> abbrLinks;

                vector<const RoutingLink*> tmpInlinks;
                nextNode->getInLinks(networkRank, &tmpInlinks);
                assert(tmpInlinks.size()==1);
                abbrLinks.push_back(tmpInlinks[0]);

                while (true)
                {
                    if (nextNode->upperNode()!=NULL)
                    {
                        // 縮約されていないノードが見つかった
                        break;
                    }
                                        
                    // 省略されたノードとリンクを追加
                    vector<const RoutingLink*> tmpOutLinks;
                    nextNode->getOutLinks(networkRank, &tmpOutLinks);
                    assert(tmpOutLinks.size()==1);
                    abbrNodes.push_back(nextNode);
                    abbrLinks.push_back(tmpOutLinks[0]);
                    
                    for (unsigned int j=0; j<tmpOutLinks.size(); j++)
                    {
                        // nextLinks->size()は1であるはず...
                        if (tmpOutLinks[j]
                            ->endNode()
                            ->rank() >= networkRank)
                        {
                            nextNode = tmpOutLinks[j]->endNode();
                            break;
                        }
                    }
                }
                RoutingLink* link
                    = new RoutingLink(NULL,
                                      (*citn).second,
                                      nextNode->upperNode());
                link->setRank(networkRank);
                link->setLinkType(RoutingLink::FLAT);

                // サブIDの付与
                string beginId = (*citn).second->subId();
                string endId   = nextNode->subId();
                string subId
                    = beginId.substr(0, NUM_FIGURE_FOR_INTERSECTION)
                    + endId.substr(NUM_FIGURE_FOR_INTERSECTION,
                                   NUM_FIGURE_FOR_INTERSECTION);
                link->setSubId(subId);

                for (unsigned int i=0; i<abbrNodes.size(); i++)
                {
                    link->addLowerNode(abbrNodes[i]);
                }
                for (unsigned int i=0; i<abbrLinks.size(); i++)
                {
                    link->addLowerLink(abbrLinks[i]);
                    const_cast<RoutingLink*>(abbrLinks[i])
                        ->setUpperLink(link);
                }

                numLinks++;
                _currentRoutingNetwork->addLink(link);

                (*citn).second->addOutLink(link);
                const_cast<RoutingNode*>(nextNode->upperNode())
                    ->addInLink(link);
            }
            else
            {
                /*
                 * 隣接ノードのネットワークランクが高い
                 * -> 直接接続する
                 */
                RoutingLink* link
                    = new RoutingLink(lowerOutLink->intersection(),
                                      (*citn).second,
                                      nextNode->upperNode());
                link->setRank(networkRank);
                link->setLinkType(RoutingLink::FLAT);

                // サブIDの付与
                string beginId = (*citn).second->subId();
                string endId   = nextNode->upperNode()->subId();
                string subId
                    = beginId.substr(0, NUM_FIGURE_FOR_INTERSECTION)
                    + endId.substr(NUM_FIGURE_FOR_INTERSECTION,
                                   NUM_FIGURE_FOR_INTERSECTION);
                link->setSubId(subId);

                link->addLowerLink(lowerOutLink);
                lowerOutLink->setUpperLink(link);

                numLinks++;
                _currentRoutingNetwork->addLink(link);

                (*citn).second->addOutLink(link);
                const_cast<RoutingNode*>(nextNode->upperNode())
                    ->addInLink(link);
            }
        }
    }
    return true;
}

//======================================================================
RoutingNetwork* RoutingNetworkBuilder::routingNetwork()
{
    RoutingNetwork* routingNetwork = _currentRoutingNetwork;
    _currentRoutingNetwork = NULL;
    return routingNetwork;
}

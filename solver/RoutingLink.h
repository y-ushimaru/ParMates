/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROUTING_LINK_H__
#define __ROUTING_LINK_H__

#include "VehicleFamily.h"
#include "Conf.h"
#include "AmuConverter.h"
#include <vector>
#include <string>
#include <map>
#include <string>

class Section;
class Intersection;
class RoutingNode;

/// 経路探索用グラフのリンク
/**
 * RoadMapにおけるIntersectionに相当する．
 *
 * RoadMapにおけるSectionがRoutingNodeとなり，
 * Section->Intersection->Sectionの接続関係を
 * RoutingNode->RoutingLink->RoutingNodeに変換することで
 * RoutingLinkに右左折コストを与えることができる．
 * 
 * I0(1)->S0->I1(2)->S1->I2(2)->S2->I3(4)(※)のような場合
 * 両端のIntersectionの接続次数が2のSectionは縮約される．
 * つまり "I1(2)->S1->I2(2)" が1つのRoutingLinkとなる．
 * このとき，メンバ変数 _inter は上流の I1 を指す．
 * ※I0(1)とは，Intersection(ID:0)の次数が1であることを表す．
 *
 * RoutingNode <--> Section，
 * RoutingLink <--> Intersection 間の変換に注意．
 *
 * * @ingroup routing
 */
class RoutingLink
{
public:
    /// コンストラクタ
    /**
     * IDは後でつける
     */
    RoutingLink(const Intersection* inter,
                const RoutingNode* begin,
                const RoutingNode* end);

    ~RoutingLink();

    /// 識別番号を返す
    std::string id() const
    {
        return _id;
    }

    /// サブIDを返す
    std::string subId() const
    {
        return _subId;
    }

    /// 識別番号をセットする
    /**
     * SubIDにランクを表すprefixを付与してIDとする
     */
    void setSubId(const std::string& subId)
    {
        _subId = subId;
        _id = AmuConverter::itos(_rank, NUM_FIGURE_FOR_ROUTING_LAYER)
            + _subId;
    }

    /// リンクのランクを返す
    unsigned int rank() const
    {
        return _rank;
    }

    /// リンクのランクを設定する
    void setRank(unsigned int rank)
    {
        _rank = rank;
        _id = AmuConverter::itos(_rank, NUM_FIGURE_FOR_ROUTING_LAYER)
            + _subId;
    }

    /// リンクの種別の列挙
    enum RoutingLinkType
    {
        FLAT,    //< 同一ランク内のリンク
        UPWARD,  //< 上位層へのリンク
        DOWNWARD //< 階層へのリンク
    };

    /// リンクの種別を設定する
    void setLinkType(RoutingLinkType type)
    {
        _linkType = type;
    }
 
    /// 交差点を返す
    const Intersection* intersection() const
    {
        return _inter;
    }

    /// 上位リンクを設定する
    void setUpperLink(const RoutingLink* upperLink)
    {
        _upperLink = upperLink;
    }
    
    /// 上位リンクを返す
    const RoutingLink* upperLink() const
    {
        return _upperLink;
    }
    
    /// ノードを内包する
    void addLowerNode(const RoutingNode* node)
    {
        _lowerNodes.push_back(node);
    }

    /// リンクを内包する
    void addLowerLink(const RoutingLink* link)
    {
        _lowerLinks.push_back(link);
    }

    /// 内包する最下位ノードを返す
    const std::vector<const RoutingNode*>* includedLowestNodes() const
    {
        return &_includedLowestNodes;
    }

    /// 集約リンクかどうか
    /**
     * 内部に複数のリンクを内包するかどうかで判別する
     */
    bool isAggregatedLink() const;

    /// すべての集約リンクを展開して内包するノードを返す
    void getAllAbbrNodes
    (std::vector<const RoutingNode*>* result_nodes) const;

    /// 始端ノードを返す
    const RoutingNode* beginNode() const
    {
        return _beginNode;
    }

    /// 終端ノードを返す
    const RoutingNode* endNode() const
    {
        return _endNode;
    }

    /// 全コストをセットする
    /*
     * シミュレーション開始時に呼ばれる．
     * シミュレーション中には時間コストのみ更新される．
     *
     * 引数の配列サイズは呼出側でチェックする．
     */
    void setCosts(const double costs[])
    {
        for (unsigned int i=0; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
        {
            _costs[i] = costs[i];
        }
    }

    /// 時間コストを更新する
    void renewTimeCosts(const double time)
    {
        _costs[VEHICLE_ROUTING_PARAMETER_OF_TIME] = time;
    }

    /// i番目のコストを返す
    double cost(int i) const
    {
        return _costs[i];
    }

    /// 属性を設定する
    void setProperty();

    /// 距離を返す
    double length() const
    {
        return _length;
    }

    /// リンクが交差点の直進を含むかを返す
    bool isStraight() const
    {
        return _isStraight;
    }

    /// リンクが交差点の左折を含むか
    bool isLeft() const
    {
        return _isLeft;
    }

    /// リンクが交差点の右折を含むか
    bool isRight() const
    {
        return _isRight;
    }

    /// リンクが示す単路の車線数の最小値を返す
    unsigned int width() const
    {
        return _width;
    }

    /// このリンクに設定されたホワイトリストを返す
    const std::vector<std::string>* whitelistedVehicleTypes() const
    {
        return &_whitelistedVehicleTypes;
    }

    /// このリンクに設定されたブラックリストを返す
    const std::vector<std::string>* blacklistedVehicleTypes() const
    {
        return &_blacklistedVehicleTypes;
    }

    /// このリンクに設定された経路選択確率テーブルを返す
    const std::map<VehicleType, double>* routingProbability() const
    {
        return &_routingProbability;
    }

    /// このリンクを指定した@p typeの車両が通過できるか
    bool mayPass(VehicleType type) const;
    
    /// 経路探索時に指定した@p typeの車両が展開する確率を返す
    double routingProbability(VehicleType type) const;

    /// 初期コストを設定する
    void setInitialCosts();

    /// コストを更新する
    void renewCosts();

    /// 情報を表示する
    void print() const;

private:
    /// 識別番号
    /**
     * 階層+サブID
     */
    std::string _id;

    /// サブID
    /**
     * _beginNodeのサブIDの前半+_endNodeのサブIDの後半
     * すなわち，このリンクを経由して接続することになる
     * IntersectionのIDを結合したもの
     */
    std::string _subId;

    /// リンクのランク
    /**
     * RoutingNodeと異なり各NetworkRankでnewされるため，
     * リンクのランクとネットワークのランクは必ず等しい
     */
    unsigned int _rank;
   
    /// リンクの種別
    RoutingLinkType _linkType;
    
    /// 該当する交差点
    /*
     * 縮約したリンクの場合にはNULLが入る
     */
    const Intersection* _inter;

    /// 上位のリンク
    const RoutingLink* _upperLink;
    
    /// 対応する下位のノード
    /*
     * 縮約したノードが入る．縮約しない場合はempty
     */
    std::vector<const RoutingNode*> _lowerNodes;

    /// 対応する下位のリンク
    /*
     * 縮約した場合には複数のリンクが入る
     */
    std::vector<const RoutingLink*> _lowerLinks;

    /// 最下位のノード
    /**
     * _beginNode，_endNodeは"含まない"
     */
    std::vector<const RoutingNode*> _includedLowestNodes;

    /// リンクの始点
    const RoutingNode* _beginNode;

    /// リンクの終点
    const RoutingNode* _endNode;

    /// コスト
    double _costs[VEHICLE_ROUTING_PARAMETER_SIZE];

    /// リンクの長さ
    double _length;

    /// リンクが交差点の直進を含むか
    bool _isStraight;

    /// リンクが交差点の左折を含むか
    bool _isLeft;

    /// リンクが交差点の右折を含むか
    bool _isRight;

    /// リンクが示す単路の車線数の最小値
    unsigned int _width;

    /// 通行権のホワイトリスト(通行許可)
    /**
     * @attention ホワイトリストを優先
     */
    std::vector<std::string> _whitelistedVehicleTypes;

    /// 通行権のブラックリスト
    std::vector<std::string> _blacklistedVehicleTypes;

    /// 経路探索時の探索確率テーブル
    std::map<VehicleType, double> _routingProbability;
};

#endif //__ROUTING_LINK_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __OD_NODE_GROUP_H__
#define __OD_NODE_GROUP_H__

#include <string>
#include <vector>

class ODNode;
class RandomGenerator;

/// ODグループクラス
/**
 * @ingroup roadNetwork
 */
class ODNodeGroup
{
public:
    ODNodeGroup();
    ODNodeGroup(const std::string& id);
    ~ODNodeGroup();

    /// 識別番号を返す
    const std::string& id() const;

    /// 単独ODノードを追加する
    void addODSolo(ODNode* node, double weight);

    /// 単独のODノードをランダムに返す
    void getODSolo(ODNode** result_node);

    /// ODペアを追加する
    void addODPair(ODNode* start, ODNode* goal, double weight);

    /// ODペアをランダムに返す
    void getODPair(ODNode** result_start, ODNode** result_goal);

    //==================================================================
    /** @name ODノードと重みを共に保持する内部構造体 */
    /// @{

    /// 単独のODノードと重み
    struct WeightedODSolo
    {
        ODNode* node;
        double weight;
    };

    /// ODペアと重み
    struct WeightedODPair
    {
        ODNode* start;
        ODNode* goal;
        double weight;
    };

    /// @}

    //==================================================================

    /// 情報を表示する
    void print() const;

private:
    /// 識別番号
    std::string _id;

    /// 種別を指定するための列挙子
    typedef enum
    {
        UNDEFINED,
        SOLO,
        PAIR
    } GroupType;

    /// 種別
    GroupType _type;

    /// 重みの合計
    double _sumTotal;

    /// 単独のODノードによって構成されたグループ
    std::vector<WeightedODSolo*> _odSolos;

    /// ODペアによって構成されたグループ
    std::vector<WeightedODPair*> _odPairs;

    /// 乱数生成器
    RandomGenerator* _rnd;
};

#endif //__ODNODE_GROUP_H__

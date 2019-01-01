/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __NODEASTAR_H__
#define __NODEASTAR_H__

class Intersection;

/// A*アルゴリズム用ノードクラス 
/**
 * ノードは基本的に交差点であるが、補助的に親ノードの交差点を保持する。
 * 道路は純粋なネットワークではなく、ノードでの右左折によって評価値が変わるため。
 * 
 * @sa Tree
 * @ingroup routing
 */
class NodeAStar
{
protected:
    /// スタートからのコスト
    double _gv;

    /// ゴールまでの推定コスト
    /**
     * ヒューリスティック関数によって求められる
     */
    double _hv;

    Intersection* _top;    //< 自身が指す交差点
    Intersection* _bottom; //< 親ノードのtop
 
    /// 親ノード
    NodeAStar* _parent;

    /// このノードが展開可能であるか
    bool _isAble;

public:
    /// コンストラクタ
    /**
     * @param me 自身が指す交差点
     * @param pa 親ノード
     */
    NodeAStar(Intersection* me, NodeAStar* pa);

    ~NodeAStar();
  
    /// スタートからのコストを設定する
    void setGV(double g);

    /// スタートからのコストを返す
    double gv() const;

    /// ゴールまでの推定値を設定する
    void setHV(double h);

    /// ゴールまでの推定値を返す
    double hv() const;

    /// 自身が指す交差点を返す
    Intersection* top() const;

    /// 自身が指す交差点を設定する
    void setTop(Intersection*);

    /// 親ノードが指す交差点を返す
    Intersection* bottom() const;

    /// 親ノードが指す交差点を設定する
    void setBottom(Intersection*);

    /// 親ノードを返す
    NodeAStar* parent() const;

    /// 展開可能フラグをONにする
    void enable();
  
    /// 展開可能フラグをOFFにす。
    void disable();

    /// ノードが展開可能かどうか
    bool isAble() const;

    /// 他のノードと同一(topとbottomが同じ)かどうか
    bool compare(const NodeAStar&) const;
};

#endif

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __TREE_H__
#define __TREE_H__
#include <iosfwd>
#include <map>
#include <vector>

class NodeAStar;

/// 探索木
/**
 * A*アルゴリズムによる探索に用いる。
 *  @sa NodeAStar
 *  @ingroup routing
 */
class Tree
{
public:
    Tree();

    /**
     * このクラスはノードのメインコンテナになるので
     * デストラクタの中でdeleteを行なう。
     */
    ~Tree();
  
    /// ノードを２つのコンテナ(_nodes,_leaves)に加える
    /**
     *  同じものがあり,なおかつそれの方が
     *  評価関数値が小さい場合はfalseを返し，NodeAStar::disableを呼んで
     *  明示的に展開してやらない限り展開できないようにしてから，_leavesに加える。
     */
    bool push(NodeAStar*);

    /**
     * 末端のノードのうち展開可能で,評価関数値が最小のものを返し、それを_leavesから
     * 削除する。
     */
    NodeAStar* popAble();

    /**
     * 末端のノードのうちで展開不可能で,評価関数値が最小のものを返し、それを_leavesから
     * 削除する。
     */
    NodeAStar* popUnable();

    /** 末端のノードのうち展開可能性を無視して,評価関数値が最小のものを返し、それを_leavesから
     *  削除する。*/
    NodeAStar* pop();

    /**
     * 末端のノードの内評価関数が小さいものからposition番目のものを返す。
     * 展開可能性は無視。
     */
    NodeAStar* leaf(int positon = 0) const;

    /**
     * 末端のノードの内評価関数が小さいものからposition番目のものを返す。
     * 展開可能なものしか考慮しない 
     */
    NodeAStar* leafAble(int positon = 0) const;

    /**
     * 末端のノードの内評価関数が小さいものからposition番目のものを返す。
     * 展開不可能なものしか考慮しない。
     */
    NodeAStar* leafUnable(int positon = 0) const;

    /// 格納しているノードをダンプする。
    void dump();

    /// 格納しているノードのサイズ
    int size();

    /// 要素をファイルに出力する。
    void print(std::ostream& rout);

    /// 要素をファイルに出力する。
    /**
     * @param rout 出力先ストリーム [inout]
     * @param counter 展開ステップの番号
     */
    void print(std::ostream& rout, int counter);

    //  void draw() const;
protected:
    /// すべてのノードを格納する。
    std::vector<NodeAStar*> _nodes;
  
    /// 末端のノードを格納する。
    /** keyはf=g+hとする。fについて昇順にソートされている。*/
    std::multimap<double, NodeAStar*, std::less<double> > _leaves;
  
    //
    //utility functions
    //
  
    //
    //for push()
    //
  
    /// 同じノードが存在するか
    bool _isExist(NodeAStar* node) const;

    /// 同じノードを返す
    NodeAStar* _findSame(NodeAStar* node) const;

    /// リバースノード(topとbottomが@p nodeと反対のノード)を探す
    NodeAStar* _findReverse(NodeAStar* node) const;
  
    /// nodeの祖先が先端にあれば,その祖先を先端からは消去する。
    void _rmParentFromLeaves(NodeAStar* node);
};

#endif

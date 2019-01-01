/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __BORDER_H__
#define __BORDER_H__

#include "AmuPoint.h"
#include <vector>

class AmuLineSegment;
class Connector;

/// レーン束オブジェクト同士を繋ぐ境界を表すクラス
/**
 *  Intersection と Street の境界線上に領域を持ち，
 *  それらのレーン間をつなぐ Connector を集約する．
 *
 *  @ingroup RoadNetwork
 */
class Border
{
public:
    /// 線分のみ作成．コネクタはサブクラスに任せる
    Border(const AmuPoint begin, const AmuPoint end, double roadsideWidth);

    /// コネクタとともに線分を作成
    /**
     * @p ptBegin，@p ptEnd は境界線の両端の点．デストラクタで消去されない．
     * @p din，@p doutはそれぞれこの境界面を通して交差点に流入・流出する点の数．
     */
    Border(const AmuPoint begin, const AmuPoint end, int din, int dout,
           double roadsideWidth);
  
    /// デストラクタ
    /**
     * 保持している _lineSegmentを消去する
     */
    virtual ~Border();
  
    /// 幾何的線分を得る
    /**
     * @warning 消去してはならない
     */
    const AmuLineSegment* lineSegment() const;
  
    /// 流入ポイントから順に数えて@p i 番目の Connector を返す
    /**
     * 番号は交差点に流入する方から見て左から順に0を起点に打っており，
     * 流入ポイント→流出ポイントの順に並んでいる
     */
    const Connector* connector(int i) const;

    /// 流入ポイントの集合を返す
    std::vector<const Connector*> inPoints() const;

    /// 流出ポイントの集合を返す
    std::vector<const Connector*> outPoints() const;

    /// 1:1-rの内分点にコネクタを作成する
    /**
     * コネクタの位置は開始点からの長さの割合で指定する．
     * rは追加順に大きくなっていく必要がある
     */
    bool addConnector(double r, bool isIn);

    /// 任意の位置にコネクタを作成
    /**
     * 位置に対する責任は呼出側に持たせる
     */
    bool addConnector(double x, double y, double z, bool isIn);

    /// 逆向きにしたBorderを返す
    Border reversal() const;

    /// ポイントが流入点かどうか
    bool isIn(Connector*) const;

    /// ポイントが流出点かどうか
    bool isOut(Connector*) const;

    /// 流入点の数
    int numIn() const;

    /// 流出点の数
    int numOut() const;

protected:
    /// 幾何的な線分オブジェクト
    /**
     * デストラクタで消去される
     。*/
    const AmuLineSegment* _ptLSegment;

    /// 流入ポイント
    std::vector<const Connector*> _inPoints;

    /// 流出ポイント
    std::vector<const Connector*> _outPoints;

    /// 路側幅
    double _roadsideWidth;

protected:
    //====================================================================
    /** @name 作成に関する関数 */
    //@{

    /// コネクタを作成する関数
    /**
     * コンストラクタから呼ばれる
     * 
     * @attention すべてのコネクタは等間隔に配置される
     */
    virtual void _createConnectors();

    // @p d0 : @p d1 の内分点にコネクタを作成する
    const Connector* _createInnerConnector(double d0, double d1);

    //@}
};

#endif //__BORDER_H__


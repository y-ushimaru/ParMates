/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROADMAP_BUILDER_H__
#define __ROADMAP_BUILDER_H__
#include <string>

class RoadMap;
class Parallel;

/**
 * @addtogroup IO
 * @brief 入出力を扱うモジュール
 */

/// RoadMap作成クラス
/**
 * 現在はシミュレーションの最初でしか用いない
 *
 * @see RoadMap
 * @ingroup IO RoadNetwork Initialization
 */
class RoadMapBuilder
{
public:
    RoadMapBuilder();
    ~RoadMapBuilder();

    /// RoadMapの作成を開始する
    void buildRoadMap();

    //====================================================================
    /** @name 交差点を生成する関数 */
    /// @{

    /// ファイルを読み込んで交差点群を作成する
    bool buildIntersections();

    /// 交差点を生成する
    bool createIntersection(const std::string& fNetwork);

    /// 交差点の接続情報を設定する
    bool setConnection(const std::string& fNetwork);

    /// 交差点の代表点(中心点)の座標を設定する
    bool setPosition(const std::string& fMapPosition);

    /// 交差点の構造情報を設定する
    bool setIntersectionStructInfo(bool readFile);

    /// 交差点の詳細構造を作成する
    bool createIntersectionStructure();

    /// @}

    //====================================================================
    /** @name 単路を生成する関数 */
    /// @[

    /// 単路群を作成する
    bool buildSections();

    /// 単路を生成する
    bool createSection();

    /// 単路の構造情報を設定する
    bool setSectionStructInfo(bool readFile);

    /// 単路の詳細構造を作成する
    bool createSectionStructure();

    /// @}

    //====================================================================
    /** @name ネットワークを生成する関数 */
    /// @{

    /// 交差点と単路をつなぐ
    bool buildNetwork();

    /// サブネットワーク全体を構築する
    bool buildSubnetwork();

    /// レーンの接続関係を構築する
    bool buildLaneConnection();

    /// レーンの交差関係を構築する
    bool buildLaneCollision();

    /// @}

    //======================================================================
    /** @name 信号を生成する関数 */
    /// @{

    /// 信号を作成する
    bool buildSignals();

    /// 全青の信号を作成する
    bool buildSignalsAllBlue();

    /// @}

    //======================================================================
    /// 制限速度をファイルから読み込んで設定する
    bool setSpeedLimit();

    /// 単路の交通規制情報をファイルから読み込んで設定する
    bool setTrafficControlSection();

    /// 単路の選択確率をファイルから読み込んで設定する
    bool setRoutingProbability();

    //===================================================================
    /// 作成したRoadMapを返す
    RoadMap* roadMap();

private:
    /// 現在のRoadMap
    RoadMap* _currentRoadMap;

    //===================================================================
    /** @name RoadMapの生成状況を表す変数 */
    /// @{

    /// 交差点が生成されたか否か
    bool _isIntersectionCreated;

    /// 接続情報が設定されたか否か
    bool _isConnectionSet;

    /// 座標が設定されたか否か
    bool _isPositionSet;

    /// 交差点の構造情報が設定されたか否か
    bool _isIntersectionStructInfoSet;

    /// 交差点の内部構造が作成されたか否か
    bool _isIntersectionStructureCreated;

    /// 単路が生成されたか否か
    bool _isSectionCreated;

    /// 単路の構造情報が設定されたか否か
    bool _isSectionStructInfoSet;

    /// 単路の内部構造が作成されたか否か
    bool _isSectionStructureCreated;

    /// 道路ネットワークが作成されたか否か
    bool _isNetworkCreated;

    /// サブネットワークが作成されたか否か
    bool _isSubnetworkCreated;

    /// レーンの接続関係が構築されたか否か
    bool _isLaneConnectionCreated;

    /// レーンの交差関係が構築されたか否か
    bool _isLaneCollisionCreated;

    ///@}

    //===================================================================
    ///parallel

  public:
      void setParallel(Parallel* parallel);

  protected:

      Parallel* _parallel;
      
};

#endif //__ROADMAP_BUILDER_H__

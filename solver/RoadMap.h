/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROADMAP_H__
#define __ROADMAP_H__

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

class Signal;
class LaneBundle;
class Intersection;
class ODNode;
class Section;
class Lane;
class RoadEntity;
class Vehicle;
class Parallel;

// 型の別名を定義する
typedef std::unordered_map
<std::string, Intersection*> RMAPI;
typedef std::unordered_map
<std::string, Intersection*>::iterator ITRMAPI;
typedef std::unordered_map
<std::string, Intersection*>::const_iterator CITRMAPI;

typedef std::unordered_map
<std::string, Section*> RMAPS;
typedef std::unordered_map
<std::string, Section*>::iterator ITRMAPS;
typedef std::unordered_map
<std::string, Section*>::const_iterator CITRMAPS;

typedef std::unordered_map
<std::string, Signal*> RMAPSI;
typedef std::unordered_map
<std::string, Signal*>::iterator ITRMAPSI;
typedef std::unordered_map
<std::string, Signal*>::const_iterator CITRMAPSI;

/**
 * @addtogroup RoadEnvironment
 * @brief 道路環境を定義するモジュール
 */

/**
 * @addtogroup RoadNetwork
 * @brief 道路ネットワーク
 * @ingroup RoadEnvironment
 */

/// 道路地図を表すクラス
/**
 * @ingroup RoadNetwork
 */
class RoadMap
{
public:
    RoadMap();
    ~RoadMap();

private:
    /// 交差点のメインコンテナ
    RMAPI _intersections;

    /// 単路のメインコンテナ
    RMAPS _sections;

    /// レーン束のサブコンテナ
    /**
     * IntersectionとSectionをまとめて保持する
     */
    std::vector<LaneBundle*> _bundles;

    /// 使用中のレーン束のコンテナ
    /**
     * _usedBundlesの要素に対してのみrenewAgentLineを実行する．
     * renewAgentLineの最後にクリアする．
     */
    std::vector<LaneBundle*> _usedBundles;

    /// 使用中のODノードのコンテナ
    /**
     * _usedODNodesの要素に対してのみdeleteArrivedAgentsを実行する．
     * deleteArrivedAgentsの最後にクリアする．
     */
    std::vector<ODNode*> _usedODNodes;

    /// 信号のメインコンテナ
    RMAPSI _signals;

public:
    //====================================================================
    /** @name 道路構造に関するもの */
    /// @{

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Intersection関係

    /// Intersectionのコンテナを返す
    const RMAPI* intersections() const;

    /// ODノードを返す
    std::vector<ODNode*> odNodes();

    /// 識別番号 @p id の交差点を返す
    Intersection* intersection(const std::string& id) const;

    /// 交差点をコンテナに加える
    void addIntersection(Intersection* ptInter);

    /// 交差点レーンチェック
    bool checkIntersectionLane();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Section関係

    /// Sectionのコンテナを返す
    const RMAPS* sections() const;

    /// 識別番号 @p id の単路を返す
    Section* section(const std::string& id) const;

    /// 単路をコンテナに加える
    void addSection(Section* ptSection);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // レーン束オブジェクト（Intersection/Section)関係

    /// 全てのレーン束オブジェクトを返す
    const std::vector<LaneBundle*>* laneBundles() const;

    /// 使用中のレーン束を追加する
    void addUsedLaneBundle(LaneBundle* laneBundle);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Signal関係

    /// Signalのコンテナを返す
    const RMAPSI* signals() const;

    /// 識別番号 @p id の信号を返す
    Signal* signal(const std::string& id) const;

    /// 信号をコンテナに加える
    void addSignal(Signal* signal);

    /// @}

    //====================================================================
    /** @name エージェントに関するもの */
    /// @{

    /// レーン上のエージェント列を更新する
    void renewAgentLine();

    /// 目的地に到達したエージェントを消去する
    void deleteArrivedAgents();

    /// @}

    //====================================================================
    /// 道路領域の座標値の最大と最小を計算する（地面を描画するために用いる）
    void region(double& xmin, double& xmax, double& ymin, double& ymax) const;

    /// 道路ネットワークデータをファイル出力する
    void writeMapInfo() const;

    /// intersectionの情報を表示する
    void dispIntersections() const;

    //====================================================================
    // parallel

    /// parallel オブジェクトをセットする
    void setParallel(Parallel* parallel);

    /// 交差点にプロセスIDを付与する
    void setProcessId();

    /// エージェントの情報を取得する
    void getVehicleData(Vehicle* vehicle, int opponentId);

    /// 参照用エージェントの情報を取得する
    void getVirtualVehicleData(Vehicle* vehicle, int opponentId);

    /// 送信データ本体を返す
    std::vector<std::vector<double> >* sendData();

    /// 全エージェント数をカウントする
    int countAllWeight();

    /// 各交差点のエージェント数を書き出す
    void weightToInterFactor();

    /// 共有領域での送信データを決定する
    void decideSendDataInSharedSection();

    /// 再分割時の送信データを決定する
    void decideSendDataInRepartitioning();

    /// エージェント列を仮に更新する
    void renewAgentLineTmp();

  private:

    /// parallelオブジェクト
    Parallel* _parallel;

    /// 共有セクションポインタと隣接交差点ポインタのマップ
    std::map<Section*, Intersection*> _sharedSections;

    /// 送信データ本体
    std::vector<std::vector<double> > _sendData;

};

#endif //__ROADMAP_H__

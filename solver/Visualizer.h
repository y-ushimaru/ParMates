/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VISUALIZER_H__
#define __VISUALIZER_H__

#include <string>
#include <GL/glut.h>

class Simulator;
class PFLogger;
class RandomGenerator;

/**
 * @addtogroup Visualization
 * @brief シミュレーションの可視化
 * @ingroup Procedure
 */

/// 可視化クラス
/**
 * AutoGLを用いた可視化とGUI
 *
 * @ingroup Visualization
 */
class Visualizer
{
public:
    Visualizer();
    ~Visualizer();

    /// 可視化を開始する
    static void visualize();

    //******************************************************************
    /**
     * @name シミュレーションの操作に関する関数群
     * VisualzierSimlationController.cppで定義する
     * */
    /// @{
 
public:
    /// シミュレータをセットする
    void setSimulator(Simulator* simulator);

protected:
    /// フラグを更新する
    static void renewFlag();

    /// 一時的な描画フラグをクリアする
    static void clearTempFlag();

    /// シミュレータを1ステップ進める
    static void timeIncrement();

    /// _poseTimeまでシミュレータを動かす
    static void run();

    /// Quitボタンが押されたときの動作
    static void quitButtonCallback();

    /// Resetボタンが押されたときの動作
    static void resetButtonCallback();

    /// AutoIncrementボタンが押されたときの動作
    static void autoIncrementButtonCallback();

    /// SkipRunボタンが押されたときの動作
    static void skipRunButtonCallback();

    /// @}

    //******************************************************************
    /**
     * @name ビューの操作に関する関数群
     * VisualzierViewController.cppで定義する
     * */
    /// @{

protected:
    /// Drawボタンが押されたときの動作
    static void drawButtonCallback();

    /// ビューの制御変数を出力する
    static void printViewingParamsCallback();

    /// ビューの制御変数を設定する
    static void setViewingParamsCallback();

    /// 背景テクスチャを設定する
    static void updateBackgroundTexture();

    /// 背景テクスチャを消去する
    static void clearBackgroundTexture();

    /// @}

    //******************************************************************
    /**
     * @name ユーザのマニュアル操作に関する関数群
     * VisualzierUserManipulation.cppで定義する
     * */
    /// @{
    
protected:
    /// 指定したIDの車両を探す
    static void searchVehicle();

    /// 指定したIDの車両の情報を表示する
    static void showVehicleInfo();

    /// 指定したIDの交差点を探す
    static void searchIntersection();

    /// 指定したIDの交差点の情報を表示する
    static void showIntersectionInfo();

    /// 車両をマニュアルで発生させる
    static void generateVehicleManual();

    /// 指定した始点・終点間の経路を探索する
    static void searchRoute();

    /// searchRouteのログをインクリメントして表示する
    static void incrementSearchRouteLog();

    /// searchRouteのログをデクリメントして表示する
    static void decrementSearchRouteLog();

    /// searchRouteのログ表示をリセットする
    static void resetSearchRouteLog();

    /// searchRouteのログを表示する
    static void showSearchRouteLog();

    /// @}

    //******************************************************************
    /**
     * @name オブジェクトの描画に関する関数群
     * VisualzierDrawer.cppで定義する
     * */
    /// @{

protected:
    /// ビューを再描画する
    static void viewRedrawCallback();

    /// ビューをレンダリングする(オフラインレンダリング用)
    static void viewRenderCallback();

    /// 背景テクスチャを貼り付ける
    static void putTexture();

    /// 地面を描画する
    static void drawGround();

    /// 道路を描画する
    static void drawRoadMap();

    /// ランクが@p rankである経路探索用地図を描画する
    static void drawRoutingNetwork(int rank);

    /// 路側器を描画する
    static void drawRoadsideUnits();

    /// 自動車を描画する
    static void drawVehicles();

#ifdef INCLUDE_PEDESTRIANS
    /// 歩行者を描画する
    static void drawPedestrians();
#endif //INCLUDE_PEDESTRIANS

    /// アナログ時計を描画する
    static void drawAnalogClock();

    /// @}

    //******************************************************************
    /**
     * @name パネルに関する関数群
     * VisualizerPanel.cppで定義する
     */
    /// @{

protected:
    /// パネルの設定
    static void setPanel();

    /// @}

    //******************************************************************
protected:
    /// 乱数生成器
    static RandomGenerator* _rnd;

    /// 可視化の対象となるSimulator
    static Simulator* _sim;

    /// 経路探索結果のログ
    /**
     * Routing Controlで使用する
     */
    static PFLogger* _pfLogger;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 地図領域(交差点座標)に関する変数 */
    /// @{
    static double _xmin;
    static double _xmax;
    static double _ymin;
    static double _ymax;
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name シミュレーション全般に関する変数 */
    /// @{
    
    /// runの目標時刻
    static int _poseTime;
    
    /// 1ステップあたりの時間[msec]
    static int _timeUnitRate;

    /// アイドルイベントが有効か
    static int _idleEventIsOn;

    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 出力項目に関するフラグ */
    /// @{
    
    /// スクリーンキャプチャを保存するか
    static int _withCapturingIsOn;

    /// 時系列詳細データを出力するか
    static int _outputTimelineDIsOn;

    /// 時系列統計データを出力するか
    static int _outputTimelineSIsOn;

    /// 計測機器の詳細データを出力するか
    static int _outputInstrumentDIsOn;

    /// 計測機器の統計データを出力するか
    static int _outputInstrumentSIsOn;
    
    /// エージェント発生データを出力するか
    static int _outputGenCounterIsOn;

    /// 車列最後尾データを出力するか
    static int _outputConvoyIsOn;
    
    //!< エージェントのトリップ情報を出力するか
    static int _outputTripInfoIsOn;
    
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name シミュレーションの描画全般に関する変数 */
    /// @{

    //< 描画1回あたりの時間[msec]
    /**
     * 例えば、_viewingTimeRateに_timeUnitRateの2倍の値を設定すると
     * 2ステップに1回の描画となる
     */
    static int _viewingTimeRate;

    /// 描画をスキップして実行する時間[msec]
    static int _skipRunTimeRate;

    /// 静止画出力する際のファイル番号 
    static int _frameNumber;

    /// @}
   
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// ビューのサイズ
    static double _viewSize;

    /** @name ビューの視線位置 */
    //@{
    static double _viewPositionX;
    static double _viewPositionY;
    static double _viewPositionZ;
    //@}

    /** @name ビューの視線方向 */
    /// @{
    static double _viewDirectionX;
    static double _viewDirectionY;
    static double _viewDirectionZ;
    /// @}

    /** @name ビューの上方向 */
    /// @{
    static double _viewUpVectorX;
    static double _viewUpVectorY;
    static double _viewUpVectorZ;
    /// @}

    /** @name ビューの制御変数設定用変数 */
    /// @{
    static double _svpSize;
    static double _svpPositionX;
    static double _svpPositionY;
    static double _svpPositionZ;
    static double _svpDirectionX;
    static double _svpDirectionY;
    static double _svpDirectionZ;
    static double _svpUpVectorX;
    static double _svpUpVectorY;
    static double _svpUpVectorZ;
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name ビュー全体の表示パラメータ */
    /// @{
    
    /// 簡略化された出力を表示するか 
    static int _simpleMapIsShown;

    /// モノクロで可視化するか
    /**
     * 現在未使用
     */
    static int _monochromeIsOn;

    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 道路の表示パラメータ */
    /// @{
    
    /// 道路を表示するか
    static int _roadNetworkIsShown;

    /// 交差点の識別番号を表示するか
    static int _interIdIsShown;

    /// 内部レーンの識別番号を表示するか
    static int _laneIdIsShown;

    /// 交差点の内部レーンを表示するか
    static int _lanesInterIsShown;

    /// 単路の内部レーンを表示するか
    static int _lanesSectionIsShown;

    /// サブセクションを表示するか
    static int _subsectionShapeIsShown;

    /// サブネットワークを表示するか
    static int _subnetworkIsShown;

    /// サブセクションの識別番号を表示するか
    static int _subsectionIdIsShown;

    /// 路面描画モード
    /**
     * 0: 描画属性
     * 1: 通行権
     */
    static int _surfaceMode;

    /// 信号を表示するか
    static int _signalsIsShown;

    /// 路側器を表示するか
    static int _roadsideUnitsIsShown;

    //!< コネクタID描画モード
    /**
     * 0: Disable
     * 1: Global ID
     * 2: Local ID
     */
    static int _connectorIdMode;
    
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 経路探索用(表示も含む)パラメータ */
    /// @{
    
    /// 経路探索用ネットワークを表示するか
    static int _routingNetworkIsShown;
    
    /// 表示する経路探索用ネットワークのランク
    static int _routingNetworkRank;

    /// 経路探索用ノードの属性表示モード
    /**
     * 0: Disable
     * 1: ID
     * 2: Rank
     */
    static int _routingNodePropMode;

    /// 経路探索用リンクの属性表示モード
    /**
     * 0: Disable
     * 1: Length
     * 2: Time
     * 3: Straight
     * 4: Left
     * 5: Right
     * 6: Width
     */
    static int _routingLinkPropMode;

    /// 経路探索の始点
    static char _routingFromIntersection[16];

    /// 経路探索の始点の次の点
    static char _routingNextIntersection[16];

    /// 経路探索の終点
    static char _routingGoalIntersection[16];

    /// 経由地
    static char _routingStopPoints[512];

    /// 選好するネットワークランク
    static char _routingPrefRank[16];

    /// 経路探索のログを可視化するか
    static int _routingLogIsShown;

    /// 可視化するログのサイズ
    static unsigned long _routingLogCount;
    
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name エージェントの表示パラメータ */
    /// @{

    /// 車両の識別番号を表示するか
    static int _vehicleIdIsShown;

    /// 車両色モード
    /**
     * 0: VehicleFamily
     * 1: AverageVelocity
     * 2: Hybrid
     */
    static int _vehicleColorMode;
    
    /// 速度を表す色の表示方法
    /**
     * 0: Gradation
     * 1: Binary
     */
    static int _velocityColorMode;

    /// 希望速度に対する走行速度の閾値
    static double _velocityRateThreshold;
    
#ifdef INCLUDE_PEDESTRIANS
    /// 歩行者の識別番号を表示するか
    static int _pedestrianIdIsShown;
#endif //INCLUDE_PEDESTRIANS
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name エージェントと交差点の検索に使用する変数 */
    /// @{
   
    /// 出力する車両のID
    static char _infoVehicleId[16];

    /// 出力する交差点のID
    static char _infoIntersectionId[16];
    
    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name エージェント発生用のパラメータ */
    /// @{
    
    /// 車種ID
    static int  _genVehicleType;
    
    /// 出発地ID
    static char _genVehicleStart[16];
    
    /// 目的地ID
    static char _genVehicleGoal[16];

    /// 経路選択用パラメータ
    static char _genVehicleParams[256];

    /// 経路探索で選好するネットワークランク
    static char _genVehiclePrefRank[16];

    /// 経由地
    static char _genVehicleStopPoints[512];

    /// @}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** @name 背景画像の表示用変数 */
    /// @{
    static double _bgXmin;
    static double _bgXmax;
    static double _bgYmin;
    static double _bgYmax;

    /// 背景画像ファイル名
    static char   _bgFileName[64];

    /// テクスチャID
    static GLuint _texture;

    /// 背景画像を表示するか
    static bool   _textureIsShown;

    /// グレースケールで表示するか
    static int    _bgGrayscaleIsOn;
    
    /// 背景画像の著作権表記
    static char   _bgCreditString[64];

    /// @}
};

#endif //__VISUALIZER_H__

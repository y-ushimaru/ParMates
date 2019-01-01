/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __CONF_H__
#define __CONF_H__

/** @name 円周率の定義 */
//@{
#ifndef M_PI
#define M_PI           3.14159265358979323846  //< pi
#endif
#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  //< pi/2
#endif
#ifndef M_PI_4
#define M_PI_4         0.78539816339744830962  //< pi/4
#endif
#ifndef M_1_PI
#define M_1_PI         0.31830988618379067154  //< 1/pi
#endif
#ifndef M_2_PI
#define M_2_PI         0.63661977236758134308  //< 2/pi
#endif
//@}

/** @name 識別番号の桁数の設定 */
//@{

/// 交差点
#define NUM_FIGURE_FOR_INTERSECTION     6

/// 単路識別番号
#define NUM_FIGURE_FOR_SECTION          (NUM_FIGURE_FOR_INTERSECTION * 2)

/// サブセクション
#define NUM_FIGURE_FOR_SUBSECTION       2

/// レーン
#define NUM_FIGURE_FOR_LANE             8

/// ODノードグループ
#define NUM_FIGURE_FOR_OD_GROUP         6

/// 経路探索用ネットワーク階層
#define NUM_FIGURE_FOR_ROUTING_LAYER    2

/// 経路探索用ノード
#define NUM_FIGURE_FOR_ROUTING_NODE    (NUM_FIGURE_FOR_ROUTING_LAYER + NUM_FIGURE_FOR_SECTION)

/// 経路探索用リンク
#define NUM_FIGURE_FOR_ROUTING_LINK    (NUM_FIGURE_FOR_ROUTING_LAYER + NUM_FIGURE_FOR_SECTION)

/// コネクタのグローバル識別番号表示時の桁数
/**
 * @note
 * 実際の識別番号は生成されるたびに，
 * (コネクタ総数+1)で与えられる
 */
#define NUM_FIGURE_FOR_CONNECTOR_GLOBAL 2

/// コネクタのローカル識別番号の桁数
#define NUM_FIGURE_FOR_CONNECTOR_LOCAL  4

/// 車両の識別番号の桁数
#define NUM_FIGURE_FOR_VEHICLE          6

/// 感知器の桁数
#define NUM_FIGURE_FOR_DETECTORUNIT     6

#ifdef INCLUDE_PEDESTRIANS

/// 歩行者の識別番号の桁数
#define NUM_FIGURE_FOR_PEDESTRIAN       8

#endif // INCLUDE_PEDESTRIANS

/// 時刻を描画するときの桁数
#define NUM_FIGURE_FOR_DRAW_TIME        6

//@}

/** @name 道路ネットワーク */
//@{

/// 横のレーンを探索するときの線分の長さ[m]
#define SEARCH_SIDE_LANE_LINE_LENGTH 5

//@}

/** @name 信号制御 */
//@{

/// スプリットの最大数
#define NUM_MAX_SPLIT 20

//@}

/** @name エージェントに関する定数 */
//@{

/// 経路探索パラメータの数
#define VEHICLE_ROUTING_PARAMETER_SIZE  5

/// 経路探索パラメータの意味
#define VEHICLE_ROUTING_PARAMETER_OF_LENGTH   0
#define VEHICLE_ROUTING_PARAMETER_OF_TIME     1
#define VEHICLE_ROUTING_PARAMETER_OF_STRAIGHT 2
#define VEHICLE_ROUTING_PARAMETER_OF_LEFT     3
#define VEHICLE_ROUTING_PARAMETER_OF_RIGHT    4
#define VEHICLE_ROUTING_PARAMETER_OF_WIDTH    5 //削除

/// 旅行時間算出用の車両数
#define VEHICLE_PASS_TIME_INTERSECTION 10

//@}

/** @name 別名定義 */
//@{

/// 符号なしlong
/*
 * 32bit OSであっても範囲は[0, 4294967295]であり9桁を保証．
 */
typedef unsigned long ulint;

//@}

#endif //__CONF_H__

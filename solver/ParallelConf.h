/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PARALLEL_CONF_H__
#define __PARALLEL_CONF_H__

// データの送受信時の識別子
#define REAL_VEHICLE -10
#define VIRTUAL_VEHICLE -20

// ODノードの重み
#define ODNODE_WEIGHT 0

// 負荷を通信する間隔
#define SEND_LOAD_INTERVAL 100000

// 計算時間を計測する間隔
#define MEASURE_INTERVAL 5000

// 計測開始時間
#define START_MEASURE_TIME 3600000

// 計測終了時間
#define END_MEASURE_TIME 7200000

// 再分割を始める時間
#define START_REPARTITIONING_TIME 900000

// 再分割を終える時間
#define END_REPARTITIONING_TIME 72000000

// 動的負荷分散を行うか
#define DYNAMIC_LOAD_BALANCING

// エージェント数による重み付け
#define WEIGHT_BY_AGENTS

// 計算時間による重み付け
//#define WEIGHT_BY_CALCTIME

// 初期分割を指定するか
#define INIT_NETWORK_GRAPH

// 正確な現在時刻を取得
double getTime();

#endif //__PARALLEL_CONF_H__

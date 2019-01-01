/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATE_VEHICLE_COUNTER_IO_H__
#define __GENERATE_VEHICLE_COUNTER_IO_H__

#include "ODNode.h"

class RoadMap;

/// 発生した車両の情報を出力するクラス
/**
 * インスタンスを生成しない
 *
 * @note 計測の処理はODNodeが担当する
 * @ingroup IO Monitoring
 */
class GenerateVehicleCounterIO
{
private:
    GenerateVehicleCounterIO(){};
    ~GenerateVehicleCounterIO(){};

public:
    /// ファイルで指定されたODノードの出力ファイルを作成する
    static void getReadyCounters(RoadMap* roadMap);

    /// 発生した車両の情報をファイル出力する
    static void writeGeneratedVehicleData
    (ODNode* node, std::vector<ODNode::GeneratedVehicleData>* gvd);

    /// 情報を画面に出力する
    static void print(std::vector<ODNode*>* nodes);
};

#endif //__GENERATE_VEHICLE_COUNTER_IO_H__

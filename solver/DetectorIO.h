/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __DETECTORIO_H__
#define __DETECTORIO_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class RoadMap;
class DetectorUnit;

/// 車両感知器のファイル入出力を行うクラス
/**
 * インスタンスを作成しない
 *
 * @ingroup IO Monitoring
 */
class DetectorIO
{
private:
    DetectorIO(){};
    ~DetectorIO(){};

public:
    /// ファイルを読み込みDetectorUnitを生成する
    static void getReadyDetectors(RoadMap* roadMap);

    /// 出力ファイルを作成する
    static void getReadyOutputFiles(std::vector<DetectorUnit*>* units);

    /// 観測された情報をファイル出力する
    static void writeTrafficData(std::vector<DetectorUnit*>* units);

    /// 統計情報をファイル出力する
    static void writeStatData(DetectorUnit* unit);

    /// 情報の表示
    static void print();
};

#endif //__DETECTORIO_H__

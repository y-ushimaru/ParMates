/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNALIO_H__
#define __SIGNALIO_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "Signal.h"
#include "SignalAspect.h"
#include "SignalControlDataSet.h"
#include "SignalControlData.h"
#include "RoadMap.h"

#ifdef USE_ZLIB
#include <zlib.h>
#else //USE_ZLIB
#include <cstdio>
#endif //USE_ZLIB

class RoadMap;

/// 信号制御情報のファイル入力を行うクラス
/**
 * @sa SignalControlDataSet
 * @ingroup IO Signal
 */
class SignalIO
{
private:
    SignalIO(){};
    ~SignalIO(){};

public:
    /// 唯一のインスタンスを返す
    static SignalIO& instance();

    /** @name 入力に関するもの */
    /// @{

    /// 現示データを返す
    std::vector<SignalAspect> aspect(const std::string& id, const int sideNum) const;

    /// 全青用現示データを返す
    std::vector<SignalAspect> aspectBlue(const std::string& id, const int sideNum) const;

    /// 信号スプリットデータを返す
    SignalControlDataSet signalControlDataSet(const std::string& id) const;

    /// 全青用スプリットデータを返す
    SignalControlDataSet signalControlDataSetBlue(const std::string& id) const;

    /// @}

    /** @name 出力に関するもの */ 
    /// @{

    /// 信号の動的データ(タイムステップごとの信号現示)を出力する
    bool writeSignalsDynamicData(const ulint time, const RMAPSI* signals);

private:
    /// 動的データ出力の本体
    bool _writeSignalDynamicData(Signal* signal);

    /// ディレクトリを階層的に作成する
    /**
     * @p pathsに格納された名前のディレクトリを順に作る
     * ./paths[0]/paths[1]/paths[2]/...
     */
    bool _makeDirectories(std::vector<std::string> paths) const;

    /// @}
private:
    /// 動的データファイルのPrefix
    static const std::string _timePrefix;

    /// 動的データの出力ファイル
    /*
     * zlibを使用する場合はgzFile,
     * 使用しない場合はFile*に置き換わる
     */
#ifdef USE_ZLIB
    gzFile _dynamicOut;
#else //USE_ZLIB
    FILE* _dynamicOut;
#endif //USE_ZLIB

public:
    /// RoadMapをセット
    void setRoadMap(RoadMap* roadMap);
	
private:
    /// RoadMap
    RoadMap* _roadMap;

protected:
};

#endif //__SIGNALIO_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __CONVOY_MONITOR_IO_H__
#define __CONVOY_MONITOR_IO_H__

class RoadMap;
class Section;

/// 単路上の停車車両の最後尾を出力するクラス
class ConvoyMonitorIO
{
private:
    ConvoyMonitorIO(){};
    ~ConvoyMonitorIO(){};

public:
    /// ファイルで指定されたSectionの出力ファイルを作成する
    static void getReadyMonitors(RoadMap* roadMap);

    /// 最後尾情報をファイル出力する
    static void writeConvoyData(RoadMap* roadMap);

    /// セクションごとの最後尾情報をファイル出力する
    static void writeConvoyData(Section* section,
                                bool isUp);
};

#endif //__CONVOY_MONITOR_IO_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATE_PEDESTRIAN_IO_H__
#define __GENERATE_PEDESTRIAN_IO_H__

class RoadMap;

class GeneratePedestrianIO
{
private:
    GeneratePedestrianIO(){};
    ~GeneratePedestrianIO(){};

public:
    /// 唯一のインスタンスを返す
    static GeneratePedestrianIO& instance();

    /// RoadMapを設定する
    void setRoadMap(RoadMap* roadMap);

    /// 歩行者の発生交通量を読み込む
    bool readPedGenVolume();

private:
    /// 地図
    RoadMap* _roadMap;

};

#endif //__GENERATE_PEDESTRIAN_IO_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATE_PEDESTRIAN_CONTROLLER_H__
#define __GENERATE_PEDESTRIAN_CONTROLLER_H__

#include "../Conf.h"

class RoadMap;
class Intersection;
class Zebra;
class Pedestrian;
class RandomGenerator;

/// 歩行者の発生を制御するクラス
/**
 * @ingroup Running
 */
class GeneratePedestrianController
{
public:
    /// 唯一のインスタンスを返す
    static GeneratePedestrianController& instance();

    /// RoadMapをセットする
    void setRoadMap(RoadMap* roadMap);

    /// 歩行者発生に関する初期設定を行う
    /**
     * @return 設定に成功したかどうか
     */
    bool getReadyGeneration();

    //======================================================================
    /** @name エージェントの発生 */
    //@{

    /// 歩行者を発生させる
    void generatePedestrian();

    /// 発生歩行者データを格納する構造体
    struct GeneratedPedestrianData
    {
        Pedestrian* pedestrian;
        const Intersection* intersection;
        const Zebra* zebra;
        ulint startTime;
    };

    /// 発生歩行者データ出力フラグを設定する
    void setOutputGeneratePedestrianDataFlag(bool flag);

private:
    GeneratePedestrianController();
    ~GeneratePedestrianController();

    bool canGeneratePedestrian(Zebra* zebra, int dir);

    /// 地図オブジェクト
    RoadMap* _roadMap;

    /// 発生歩行者データ
    //std::vector<GeneratedPedestrianData> _nodeGvd;

    /// 発生車両データを出力するかどうか
    bool _isOutputGeneratePedestrianData;

    /// 乱数生成器
    RandomGenerator* _rnd;
};

#endif //__GENERATE_PEDESTRIAN_CONTROLLER_H__

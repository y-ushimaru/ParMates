/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIMULATOR_PED_EXT_H__
#define __SIMULATOR_PED_EXT_H__

class Simulator;
class GeneratePedestrianController;

/// シミュレータの歩行者用機能拡張クラス
/**
 * @ingroup Pedestrian
 */
class SimulatorPedExt
{
public:
    SimulatorPedExt(){};
    SimulatorPedExt(Simulator* simulator);
    ~SimulatorPedExt(){};

    /// Pedestrianに関する初期設定を行う
    /**
     * @return 設定に成功したかどうか
     */
    bool getReadyPedestrians();

    /// 歩行者を発生させる
    void generatePedestrian();

private:
    /// 対応するSimulator
    Simulator* _simulator;

    /// 歩行者発生を担当するコントローラ
    GeneratePedestrianController* _genPedestrianController;
};

#endif //__SIMULATOR_PED_EXT_H__

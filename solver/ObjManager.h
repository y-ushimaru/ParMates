/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __OBJ_MANAGER_H__
#define __OBJ_MANAGER_H__

#include <vector>
#include <deque>
#include <string>

class Connector;
class DetectorUnit;
class Vehicle;

#ifdef INCLUDE_TRAMS
class VehicleTram;
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
class Pedestrian;
#endif //INCLUDE_PEDESTRIANS

/// エージェント・オブジェクトの生成・消去および識別番号を管理するクラス
/**
 * @attention シミュレーション終了時に全てのエージェント・オブジェクトを破棄すること
 * @note Managerは静的クラスとする
 * @ingroup Manager
 */
class ObjManager
{
private:
    /// コネクタのメインコンテナ
    static std::vector<Connector*> _connectors;

    /// 車両感知器ユニットのメインコンテナ
    static std::vector<DetectorUnit*> _detectorUnits;

    /// 道路地図上に存在する車のメインコンテナ
    static std::vector<Vehicle*> _vehicles;

    /// 車両識別番号用カウンタ
    /**
     * 識別番号は生成された順につける
     * 消去される車両もあるので、_vehicles.size()とは異なる
     */
    static int numVehicle;


#ifdef INCLUDE_PEDESTRIANS
    /// 道路地図上に存在する歩行者のメインコンテナ
    static std::vector<Pedestrian*> _pedestrians;

    /// 歩行者識別番号用カウンタ
    /**
     * 識別番号は生成された順につける
     * 消去される歩行者もあるので、_pedestrians.size()とは異なる
     */
    static int numPedestrian;

#endif // INCLUDE_PEDESTRIANS

    /*
     *  コネクタが途中で消去されることはないのでnumConnectorは必要ない
     */

public:
    //====================================================================
    /** @name オブジェクト全般に関する動作 */
    //@{

    /// 保持するオブジェクト全てを消去する
    /**
     * @note シミュレーションを終えるときに用いる
     */
    static void deleteAll();

    /// コネクタ以外のオブジェクトを消去する
    /**
     * @note シミュレーションをリセットするときに用いる
     */
    static void deleteAllAgents();

    //@}

    //====================================================================
    /** @name コネクタに関する操作 */
    //@{

    /// 保持する全コネクタを返す
    /**
     * @return コネクタのvectorへのポインタ
     */
    static std::vector<Connector*>* connectors();

    /// x座標、y座標、z座標からコネクタを生成する
    /**
     * @param x 生成するコネクタのx座標
     * @param y 生成するコネクタのy座標
     * @param z 生成するコネクタのz座標
     */
    static Connector* createConnector(double x, double y, double z);

    //@}

    //====================================================================
    /** @name 路側器に関する操作 */
    //// @{

    /// 保持する全感知器を返す
    /**
     * @return 感知器のvectorへのポインタ
     */
    static std::vector<DetectorUnit*>* detectorUnits();

    /// 指定された識別番号を持つ感知器を返す
    /**
     * @param id 感知器の識別番号
     * @return 該当する感知器へのポインタ
     */
    static DetectorUnit* detectorUnit(const std::string& id);

    /// 作成された感知器をコンテナに追加する
    /**
     * @note DetectorUnitをnewするのはDetectorIO::readDetectorFileの内部
     *
     * @param unit 追加する感知器へのポインタ
     * @return 正常に追加されたか否か
     *
     * @see DetectorIO
     */
    static bool addDetectorUnit(DetectorUnit* unit);

    /// @}

    //====================================================================
    /** @name 車両に関する操作 */
    /// @{

    /// 保持する車両を返す
    /**
     * @return 車両vectorへのポインタ
     */
    static std::vector<Vehicle*>* vehicles();

    /// 登場直後の車両を返す
    /**
     * @return 登場直後車両vectorへのポインタ
     */
    static std::vector<Vehicle*>* justAddedVehicles();

    /// 登場直後の車両をクリアする
    static void clearJustAddedVehicles();

    /// 指定された識別番号を持つ車両を返す
    /**
     * @param id 車両の識別番号
     * @return 該当する車両へのポインタ
     */
    static Vehicle* vehicle(const std::string& id);

    /// 車両（普通車と大型車）を生成する
    /**
     * @note この時点ではまだ識別番号を付与しない
     *
     * @return 生成した車両
     */
    static Vehicle* createVehicle();

    /// シミュレーションに登場したエージェント数の合計を出力する
    static void printNumVehicle();

#ifdef INCLUDE_TRAMS
    /// 路面電車を生成する
    static VehicleTram* createVehicleTram();

#endif //INCLUDE_TRAMS

    /// 車両をシミュレータ上に登場させる
    /**
     * @note 車の識別番号はこの時付けられる
     *
     * @param vehicle 登場させる車両
     */
    static bool addVehicleToReal(Vehicle* vehicle);

    /// 全ての車両を消去する
    static void deleteAllVehicles();

    /// 指定された車両だけを消去する
    /**
     * @param vehicle 消去したい車両へのポインタ
     */
    static void deleteVehicle(Vehicle* vehicle);

    /// @}

#ifdef INCLUDE_PEDESTRIANS
    //====================================================================
    /** @name 歩行者に関する操作 */
    /// @{

    /// 保持する歩行者を返す
    /**
     * @return 歩行者のvectorへのポインタ
     */
    static std::vector<Pedestrian*>* pedestrians();

    /// 指定された識別番号を持つ歩行者を返す
    /**
     * @param id 歩行者の識別番号
     * @return 該当する歩行者へのポインタ
     */
    static Pedestrian* pedestrian(const std::string& id);

    /// 歩行者を生成する
    /**
     * @note Simulatorから呼ばれる
     * @note この時点ではまだ識別番号を付与しない
     *
     * @return 生成した歩行者
     */
    static Pedestrian* createPedestrian();

    /// 歩行者をシミュレータ上に登場させる
    /**
     * @note 歩行者の識別番号はこの時付けられる
     *
     * @param vehicle 登場させる歩行者
     */
    static bool addPedestrianToReal(Pedestrian *pedestrian);

    /// 全ての歩行者を消去する
    static void deleteAllPedestrians();

    /// 渡り終えた歩行者のみ消去する。
    static void deleteFinishedPedestrians();

    /// 指定された歩行者だけを消去する
    static void deletePedestrian(Pedestrian* pedestrian);

    /// @}

#endif //INCLUDE_PEDESTRIAN
};

#endif //__OBJ_MANAGER_H__

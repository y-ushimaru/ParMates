/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VEHICLE_FAMILY_MANAGER_H__
#define __VEHICLE_FAMILY_MANAGER_H__

#include <map>

//######################################################################
/// 車種ごとの属性を集約するクラス
/**
 * @ingroup Vehicle
 */
class VFAttribute
{
public:
    VFAttribute();
    VFAttribute(int id,
                double bodyLength,
                double bodyWidth,
                double bodyHeight,
                int    bodyArticulation,
                double bodyWeight,
                double maxAcceleration,
                double maxDeceleration,
                double bodyColorR,
                double bodyColorG,
                double bodyColorB);
    ~VFAttribute();


    /** @name アクセッサ */
    //@{

    /// 車種を返す
    int id() const;

    /// 車長を返す
    double bodyLength() const;

    /// 車幅を返す
    double bodyWidth() const;

    /// 車高を返す
    double bodyHeight() const;

    /// サイズをまとめて返す
    void getSize(double *result_l,
                 double *result_w,
                 double *result_h) const;

    /// 連接数を返す
    int bodyArticulation() const;
    
    /// 最大加速度を返す
    double maxAcceleration() const;

    /// 最大限速度を返す
    double maxDeceleration() const;

    /// 性能をまとめて返す
    void getPerformance(double *result_a,
                        double *result_d) const;

    /// 車体色をまとめて返す
    void getBodyColor(double *result_r,
                      double *result_g,
                      double *result_b) const;

    /// @}

private:
    /// 車種ID
    int _id;

    /// 車長(m)
    double _bodyLength;

    /// 車幅(m)
    double _bodyWidth;

    /// 車高(m)
    double _bodyHeight;

    /// 連接数(通常は1)
    int _bodyArticulation;
    
    /// 車重(kg): 未使用
    double _bodyWeight;

    /// 最大加速度(m/(sec^2))
    double _maxAcceleration;

    /// 最大減速度(m/(sec^2))
    double _maxDeceleration;

    /// 車体色の赤成分[0,1]
    double _bodyColorR;

    /// 車体色の緑成分[0,1]
    double _bodyColorG;

    /// 車体色の青成分[0,1]
    double _bodyColorB;
};

//######################################################################
/// 車種を管理するクラス
/**
 * @note Managerは静的クラスとする
 * 
 * @ingroup Manager Vehicle
 */
class VehicleFamilyManager
{
private:
    /// 車種のメインコンテナ
    static std::map<int, VFAttribute, std::less<int> > _families;

public:
    /// 車種を追加する
    static void addVehicleFamily(VFAttribute vehicleFamily);

    /// 車種のコンテナを返す
    static std::map<int, VFAttribute, std::less<int> >* families();

    /// 指定した車種のポインタを返す
    static VFAttribute* vehicleFamilyAttribute(int id);

private:
    VehicleFamilyManager(){};
    ~VehicleFamilyManager(){};
};

#endif //__VEHICLE_FAMILY_MANAGER_H__

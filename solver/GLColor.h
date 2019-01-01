/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GL_COLOR_H__
#define __GL_COLOR_H__

/// autogl に描画をさせる時の色の管理を行うクラス
/**
 * このクラスはインスタンスを作成しない
 *
 * @ingroup Drawing
 */
class GLColor
{
public:

    //==================================================================
    // 背景、地面など全般的なもの
    static void setBackground();
    static void setGround();
    static void setSimpleNetwork();

    //==================================================================
    // 道路構造

    // サブセクション
    // 道路表面の描画色
    static void setSubsection();
    static void setCrossWalk();
    static void setSideWalk();

    // 通行権による色分け
    static void setVehicleAccessible();
    static void setPedestrianAccessible();
    static void setAnyTrafficAccessible();
    static void setAnyTrafficInaccessible();

    // サブネットワーク
    static void setSubnetwork();
    static void setSubsectionEdge();
    static void setWalkerGateway();

    // レーン
    static void setLane();
    static void setStraightLane();
    static void setLeftLane();
    static void setRightLane();
    static void setUpLane();
    static void setDownLane();

    static void setTramLane();

    // ボーダー、コネクタ他
    static void setBorder();
    static void setInPoint();
    static void setOutPoint();
    static void setDetector();
    static void setInterId();
    static void setLaneId();
    static void setSubsectionId();
    static void setRoadsideUnitId();

    //======================================================================
    // 経路探索用地図
    static void setRoutingLink();
    static void setRoutingLinkString();
    static void setRoutingNodeString();
    static void setPFLog();
    static void setPFLog(unsigned int rank);
    static void setPFLastLog();

    //==================================================================
    // 信号
    static void setBlueSignal();
    static void setRedSignal();
    static void setYellowSignal();
    static void setNoneSignal();
    static void setAllSignal();
    static void setStraightSignal();
    static void setLeftSignal();
    static void setRightSignal();
    static void setStraightLeftSignal();
    static void setStraightRightSignal();
    static void setLeftRightSignal();

    //====================================================================
    // エージェント
    static void setVehicle();
    static void setSleepingVehicle();
    static void setTruck();
    static void setBus();
    static void setTram();
    static void setVehicleId();

    // 歩行者の位置
    static void setPedestrian ();
    // 歩行者の進行方向と速度
    static void setPedestrianArrow ();
    static void setShiftedPedestrianArrow ();
    static void setPedestrianId();

private:
    GLColor();
    ~GLColor();
};

#endif //__GL_COLOR_H__

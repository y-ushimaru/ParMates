/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include "Simulator.h"
#include "PFLogger.h"
#include "RoadMap.h"
#include "RoutingManager.h"
#include "RoutingNetwork.h"
#include "Drawer.h"
#include "GLColor.h"
#include "TimeManager.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "Vehicle.h"
#include "Conf.h"
#include "autogl_mates.h"
#include <autogl.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <deque>

/* OpenCV用 */
#ifdef USE_OPENCV
#include <cv.h>
#include <highgui.h>
#endif //USE_OPENCV

#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/DrawerPedExt.h"
#endif //INCLUDE_PEDESTRIANS

using namespace std;

//======================================================================
void Visualizer::viewRedrawCallback()
{
    // テクスチャの貼り付け
    if (_textureIsShown)
    {
        putTexture();
    }

    // 地面の描画
    if (!(GVManager::getFlag("VIS_SIMPLE_MAP"))
        && _subsectionShapeIsShown
        && !_textureIsShown)
    {
        drawGround();
    }

    // 地図の描画
    if (_roadNetworkIsShown)
    {
        drawRoadMap();
    }

    // 経路探索用地図の描画
    if (_routingNetworkIsShown)
    {
        drawRoutingNetwork(_routingNetworkRank);
    }
    
    // 経路探索ログを表示
    if (_routingLogIsShown
        && !(_pfLogger->isEmpty()))
    {
        RoutingNetworkDrawer* routingNetworkDrawer
            = &RoutingNetworkDrawer::instance();
        routingNetworkDrawer->drawLog(*_pfLogger,
                                      _routingLogCount);
    }

    // 路側器の描画
    if (_roadsideUnitsIsShown)
    {
        drawRoadsideUnits();
    }

    // 自動車の描画
    drawVehicles();

#ifdef INCLUDE_PEDESTRIANS
    // 歩行者の描画
    drawPedestrians();
#endif //INCLUDE_PEDESTRIANS

    // 時刻の描画
    if (GVManager::getFlag("VIS_SHOW_ANALOG_CLOCK"))
    {
        drawAnalogClock();
    }
    else
    {
        AutoGL_SetColor(0,0,0);
        ulint presentTime = TimeManager::time()/1000;
        string timeString
            = AmuConverter::itos(presentTime,
                                 NUM_FIGURE_FOR_DRAW_TIME)
            + "[sec]";
        AutoGL_DrawStringAtScaledPosition(0.01, 0.97,
                                          timeString.c_str());
    }

    // 著作権表記
    if (_textureIsShown
        && strcmp(_bgCreditString, "")!=0)
    {
        AutoGL_SetColor(0, 0, 0);
        AutoGL_DrawStringAtScaledPosition(0.01, 0.01,
                                          _bgCreditString);
    }
}

//======================================================================
void Visualizer::viewRenderCallback()
{
    // ビューのサイズ（中心から端までの長さ）
    AutoGL_SetViewSize(_viewSize);
    // ビューの中心
    AutoGL_SetViewCenter(_viewPositionX,
                         _viewPositionY,
                         _viewPositionZ);
    // 視線方向
    AutoGL_SetViewDirection(_viewDirectionX,
                            _viewDirectionY,
                            _viewDirectionZ);
    // 画面上方向
    AutoGL_SetViewUpVector(_viewUpVectorX,
                           _viewUpVectorY,
                           _viewUpVectorY);

    // 広域の場合はシンプル地図モードに
    if (_viewSize>1000)
    {
        GVManager::resetFlag("VIS_SIMPLE_MAP", true);
    }

    // シミュレータを停止する時刻の決定
    ulint maxTime = _poseTime;
    ulint mt = GVManager::getMaxTime();
    if (mt>100)
    {
        maxTime = mt;
    }

    string imgDir = "/tmp/mates_images/";
    GVManager::getVariable("RESULT_IMG_DIRECTORY", &imgDir);

    cout << "*** Run advmates-sim (off-screen rendering): max time="
         << maxTime
         << " (dt=" << TimeManager::unit() << ") ***" << endl;

    // シミュレータを進める
    while (TimeManager::time()<maxTime)
    {
        _sim->run(TimeManager::time() + _viewingTimeRate);

        // ビューをファイルに出力する
        string fileName = imgDir;
        fileName += AmuConverter::itos(_frameNumber, 6);
        fileName += ".ppm";
        _frameNumber++;
        AutoGL_SetImageFileName(fileName.c_str());
        AutoGL_DrawView();
    }
}

//======================================================================
void Visualizer::putTexture()
{
    AutoGL_BeginNativeCall();
    {
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        
        glTexCoord2d(0.0, 1.0);
        glVertex3d(_bgXmin, _bgYmax, -10.0);
        
        glTexCoord2d(0.0, 0.0);
        glVertex3d(_bgXmin, _bgYmin, -10.0);
        
        glTexCoord2d(1.0, 0.0);
        glVertex3d(_bgXmax, _bgYmin, -10.0);
        
        glTexCoord2d(1,1);
        glVertex3d(_bgXmax, _bgYmax, -10.0);
        
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    AutoGL_EndNativeCall();
}

//======================================================================
void Visualizer::drawGround()
{
    GLColor::setGround();
    AutoGL_DrawQuadrangle(_xmin, _ymin, -2.0,
                          _xmax, _ymin, -2.0,
                          _xmax, _ymax, -2.0,
                          _xmin, _ymax, -2.0);
}

//======================================================================
void Visualizer::drawRoadMap()
{
    RoadMap* roadMap = _sim->roadMap();
    if (roadMap)
    {
        RoadMapDrawer& roadMapDrawer = RoadMapDrawer::instance();
        roadMapDrawer.draw(*roadMap);
    }
}

//======================================================================
void Visualizer::drawRoutingNetwork(int rank)
{
    RoutingManager* routingManager = _sim->routingManager();

    if (rank>=0)
    {
        const RoutingNetwork* routingNetwork
            = routingManager->routingNetwork(rank);
        if (!routingNetwork)
        {
            return;
        }

        RoutingNetworkDrawer& routingNetworkDrawer
            = RoutingNetworkDrawer::instance();
        routingNetworkDrawer.draw(*routingNetwork);
    }
    else
    {
        for (int i=0; i<10; i++)
        {
            const RoutingNetwork* routingNetwork
                = routingManager->routingNetwork(i);
            if (!routingNetwork)
            {
                continue;
            }

            RoutingNetworkDrawer& routingNetworkDrawer
                = RoutingNetworkDrawer::instance();
            routingNetworkDrawer.draw(*routingNetwork);            
        }
    }
}

//======================================================================
void Visualizer::drawRoadsideUnits()
{
    DetectorDrawer* detectorDrawer = &DetectorDrawer::instance();
    vector<DetectorUnit*>* detectorUnits = ObjManager::detectorUnits();

    for (unsigned int i=0; i<detectorUnits->size(); i++)
    {
        detectorDrawer->draw(*(*detectorUnits)[i]);
    }
}

//======================================================================
void Visualizer::drawVehicles()
{
    VehicleDrawer* vehicleDrawer = &VehicleDrawer::instance();
    vector<Vehicle*>* allVehicles = ObjManager::vehicles();

    // drawSimple用の図形サイズを決定
    double size = 0.02*AutoGL_GetViewSize();

    for (unsigned int i=0; i<allVehicles->size(); i++)
    {
        if ((*allVehicles)[i]->isAwayFromOriginNode())
        {
            if (GVManager::getFlag("VIS_SIMPLE_MAP"))
            {
                vehicleDrawer->drawSimple(*(*allVehicles)[i], size);
            }
            else
            {
                vehicleDrawer->draw(*(*allVehicles)[i]);
            }
        }
    }
}

//======================================================================
#ifdef INCLUDE_PEDESTRIANS
void Visualizer::drawPedestrians()
{
    PedestrianDrawer* pedDrawer = &PedestrianDrawer::instance();
    vector<Pedestrian*>* allPedestrians = ObjManager::pedestrians();

    // drawSimple用の図形サイズを決定
    // double size = 0.01*AutoGL_GetViewSize();

    for (unsigned int i=0; i<allPedestrians->size(); i++)
    {
        if (GVManager::getFlag("VIS_SIMPLE_MAP"))
        {
            // pedDrawer->drawSimple(*(*allPedestrians)[i], size);
        }
        else
        {
            pedDrawer->draw(*(*allPedestrians)[i]);
        }
    }
}
#endif //INCLUDE_PEDESTRIANS

//======================================================================
void Visualizer::drawAnalogClock()
{
    // 表示時刻の決定
    double startHour
        = GVManager::getNumeric("VIS_CLOCK_START_HOUR");
    double startMinute
        = GVManager::getNumeric("VIS_CLOCK_START_MINUTE");

    ulint presentTime   = TimeManager::time()/(1000*60);
    ulint presentMinute = presentTime%60;
    ulint presentHour   = ((presentTime-presentMinute)/60)%24;
    ulint offset = 0;

    presentMinute += static_cast<unsigned int>(startMinute);
    if (presentMinute >= 60)
    {
        presentMinute -= 60;
        offset = 1;
    }
    presentHour
        = (static_cast<unsigned int>(startHour) 
           + presentHour + offset)%24;

    bool isPm = false;
    if (presentHour >= 12)
    {
        isPm = true;
        presentHour -= 12;
    }

    // 時計サイズ
    double clockSize = 0.1;

    // 時計の円の中心点(centerX, centerY)を求める
    int widthDc, heightDc;            // デバイス座標
    int centerXDc, centerYDc;         // デバイス座標
    double centerX, centerY, centerZ; // 世界座標

    AutoGL_GetViewRangeDc(&widthDc, &heightDc);
    centerXDc = (-widthDc  / 2) * (1-clockSize*1.2);
    centerYDc = (-heightDc / 2) * (1-clockSize*1.2);
    AutoGL_GetPositionOfDc(&centerX, &centerY, &centerZ,
                           centerXDc, centerYDc, 0);
    AmuPoint center(centerX, centerY, centerZ+105);

    double radius = clockSize * AutoGL_GetViewSize();

    // 時計枠
    AutoGL_SetColor(0, 0, 0);
    AutoGL_DrawCircle3D(centerX, centerY, centerZ+99,
                        0, 0, 1, radius*1.1, 12);

    // 時計盤
    if (!isPm)
    {
        AutoGL_SetColor(1, 1, 0.8);
    }
    else
    {
        AutoGL_SetColor(0.8, 1, 1);
    }
    AutoGL_DrawCircle3D(centerX, centerY, centerZ+100,
                        0, 0, 1, radius, 12);

    // 分針
    AutoGL_SetColor(0, 0, 0);
    double minuteX, minuteY;
    minuteX = centerX + radius * sin(presentMinute/30.0 * M_PI);
    minuteY = centerY + radius * cos(presentMinute/30.0 * M_PI);
    AmuPoint minute(minuteX, minuteY, centerZ+105);
    AmuVector mvec
        = AmuVector(minuteX-centerX, minuteY-centerY, 0);
    mvec.normalize();
    mvec.revoltXY(M_PI/2);
    AmuPoint mPoint1
        = 0.2*center + 0.8*minute + radius*0.15*mvec;
    AmuPoint mPoint2
        = 0.2*center + 0.8*minute - radius*0.15*mvec;

    AutoGL_DrawTriangle(center.x() + radius*0.05*mvec.x(),
                        center.y() + radius*0.05*mvec.y(),
                        center.z(),
                        minute.x(),  minute.y(),  minute.z(),
                        mPoint1.x(), mPoint1.y(), mPoint1.z());
    AutoGL_DrawTriangle(center.x() + radius*0.05*mvec.x(),
                        center.y() + radius*0.05*mvec.y(),
                        center.z(),
                        center.x() - radius*0.05*mvec.x(),
                        center.y() - radius*0.05*mvec.y(),
                        center.z(),
                        minute.x(),  minute.y(),  minute.z());  
    AutoGL_DrawTriangle(center.x() - radius*0.05*mvec.x(),
                        center.y() - radius*0.05*mvec.y(),
                        center.z(),
                        mPoint2.x(), mPoint2.y(), mPoint2.z(),
                        minute.x(),  minute.y(),  minute.z());

    // 時針
    AutoGL_SetColor(1, 0, 0);
    double hourX, hourY;
    hourX = centerX + 0.7 * radius
        * sin((presentHour/6.0 + presentMinute/360.0)* M_PI);
    hourY = centerY + 0.7 * radius
        * cos((presentHour/6.0 + presentMinute/360.0)* M_PI);
    AmuPoint hour(hourX, hourY, centerZ+105);
    AmuVector hvec
        = AmuVector(hourX-centerX, hourY-centerY, 0);
    hvec.normalize();
    hvec.revoltXY(M_PI/2);
    AmuPoint hPoint1
        = 0.2*center + 0.8*hour + radius*0.2*hvec;
    AmuPoint hPoint2
        = 0.2*center + 0.8*hour - radius*0.12*hvec;

    AutoGL_DrawTriangle(center.x() + radius*0.05*hvec.x(),
                        center.y() + radius*0.05*hvec.y(),
                        center.z(),
                        hour.x(),    hour.y(),    hour.z(),
                        hPoint1.x(), hPoint1.y(), hPoint1.z());
    AutoGL_DrawTriangle(center.x() + radius*0.05*hvec.x(),
                        center.y() + radius*0.05*hvec.y(),
                        center.z(),
                        center.x() - radius*0.05*hvec.x(),
                        center.y() - radius*0.05*hvec.y(),
                        center.z(),
                        hour.x(),    hour.y(),    hour.z());  
    AutoGL_DrawTriangle(center.x() - radius*0.05*hvec.x(),
                        center.y() - radius*0.05*hvec.y(),
                        center.z(),
                        hPoint2.x(), hPoint2.y(), hPoint2.z(),
                        hour.x(),    hour.y(),    hour.z());
}

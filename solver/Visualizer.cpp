/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include "Simulator.h"
#include "PFLogger.h"
#include "Random.h"
#include "GVManager.h"
#include "GLColor.h"
#include <autogl.h>
#include <cstring>
#include <cassert>

using namespace std;

Simulator* Visualizer::_sim;
PFLogger*  Visualizer::_pfLogger;

RandomGenerator* Visualizer::_rnd;

double Visualizer::_xmin = -100.0;
double Visualizer::_xmax =  100.0;
double Visualizer::_ymin = -100.0;
double Visualizer::_ymax =  100.0;

double Visualizer::_viewSize       = 100;
double Visualizer::_viewPositionX  = 0.0;
double Visualizer::_viewPositionY  = 0.0;
double Visualizer::_viewPositionZ  = 0.0;
double Visualizer::_viewDirectionX = 0.0;
double Visualizer::_viewDirectionY = 0.0;
double Visualizer::_viewDirectionZ = 1.0;
double Visualizer::_viewUpVectorX  = 0.0;
double Visualizer::_viewUpVectorY  = 1.0;
double Visualizer::_viewUpVectorZ  = 0.0;

double Visualizer::_svpSize        = 100;
double Visualizer::_svpPositionX   = 0.0;
double Visualizer::_svpPositionY   = 0.0;
double Visualizer::_svpPositionZ   = 0.0;
double Visualizer::_svpDirectionX  = 0.0;
double Visualizer::_svpDirectionY  = 0.0;
double Visualizer::_svpDirectionZ  = 1.0;
double Visualizer::_svpUpVectorX   = 0.0;
double Visualizer::_svpUpVectorY   = 1.0;
double Visualizer::_svpUpVectorZ   = 0.0;

int Visualizer::_poseTime;
int Visualizer::_timeUnitRate;
int Visualizer::_idleEventIsOn = 0;

int Visualizer::_withCapturingIsOn     = 0;
int Visualizer::_outputTimelineDIsOn   = 0;
int Visualizer::_outputTimelineSIsOn   = 0;
int Visualizer::_outputInstrumentDIsOn = 0;
int Visualizer::_outputInstrumentSIsOn = 0;
int Visualizer::_outputGenCounterIsOn  = 0;
int Visualizer::_outputConvoyIsOn      = 0;
int Visualizer::_outputTripInfoIsOn    = 0;

int Visualizer::_viewingTimeRate;
int Visualizer::_skipRunTimeRate;
int Visualizer::_frameNumber;

int Visualizer::_simpleMapIsShown = 0;
int Visualizer::_monochromeIsOn   = 0;

int Visualizer::_roadNetworkIsShown     = 1;
int Visualizer::_interIdIsShown         = 0;
int Visualizer::_laneIdIsShown          = 0;
int Visualizer::_lanesInterIsShown      = 1;
int Visualizer::_lanesSectionIsShown    = 1;
int Visualizer::_subsectionShapeIsShown = 1;
int Visualizer::_subnetworkIsShown      = 0;
int Visualizer::_subsectionIdIsShown    = 0;
int Visualizer::_surfaceMode            = 0;
int Visualizer::_signalsIsShown         = 1;
int Visualizer::_roadsideUnitsIsShown   = 0;
int Visualizer::_connectorIdMode        = 0;

int Visualizer::_routingNetworkIsShown = 0;
int Visualizer::_routingNetworkRank    = 0;
int Visualizer::_routingNodePropMode   = 0;
int Visualizer::_routingLinkPropMode   = 0;

char Visualizer::_routingFromIntersection[16];
char Visualizer::_routingNextIntersection[16];
char Visualizer::_routingGoalIntersection[16];
char Visualizer::_routingStopPoints[512];
char Visualizer::_routingPrefRank[16];

int Visualizer::_routingLogIsShown         = 0;
unsigned long Visualizer::_routingLogCount = 0;

int Visualizer::_vehicleIdIsShown         = 0;
int Visualizer::_vehicleColorMode         = 0;
int Visualizer::_velocityColorMode        = 1;
double Visualizer::_velocityRateThreshold = 0.5;
#ifdef INCLUDE_PEDESTRIANS
int Visualizer::_pedestrianIdIsShown      = 0;
#endif //INCLUDE_PEDESTRIANS

char Visualizer::_infoVehicleId[16];
char Visualizer::_infoIntersectionId[16];

int  Visualizer::_genVehicleType = VehicleFamily::passenger();
char Visualizer::_genVehicleStart[16];
char Visualizer::_genVehicleGoal[16];
char Visualizer::_genVehicleParams[256];
char Visualizer::_genVehiclePrefRank[16];
char Visualizer::_genVehicleStopPoints[512]; 

double Visualizer::_bgXmin = -10.0;
double Visualizer::_bgXmax = 10.0;
double Visualizer::_bgYmin = -10.0;
double Visualizer::_bgYmax = 10.0;
char   Visualizer::_bgFileName[64];
GLuint Visualizer::_texture;
bool   Visualizer::_textureIsShown  = false;
int    Visualizer::_bgGrayscaleIsOn = 0;
char   Visualizer::_bgCreditString[64];

//======================================================================
Visualizer::Visualizer()
{
    _sim = NULL;
    _pfLogger = new PFLogger();
    _rnd = Random::randomGenerator();

    _viewSize
        = _svpSize
        = GVManager::getNumeric("VIS_VIEW_SIZE");

    _viewPositionX
        = _svpPositionX
        = GVManager::getNumeric("VIS_VIEW_CENTER_X");
    _viewPositionY
        = _svpPositionY
        = GVManager::getNumeric("VIS_VIEW_CENTER_Y");
    _viewPositionZ
        = _svpPositionZ
        = GVManager::getNumeric("VIS_VIEW_CENTER_Z");
 
    _viewDirectionX
       = _svpDirectionX
       = GVManager::getNumeric("VIS_VIEW_DIRECTION_X");
    _viewDirectionY
       = _svpDirectionY
       = GVManager::getNumeric("VIS_VIEW_DIRECTION_Y");
    _viewDirectionZ
       = _svpDirectionZ
       = GVManager::getNumeric("VIS_VIEW_DIRECTION_Z");

    _viewUpVectorX
       = _svpUpVectorX
       = GVManager::getNumeric("VIS_VIEW_UPVECTOR_X");
    _viewUpVectorY
       = _svpUpVectorY
       = GVManager::getNumeric("VIS_VIEW_UPVECTOR_Y");
    _viewUpVectorZ
       = _svpUpVectorZ
       = GVManager::getNumeric("VIS_VIEW_UPVECTOR_Z");

    _xmin = _viewPositionX - _viewSize - 10;
    _xmax = _viewPositionX + _viewSize + 10;
    _ymin = _viewPositionY - _viewSize - 10;
    _ymax = _viewPositionY + _viewSize + 10;

    strcpy(_bgFileName,
           GVManager::getString("VIS_BACKGROUND_TEXTURE_FILE").c_str());
    _bgXmin
        = GVManager::getNumeric("VIS_BACKGROUND_TEXTURE_XMIN");
    _bgXmax
        = GVManager::getNumeric("VIS_BACKGROUND_TEXTURE_XMAX");   
    _bgYmin
        = GVManager::getNumeric("VIS_BACKGROUND_TEXTURE_YMIN");
    _bgYmax
        = GVManager::getNumeric("VIS_BACKGROUND_TEXTURE_YMAX");     
    strcpy(_bgCreditString,
           GVManager::getString("VIS_BACKGROUND_CREDIT_STRING").c_str());

    if (GVManager::getMaxTime()<INT_MAX)
    {
        _poseTime = GVManager::getMaxTime();
    }
    else
    {
        _poseTime = INT_MAX;
    }

    _timeUnitRate = TimeManager::unit();

    // 1ステップごとの描画
    _viewingTimeRate = _skipRunTimeRate = _timeUnitRate;
    _frameNumber = 1;

    // 出力フラグの設定
    _outputTimelineDIsOn
        = GVManager::getFlag("FLAG_OUTPUT_TIMELINE_D")?1:0;
    _outputTimelineSIsOn
        = GVManager::getFlag("FLAG_OUTPUT_TIMELINE_S")?1:0;
    _outputInstrumentDIsOn
        = GVManager::getFlag("FLAG_OUTPUT_MONITOR_D")?1:0;
    _outputInstrumentSIsOn
        = GVManager::getFlag("FLAG_OUTPUT_MONITOR_S")?1:0;
    _outputGenCounterIsOn
        = GVManager::getFlag("FLAG_OUTPUT_GEN_COUNTER")?1:0;
    _outputConvoyIsOn
        = GVManager::getFlag("FLAG_OUTPUT_CONVOY_MONITOR")?1:0;
    _outputTripInfoIsOn
        = GVManager::getFlag("FLAG_OUTPUT_TRIP_INFO")?1:0;

    // 描画フラグの初期化
    GVManager::setNewFlag("VIS_SIMPLE_MAP",
                          (_simpleMapIsShown==1));
    GVManager::setNewFlag("VIS_MONOCHROME",
                          (_monochromeIsOn==1));
    GVManager::setNewFlag("VIS_INTER_ID",
                          (_interIdIsShown==1));
    GVManager::setNewFlag("VIS_LANE_ID",
                          (_laneIdIsShown==1));
    GVManager::setNewFlag("VIS_LANE_INTER",
                          (_lanesInterIsShown==1));
    GVManager::setNewFlag("VIS_LANE_SECTION",
                          (_lanesSectionIsShown==1));
    GVManager::setNewFlag("VIS_SUBSECTION_SHAPE",
                          (_subsectionShapeIsShown==1));
    GVManager::setNewFlag("VIS_SUBSECTION_ID",
                          (_subsectionIdIsShown==1));
    GVManager::setNewFlag("VIS_SUBNETWORK",
                          (_subnetworkIsShown==1));
    GVManager::setNewNumeric("VIS_SURFACE_MODE",
                             (double)_surfaceMode);
    GVManager::setNewFlag("VIS_SIGNAL",
                          (_signalsIsShown==1));
    GVManager::setNewFlag("VIS_ROADSIDE_UNIT",
                          (_roadsideUnitsIsShown==1));
    GVManager::setNewNumeric("VIS_CONNECTOR_ID_MODE",
                             (double)_connectorIdMode);
 
    GVManager::setNewFlag("VIS_ROUTING_NETWORK",
                          (_routingNetworkIsShown==1));
    GVManager::setNewNumeric("VIS_ROUTING_NODE_PROP_MODE",
                             (double)_routingNodePropMode);
    GVManager::setNewNumeric("VIS_ROUTING_LINK_PROP_MODE",
                             (double)_routingLinkPropMode);

    GVManager::setNewFlag("VIS_VEHICLE_ID",
                          (_vehicleIdIsShown==1));
    GVManager::setNewNumeric("VIS_VEHICLE_COLOR_MODE",
                             (double)_vehicleColorMode);
    GVManager::setNewNumeric("VIS_VELOCITY_COLOR_MODE",
                             (double)_velocityColorMode);
    GVManager::setNewNumeric("VIS_VELOCITY_RATE_THRESHOLD",
                             _velocityRateThreshold);
    
#ifdef INCLUDE_PEDESTRIANS
    GVManager::setNewFlag("VIS_PEDESTRIAN_ID",
                          (_pedestrianIdIsShown==1));
#endif //INCLUDE_PEDESTRIANS
}

//======================================================================
Visualizer::~Visualizer()
{
    delete _pfLogger;
    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
void Visualizer::visualize()
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // AutoGLのウィンドウを用意する

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

    // 背景色の設定
    GLColor::setBackground();

    // コンターマップの用意
    /* 赤->水色の順 */
    // AutoGL_SetContourMap_RB();
    AutoGL_ClearContourColor();
    AutoGL_AddContourColorOfGrade(0.0, 1.0, 0.0, 0.0);
    AutoGL_AddContourColorOfGrade(1.0, 0.0, 0.5, 1.0);

    // ドラッグ有効
    AutoGL_EnableDragInMode3D();

    // コールバックを登録する
    AutoGL_SetDefaultCallbackInMode3D (NULL);
    AutoGL_SetViewRedrawCallback(viewRedrawCallback);
    AutoGL_SetBatchProcessCallback(viewRenderCallback);

    // アイドルイベント処理を有効にする
    AutoGL_EnableIdleEvent();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // パネルの設定
    setPanel();
}

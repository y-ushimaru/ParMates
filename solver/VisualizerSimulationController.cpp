/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include "Simulator.h"
#include "PFLogger.h"
#include "RoutingManager.h"
#include "PFBase.h"
#include "PFLogger.h"
#include "Random.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "GenerateVehicleController.h"
#include "Vehicle.h"
#include <autogl.h>
#include <cassert>

using namespace std;
//======================================================================
void Visualizer::setSimulator(Simulator* simulator)
{
    assert(simulator->hasInit());
    _sim = simulator;
}

//======================================================================
void Visualizer::renewFlag()
{
    // 描画フラグの更新
    GVManager::resetFlag("VIS_SIMPLE_MAP",
                         (_simpleMapIsShown==1));
    GVManager::resetFlag("VIS_MONOCHROME",
                         (_monochromeIsOn==1));

    GVManager::resetFlag("VIS_INTER_ID",
                         (_interIdIsShown==1));
    GVManager::resetFlag("VIS_LANE_ID",
                         (_laneIdIsShown==1));
    GVManager::resetFlag("VIS_LANE_INTER",
                         (_lanesInterIsShown==1));
    GVManager::resetFlag("VIS_LANE_SECTION",
                         (_lanesSectionIsShown==1));
    GVManager::resetFlag("VIS_SUBSECTION_SHAPE",
                         (_subsectionShapeIsShown==1));
    GVManager::resetFlag("VIS_SUBSECTION_ID",
                         (_subsectionIdIsShown==1));
    GVManager::resetFlag("VIS_SUBNETWORK",
                         (_subnetworkIsShown==1));
    GVManager::resetNumeric("VIS_SURFACE_MODE",
                            (double)_surfaceMode);
    GVManager::resetFlag("VIS_SIGNAL",
                         (_signalsIsShown==1));
    GVManager::resetFlag("VIS_ROADSIDE_UNIT",
                         (_roadsideUnitsIsShown==1));
    GVManager::resetNumeric("VIS_CONNECTOR_ID_MODE",
                            (double)_connectorIdMode);

    GVManager::resetFlag("VIS_ROUTING_NETWORK",
                         (_routingNetworkIsShown==1));
    GVManager::resetNumeric("VIS_ROUTING_NODE_PROP_MODE",
                            (double)_routingNodePropMode);
    GVManager::resetNumeric("VIS_ROUTING_LINK_PROP_MODE",
                            (double)_routingLinkPropMode);

    GVManager::resetFlag("VIS_VEHICLE_ID",
                         (_vehicleIdIsShown==1));
    GVManager::resetNumeric("VIS_VEHICLE_COLOR_MODE",
                            (double)_vehicleColorMode);
    GVManager::resetNumeric("VIS_VELOCITY_COLOR_MODE",
                            (double)_velocityColorMode);
    GVManager::resetNumeric("VIS_VELOCITY_RATE_THRESHOLD",
                            _velocityRateThreshold);

    // シミュレータの出力フラグを更新
    GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D",
                         (_outputTimelineDIsOn==1));
    GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S",
                         (_outputTimelineSIsOn==1));
    GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D",
                         (_outputInstrumentDIsOn==1));
    GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S",
                         (_outputInstrumentSIsOn==1));
    GVManager::resetFlag("FLAG_OUTPUT_GEN_COUNTER",
                         (_outputGenCounterIsOn==1));
    GVManager::resetFlag("FLAG_OUTPUT_CONVOY_MONITOR",
                         (_outputConvoyIsOn==1));
     GVManager::resetFlag("FLAG_OUTPUT_TRIP_INFO",
                         (_outputTripInfoIsOn==1));
}

//======================================================================
void Visualizer::clearTempFlag()
{
    _routingLogIsShown = 0;
}

//======================================================================
void Visualizer::timeIncrement()
{
    // シミュレータを進める
    TimeManager::setUnit(_timeUnitRate);
    if (_viewingTimeRate>0)
    {
        _sim->run(TimeManager::time() + _viewingTimeRate);
    }
    drawButtonCallback();

    // 静止画の保存
    if (_withCapturingIsOn)
    {
        string fileName = "/tmp/mates_images/";
        GVManager::getVariable("RESULT_IMG_DIRECTORY", &fileName);
        fileName += AmuConverter::itos(_frameNumber, 6);
        fileName += ".ppm";
        _frameNumber++;
        AutoGL_SaveViewImageToPPMFile(fileName.c_str());
    }
}

//======================================================================
void Visualizer::run()
{
    if (_sim->checkLaneError())
    {
        return;
    }

    // 描画・出力フラグを更新する
    renewFlag();
    while (TimeManager::time()<static_cast<unsigned int>(_poseTime))
    {
        timeIncrement();
    }
}

//======================================================================
void Visualizer::quitButtonCallback()
{
    if (_outputTripInfoIsOn)
    {
        VehicleIO::instance().writeAllVehiclesDistanceData();
    }
    exit(0);
}

//======================================================================
void Visualizer::resetButtonCallback()
{
    if (_sim->hasInit())
    {
        cout << "inited" << endl;
    }
    drawButtonCallback();
}

//======================================================================
void Visualizer::autoIncrementButtonCallback()
{
    if (_idleEventIsOn)
    {
        AutoGL_SetIdleEventCallback(0);
        _idleEventIsOn = false;
    }
    else
    {
        AutoGL_SetIdleEventCallback(timeIncrement);
        _idleEventIsOn = true;
    }
}

//======================================================================
void Visualizer::skipRunButtonCallback()
{
    int saveTimeRate = _viewingTimeRate;
    _viewingTimeRate = _skipRunTimeRate;
    run();
    _viewingTimeRate = saveTimeRate;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Visualizer.h"
#include <autogl.h>

using namespace std;

//======================================================================
void Visualizer::setPanel()
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // View Controlパネル
    AutoGL_AddGroup(" View Control ");

    // 拡大縮小・移動・回転などのボタンを用意する
    AutoGL_SetPanelInMode3D();
    AutoGL_SetMode3D(AUTOGL_MODE_3D_TRANSLATE);

    AutoGL_AddComment();
    AutoGL_SetLabel("Viewing Parameters");
    AutoGL_AddCallback(printViewingParamsCallback,
                       "printViewingParamsCallback");
    AutoGL_SetLabel("Print");
    AutoGL_AddComment();
    AutoGL_SetLabel("Set Parameters");
    AutoGL_AddReal(&_svpSize, "_svpSize");
    AutoGL_SetLabel("ViewSize");
    AutoGL_AddReal(&_svpPositionX, "_svpPositionX");
    AutoGL_SetLabel("ViewCenter_X");
    AutoGL_AddReal(&_svpPositionY, "_svpPositionY");
    AutoGL_SetLabel("ViewCenter_Y");
    AutoGL_AddReal(&_svpPositionZ, "_svpPositionZ");
    AutoGL_SetLabel("ViewCenter_Z");
    AutoGL_AddReal(&_svpDirectionX, "_svpDirectionX");
    AutoGL_SetLabel("ViewDirection_X");
    AutoGL_AddReal(&_svpDirectionY, "_svpDirectionY");
    AutoGL_SetLabel("ViewDirection_Y");
    AutoGL_AddReal(&_svpDirectionZ, "_svpDirectionZ");
    AutoGL_SetLabel("ViewDirection_Z");
    AutoGL_AddReal(&_svpUpVectorX, "_svpUpVectorX");
    AutoGL_SetLabel("ViewUpVector_X");
    AutoGL_AddReal(&_svpUpVectorY, "_svpUpVectorY");
    AutoGL_SetLabel("ViewUpVector_Y");
    AutoGL_AddReal(&_svpUpVectorZ, "_svpUpVectorZ");
    AutoGL_SetLabel("ViewUpVector_Z");
    AutoGL_AddCallback(setViewingParamsCallback,
                       "setViewingParamsCallback");
    AutoGL_SetLabel("Set");
  
    AutoGL_AddComment();
    AutoGL_AddCallback(drawButtonCallback, "drawButtonCallback");
    AutoGL_SetLabel("Draw");
    AutoGL_AddCallback(quitButtonCallback, "quitButtonCallback");
    AutoGL_SetLabel("Quit");

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Visualizer Controlパネル
    AutoGL_AddGroup(" Visualizer Control ");

    AutoGL_AddComment();
    AutoGL_SetLabel("General");
    AutoGL_AddBoolean(&_simpleMapIsShown, "_simpleMapIsShown");
    AutoGL_SetLabel("Show Simple Map");
    /*
    AutoGL_AddBoolean(&_isMonochrome, "_isMonochrome");
    AutoGL_SetLabel("Monochrome View");
    */
    AutoGL_AddBoolean(&_roadNetworkIsShown, "_roadNetworkIsShown");
    AutoGL_SetLabel("Show Road Network");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Vehicle");
    AutoGL_AddBoolean(&_vehicleIdIsShown, "_vehicleIdIsShown");
    AutoGL_SetLabel("Show ID");
    AutoGL_AddInteger(&_vehicleColorMode, "_vehicleColorMode");
    AutoGL_SetLabel("Vehicle Color Mode");
    AutoGL_AddIntegerItem("vehicle family");
    AutoGL_AddIntegerItem("average velocity");
    AutoGL_AddIntegerItem("hybrid");
    AutoGL_AddInteger(&_velocityColorMode, "_velocityColroMode");
    AutoGL_SetLabel("Velocity Color");
    AutoGL_AddIntegerItem("binary");
    AutoGL_AddIntegerItem("gradation");
    AutoGL_AddReal(&_velocityRateThreshold,
                   "_velocityRateThreshold");
    AutoGL_SetLabel("Velocity Rate Threshold");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Intersection");
    AutoGL_AddBoolean(&_interIdIsShown, "_interIdIsShown");
    AutoGL_SetLabel("Show ID");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Lane");
    AutoGL_AddBoolean(&_laneIdIsShown, "_laneIdIsShown");
    AutoGL_SetLabel("Show ID");
    AutoGL_AddBoolean(&_lanesInterIsShown, "_lanesInterIsShown");
    AutoGL_SetLabel("Show Intersection Lane");
    AutoGL_AddBoolean(&_lanesSectionIsShown, "_lanesSectionIsShown");
    AutoGL_SetLabel("Show Section Lane");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Signal");
    AutoGL_AddBoolean(&_signalsIsShown, "_signalsIsShown");
    AutoGL_SetLabel("Show");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Roadside Unit");
    AutoGL_AddBoolean(&_roadsideUnitsIsShown, "_roadsideUnitsIsShown");
    AutoGL_SetLabel("Show");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Connector");
    AutoGL_AddInteger(&_connectorIdMode, "_connectorIdMode");
    AutoGL_SetLabel("Show Connector ID");
    AutoGL_AddIntegerItem("Disable");
    AutoGL_AddIntegerItem("Global ID");
    AutoGL_AddIntegerItem("Local ID");

    AutoGL_AddComment();
    AutoGL_AddCallback(drawButtonCallback, "drawButtonCallback");
    AutoGL_SetLabel("Draw");

#ifdef INCLUDE_PEDESTRIANS
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Pedestrian & Subsectionパネル
    AutoGL_AddGroup(" Pedestrian & Subsection ");

    /*
     * 自動車交通流のみを扱う場合には，
     * Subsectionの描画方法の選択に意味はない
     */
    AutoGL_AddComment();
    AutoGL_SetLabel("Pedestrian");
    AutoGL_AddBoolean(&_pedestrianIdIsShown,
                      "_pedestrianIdIsShown");
    AutoGL_SetLabel("Show ID");
    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Subsection");
    AutoGL_AddBoolean(&_subsectionShapeIsShown,
                      "_subsectionShapeIsShown");
    AutoGL_SetLabel("Show Shape");
    AutoGL_AddBoolean(&_subnetworkIsShown,
                      "_subnetworkIsShown");
    AutoGL_SetLabel("Show Subnetwork");
    AutoGL_AddBoolean(&_subsectionIdIsShown,
                      "_subsectionIdIsShown");
    AutoGL_SetLabel("Show ID");
    AutoGL_AddInteger(&_surfaceMode, "_surfaceMode");
    AutoGL_SetLabel("Surface Mode");
    AutoGL_AddIntegerItem("attribute");
    AutoGL_AddIntegerItem("access right");

    AutoGL_AddComment();
    AutoGL_AddCallback(drawButtonCallback, "drawButtonCallback");
    AutoGL_SetLabel("Draw");
#endif

#ifdef USE_OPENCV
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Texture パネル
    AutoGL_AddGroup(" Texture ");
    AutoGL_AddComment();
    AutoGL_SetLabel(" Background Texture ");
    AutoGL_AddString(_bgFileName, "_bgFileName", 64);
    AutoGL_SetLabel("Filename");

    AutoGL_AddReal(&_bgXmin, "_bgXmin");
    AutoGL_SetLabel("Xmin");
    AutoGL_AddReal(&_bgXmax, "_bgXmax");
    AutoGL_SetLabel("Xmax");
    AutoGL_AddReal(&_bgYmin, "_bgYmin");
    AutoGL_SetLabel("Ymin");
    AutoGL_AddReal(&_bgYmax, "_bgYmax");
    AutoGL_SetLabel("Ymax");
    AutoGL_AddBoolean(&_bgGrayscaleIsOn, "_bgGrayscaleIsOn");
    AutoGL_SetLabel("Grayscale");
    AutoGL_AddString(_bgCreditString, "_bgCreditString", 64);
    AutoGL_SetLabel("Credit");
 
    AutoGL_AddCallback(updateBackgroundTexture, "updateBackgroundTexture");
    AutoGL_SetLabel("Update Background Texture");
    AutoGL_AddCallback(clearBackgroundTexture, "clearBackgroundTexture");
    AutoGL_SetLabel("Clear Background Texture");
#endif //USE_OPENCV

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Simulator Controlパネル
    AutoGL_AddGroup(" Simulator Control ");

    AutoGL_AddComment();
    AutoGL_SetLabel(" Time Flow Control ");
    AutoGL_AddCallback(timeIncrement, "timeIncrement");
    AutoGL_SetLabel(" Time Increment ");
    AutoGL_AddCallback(autoIncrementButtonCallback,
                       "autoIncrementButtonCallback");
    AutoGL_SetLabel("Auto Time Increment");
  
    AutoGL_AddInteger(&_poseTime, "_poseTime");
    AutoGL_SetLabel("Time To Pose");
    AutoGL_AddCallback(run, "run");
    AutoGL_SetLabel("Continuous Run");

    AutoGL_AddInteger(&_skipRunTimeRate, "_skipRunTimeRate");
    AutoGL_SetLabel("Skip Time Rate");
    AutoGL_AddCallback(&skipRunButtonCallback,
                       "skipRunButtonCallback");
    AutoGL_SetLabel("Skip Continuous Run");

    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Output File");
    AutoGL_AddBoolean(&_outputTimelineDIsOn, "_outputTimelineDIsOn");
    AutoGL_SetLabel("Output Timeline Detail Data");
    AutoGL_AddBoolean(&_outputTimelineSIsOn, "_outputTimelineSIsOn");
    AutoGL_SetLabel("Output Timeline Statistic Data");
    AutoGL_AddBoolean(&_outputInstrumentDIsOn, "_outputInstrumentDIsOn");
    AutoGL_SetLabel("Output Monitoring Detail Data");
    AutoGL_AddBoolean(&_outputInstrumentSIsOn, "_outputInstrumentSIsOn");
    AutoGL_SetLabel("Output Monitoring Statistic Data");
    AutoGL_AddBoolean(&_outputGenCounterIsOn, "_outputGenCounterIsOn");
    AutoGL_SetLabel("Output Generate Counter Data");
    AutoGL_AddBoolean(&_outputConvoyIsOn, "_outputConvoyIsOn");
    AutoGL_SetLabel("Output Vehicle Convoy Data");
    AutoGL_AddBoolean(&_outputTripInfoIsOn, "_outputTripInfoIsOn");
    AutoGL_SetLabel("Output Trip Info Data");
    AutoGL_AddBoolean(&_withCapturingIsOn, "_withCapturingIsOn");
    AutoGL_SetLabel("With Capturing");

    AutoGL_AddComment();
    AutoGL_SetPanelForSave();

    AutoGL_AddComment();
    AutoGL_AddCallback(quitButtonCallback, "quitButtonCallback");
    AutoGL_SetLabel("Quit");

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Vehicle Generation パネル
    AutoGL_AddGroup(" Agent Generation ");
    AutoGL_AddComment();
    AutoGL_SetLabel("Vehicle");
    AutoGL_AddInteger(&_genVehicleType, "_genVehicleType");
    AutoGL_SetLabel("Type ID ");
    AutoGL_AddString(_genVehicleStart, "_genVehicleStart", 16);
    AutoGL_SetLabel("Origin ");
    AutoGL_AddString(_genVehicleGoal, "_genVehicleGoal", 16);
    AutoGL_SetLabel("Destination ");
    AutoGL_AddString(_genVehicleParams, "_genVehicleParams", 256);
    AutoGL_SetLabel("Routing Parameters");
    AutoGL_AddString(_genVehicleStopPoints, "_genVehicleStopPoints", 512);
    AutoGL_SetLabel("Stop Points");
    AutoGL_AddCallback(generateVehicleManual, "generateVehicleManual");
    AutoGL_SetLabel("Manually Generate");

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Routing Controlパネル
    AutoGL_AddGroup(" Routing Control ");
    AutoGL_AddBoolean(&_routingNetworkIsShown,
                      "_routingNetworkIsShown");
    AutoGL_SetLabel("Show Network for Routing");
    AutoGL_AddInteger(&_routingNetworkRank, "_routingNetworkRank");
    AutoGL_SetLabel("Network Rank");
    AutoGL_AddInteger(&_routingNodePropMode, "_routingNodePropMode");
    AutoGL_SetLabel("Show Node Property");
    AutoGL_AddIntegerItem("Disable");
    AutoGL_AddIntegerItem("ID");
    AutoGL_AddIntegerItem("Rank");
    AutoGL_AddInteger(&_routingLinkPropMode, "_routingLinkPropMode");
    AutoGL_SetLabel("Show Link Property");
    AutoGL_AddIntegerItem("Disable");
    AutoGL_AddIntegerItem("ID");
    AutoGL_AddIntegerItem("Length");
    AutoGL_AddIntegerItem("Time");
    AutoGL_AddIntegerItem("Straight");
    AutoGL_AddIntegerItem("Left");
    AutoGL_AddIntegerItem("Right");
    AutoGL_AddIntegerItem("Width");
    AutoGL_AddString(_routingFromIntersection,
                     "_routingFromIntersection", 16);
    AutoGL_SetLabel("From");
    AutoGL_AddString(_routingNextIntersection,
                     "_routingNextIntersection", 16);
    AutoGL_SetLabel("Next");
    AutoGL_AddString(_routingGoalIntersection, 
                     "_routingGoalIntersection", 16);
    AutoGL_SetLabel(" To ");
    AutoGL_AddString(_routingStopPoints,
                     "_routingStopPoints", 512);
    AutoGL_SetLabel("Via");
    AutoGL_AddString(_routingPrefRank,
                     "_routingPrefRank", 16);
    AutoGL_SetLabel("Preferred Network Rank");

    AutoGL_AddCallback(searchRoute, "searchRoute");
    AutoGL_SetLabel("Search Route");

    AutoGL_AddComment();
    AutoGL_SetLabel("Search Route Log Viewer");
    AutoGL_AddCallback(incrementSearchRouteLog,
                       "incrementSearchRouteLog");
    AutoGL_SetLabel("Increment");
    AutoGL_AddCallback(decrementSearchRouteLog,
                       "decrementSearchRouteLog");
    AutoGL_SetLabel("Decrement");
    AutoGL_AddCallback(resetSearchRouteLog, "resetSearchRouteLog");
    AutoGL_SetLabel("Reset Log Viewer");

    AutoGL_AddComment();
    AutoGL_AddCallback(drawButtonCallback, "drawButtonCallback");
    AutoGL_SetLabel("Draw");
    AutoGL_AddCallback(quitButtonCallback, "quitButtonCallback");
    AutoGL_SetLabel("Quit");

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Detail Information パネル
    AutoGL_AddGroup(" Detail Information ");
    AutoGL_AddComment();
    AutoGL_SetLabel("Vehicle");
    AutoGL_AddString(_infoVehicleId, "_infoVehicleId", 16);
    AutoGL_SetLabel("Vehicle ID");
    AutoGL_AddCallback(searchVehicle, "searchVehicle");
    AutoGL_SetLabel("Search Vehicle");
    AutoGL_AddCallback(showVehicleInfo, "showVehicleInfo");
    AutoGL_SetLabel("Show Vehicle Info");
    AutoGL_AddComment();
    AutoGL_AddComment();
    AutoGL_SetLabel("Intersection");
    AutoGL_AddString(_infoIntersectionId, "_infoIntersectionId", 16);
    AutoGL_SetLabel("Intersection ID");
    AutoGL_AddCallback(searchIntersection, "searchIntersection");
    AutoGL_SetLabel("Search Intersection");
    AutoGL_AddCallback(showIntersectionInfo, "showIntersectionInfo");
    AutoGL_SetLabel("Show Intersection Info");
}

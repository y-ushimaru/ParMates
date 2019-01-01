/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AppCalc.h"
#include "Simulator.h"
#include "TimeManager.h"
#include "GVManager.h"
#include "VehicleIO.h"
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#ifndef USE_MINGW
#include <getopt.h>
#endif

using namespace std;

//======================================================================
void AppCalc::init(int argc, char** argv, bool output)
{
    // オプションの処理とグローバル変数の準備，simulatorの作成
    AppMates::init(argc, argv, output);
    assert(_simulator);
    // _simulator->writeInitializedRoadMap();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 情報出力
    if (GVManager::getFlag("FLAG_MORE_VERBOSE"))
    {
        GVManager::print();
    }

    if (output
        && GVManager::getFlag("FLAG_VERBOSE"))
    {
        cout << "time to stop calculation    : " << _maxTime << endl;
        cout << "output timeline detail data          : "
             << (GVManager::getFlag("FLAG_OUTPUT_TIMELINE_D")?"true":"false")
             << "\noutput timeline statistic data       : "
             << (GVManager::getFlag("FLAG_OUTPUT_TIMELINE_S")?"true":"false")
             << "\noutput monitoring detail data      : "
             << (GVManager::getFlag("FLAG_OUTPUT_MONITOR_D")?"true":"false")
             << "\noutput monitoring statistic data   : "
             << (GVManager::getFlag("FLAG_OUTPUT_MONITOR_S")?"true":"false")
             << "\noutput generate counter data       : "
             << (GVManager::getFlag("FLAG_OUTPUT_GEN_COUNTER")?"true":"false")
             << "\noutput trip distance and trip time : "
             << (GVManager::getFlag("FLAG_OUTPUT_TRIP_INFO")?"true":"false")
             << endl;
    }
}

//======================================================================
void AppCalc::parseArgument(int argc, char** argv)
{
    /*
     * AppCalc独自のオプションを使用する場合はここに記述する．
     * これはAppMates::initから呼ばれる．
     */
}

//======================================================================
void AppCalc::printUsage()
{
    cout <<
        "Usage  : ./advmates-calc [Option list] \n"
         << endl;
    AppMates::printUsage();
    exit(EXIT_SUCCESS);
}

//======================================================================
int AppCalc::batchRun()
{
    ulint maxTime = GVManager::getMaxTime();

    cout << "*** Run advmates-calc: max time=" << maxTime
         << " (dt=" << TimeManager::unit() << ") ***" << endl;
    _simulator->run(maxTime);

    if (GVManager::getFlag("FLAG_OUTPUT_TRIP_INFO"))
    {
        VehicleIO::instance().writeAllVehiclesDistanceData();
    }

    return EXIT_SUCCESS;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AppSim.h"
#include "GVManager.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#ifndef USE_MINGW
#include <getopt.h>
#endif

using namespace std;

//======================================================================
void AppSim::init(int argc, char** argv, bool output)
{
    // 出力の抑制
    /*
     * このタイミングで設定された値はファイル/実行時オプションにより
     * 上書きされる可能性がある
     */
    GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D",       false);
    GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S",       false);
    GVManager::resetFlag("FLAG_OUTPUT_TRIP_INFO",        false);
    GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D",        false);
    GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S",        false);
    GVManager::resetFlag("FLAG_OUTPUT_GEN_COUNTER",      false);
    GVManager::resetFlag("FLAG_OUTPUT_CONVOY_MONITOR",   false);

    AppMates::init(argc, argv, false);
    assert(_simulator);
    _simulator->writeInitializedRoadMap();

    // 描画用変数の設定
    /*
     * 地図作成(AppMates::init)より後ろでなければならない．
     * 既にファイル/実行時オプションにより指定されていれば上書きしない．
     */
    double xmin, xmax, ymin, ymax;
    _simulator->roadMap()->region(xmin, xmax, ymin, ymax);
    double xdiff = xmax - xmin;
    double ydiff = ymax - ymin;

    // ビューのサイズ (中心から端までの長さ)
    if (xdiff >= ydiff)
    {
        GVManager::setNewNumeric("VIS_VIEW_SIZE", xdiff/2);
    }
    else
    {
        GVManager::setNewNumeric("VIS_VIEW_SIZE", ydiff/2);
    }

    // ビューの中心
    GVManager::setNewNumeric("VIS_VIEW_CENTER_X", (xmax+xmin)/2.0);
    GVManager::setNewNumeric("VIS_VIEW_CENTER_Y", (ymax+ymin)/2.0);
    GVManager::setNewNumeric("VIS_VIEW_CENTER_Z", 0);

    // 視線方向
    GVManager::setNewNumeric("VIS_VIEW_DIRECTION_X", 0);
    GVManager::setNewNumeric("VIS_VIEW_DIRECTION_Y", 0);
    GVManager::setNewNumeric("VIS_VIEW_DIRECTION_Z", 1);
   
    // 画面上方向
    GVManager::setNewNumeric("VIS_VIEW_UPVECTOR_X", 0);
    GVManager::setNewNumeric("VIS_VIEW_UPVECTOR_Y", 1);
    GVManager::setNewNumeric("VIS_VIEW_UPVECTOR_Z", 0);

    // アナログ時計の表示
    GVManager::setNewFlag("VIS_SHOW_ANALOG_CLOCK",     false);
    GVManager::setNewNumeric("VIS_CLOCK_START_HOUR",   0);
    GVManager::setNewNumeric("VIS_CLOCK_START_MINUTE", 0);

    // 背景画像ファイル
    /*
     * Visualizerでユーザが入力するため，ディレクトリ名と
     * ファイル名を別々に管理する
     */
    GVManager::setNewString("VIS_TEXTURE_DIR",
                            GVManager::getString("DATA_DIRECTORY")
                            +"texture/");
    GVManager::setNewString("VIS_BACKGROUND_TEXTURE_FILE",
                            "map.jpg");

    // 背景画像の貼り付け範囲
    GVManager::setNewNumeric("VIS_BACKGROUND_TEXTURE_XMIN",
                             -100.0);
    GVManager::setNewNumeric("VIS_BACKGROUND_TEXTURE_XMAX",
                             100.0);
    GVManager::setNewNumeric("VIS_BACKGROUND_TEXTURE_YMIN",
                             -100.0);
    GVManager::setNewNumeric("VIS_BACKGROUND_TEXTURE_YMAX",
                             100.0);

    // 背景画像の著作権表記
    GVManager::setNewString("VIS_BACKGROUND_CREDIT_STRING",
                            "");

    // Visualizerの生成
    /*
     * AppMates::initよりも後でなければならない
     */
    _vis.reset(new Visualizer());

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 情報出力
    if (GVManager::getFlag("FLAG_MORE_VERBOSE"))
    {
        GVManager::print();
    }
}

//======================================================================
void AppSim::parseArgument(int argc, char** argv)
{
#ifndef USE_MINGW
    double size = 200;
    double cx = 0, cy = 0, cz = 0;
    double dx = 0, dy = 0, dz = 1;
    double ux = 0, uy = 1, uz = 0;
    stringstream ss;

    int startTime = 0;

    int opt;

    // getoptのエラー出力を抑制する
    opterr = 0;

    while ((opt = getopt_long(argc, argv,
                              AppMates::shortOptions.c_str(),
                              AppMates::longOptions,
                              &AppMates::optionIndex)) != -1)
    {
        switch (opt)
        { 
        case 200: // ビューのサイズを指定する
            size = atof(optarg);
            GVManager::resetNumeric("VIS_VIEW_SIZE", size);
            break;
        case 201: // ビューの中心を指定する
        {
            stringstream ss;
            ss << optarg;
            ss >> cx >> cy >> cz;
            GVManager::resetNumeric("VIS_VIEW_CENTER_X", cx);
            GVManager::resetNumeric("VIS_VIEW_CENTER_Y", cy);
            GVManager::resetNumeric("VIS_VIEW_CENTER_Z", cz);
            break;
        }
        case 202: // ビューの視線方向を指定する
        {
            stringstream ss;
            ss << optarg;
            ss >> dx >> dy >> dz;
            GVManager::resetNumeric("VIS_VIEW_DIRECTION_X", dx);
            GVManager::resetNumeric("VIS_VIEW_DIRECTION_Y", dy);
            GVManager::resetNumeric("VIS_VIEW_DIRECTION_Z", dz);
            break;
        }
        case 203: // ビューの画面上向き方向を指定する
        {
            stringstream ss;
            ss << optarg;
            ss >> ux >> uy >> uz;
            GVManager::resetNumeric("VIS_VIEW_UPVECTOR_X", ux);
            GVManager::resetNumeric("VIS_VIEW_UPVECTOR_Y", uy);
            GVManager::resetNumeric("VIS_VIEW_UPVECTOR_Z", uz);
            break;
        }
        case 210: // 時計を描画する
        {
            startTime = atoi(optarg);
            int startMinute = startTime%100;
            int startHour   = (startTime - startMinute)/100;
            if (startMinute<0 || startMinute>=60
                || startHour<0 || startHour>=24)
            {
                cerr << "WARNING: startTime invalid - "
                     << startTime << endl;
                cerr << "it must be 0000 - 2359" << endl;
                break;
            }
            GVManager::resetFlag("VIS_SHOW_ANALOG_CLOCK", true);            
            GVManager::resetNumeric("VIS_CLOCK_START_HOUR", startHour);
            GVManager::resetNumeric("VIS_CLOCK_START_MINUTE", startMinute);
            break;
        }
        default:
            break;
        }
    }
#endif
}

//======================================================================
void AppSim::printUsage()
{
    cout <<
        "Usage  : ./advmates-sim [Option list] \n"
         << endl;
    AppMates::printUsage();
#ifndef USE_MINGW
    cout <<
        " --view-size <Size> : set view size to <Size>.\n"
        " --view-center <Position>\n"
        "                    : set view center to <Position>.\n"
        "                      The position format is the concatenation of\n"
        "                      signed x, y and z-coodinate\n"
        "                      (like +10.0-5.0+0).\n"
        " --view-direction <Vector>\n"
        "                    : set view direction to <Vector>.\n"
        "                      The vector format is the same as the position format.\n"
        " --view-upvector <Vector>\n"
        "                    : set up vector to <Vector>.\n"
        "                      The vector format is the same as the position format.\n"
         << endl;
#endif
    exit(EXIT_SUCCESS);
}

//======================================================================
int AppSim::run()
{
    if (_vis.get())
    {
        _vis->setSimulator(_simulator);
        _vis->visualize();
        return EXIT_SUCCESS;
    }
    else
    {
        cerr << "Visualizer not found." << endl;
        return EXIT_FAILURE;
    }
}



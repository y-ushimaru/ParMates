/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AppMates.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "GVInitializer.h"
#include "GVManager.h"
#include "Random.h"
#include "Simulator.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unistd.h>
#ifndef USE_MINGW
#include <getopt.h>
#endif
#ifdef _OPENMP
#include <omp.h>
#endif

#define MATES_NDEBUG

using namespace std;

//======================================================================
AppMates::AppMates(unsigned long maxTime)
    :  _simulator(), _dataPath(), _key(), _maxTime(maxTime)
{
    //_dataPathと_keyのデフォルト値を設定
    _dataPath = "./";

#ifdef MATES_NDEBUG
    _key = time(NULL);
#else
    _key = 2;
#endif
}

//======================================================================
AppMates::~AppMates()
{
    if (_simulator)
    {
        delete _simulator;
    }
}

//======================================================================
void AppMates::init(int argc, char** argv, bool output)
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // オプションの処理1
    /*
     * _dataPathのみ先に決定する
     */
    _setDataPath(argc, argv);
    cout << "data directory: " << _dataPath << endl;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // グローバル変数の初期化
    /*
     * この時点で_dataPathはデータディレクトリのパスが設定されている．
     * _setDataPathより後ろでなければならない．
     */
    GVInitializer::init(_dataPath);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // グローバル変数をファイルから読み込む
    /*
     * GVInitializer::initよりも後ろでなければならない．
     */
    string fileName;
    if (GVManager::getVariable("GV_INIT_FILE", &fileName))
    {
        GVManager::setVariablesFromFile(fileName);
    }
    // ファイル内でMAX_TIMEが更新されていたら_maxTimeを更新する
    if (GVManager::getMaxTime()>0)
    {
        _maxTime = GVManager::getMaxTime();
    }
    else
    {
        GVManager::resetMaxTime(_maxTime);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // オプションの処理2
    /*
     * 実行時オプションはファイル指定を上書きする．
     * GVManager::setVariablesFromFileよりも後ろでなければならない．
     */
    AppMates::parseArgument(argc, argv);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 乱数の準備
    /*
     * この時点で_keyには乱数の種が設定されている．
     * parseArgumentより後ろでなければならない．
     */
    cout << "random seed   : " << _key << endl;
    srand(_key);
    Random::setSeed(_key);
    Random::initialize();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _OPENMP
    // スレッド数設定
    /*
     * GVManager::setVariablesFromFileよりも後ろでなければならない．
     */
    int numThread
        = static_cast<int>(GVManager::getNumeric("NUM_THREAD"));
    if (numThread <= 0)
    {
        numThread = omp_get_num_procs();
        GVManager::resetNumeric("NUM_THREAD", numThread);
    }
    cout << "number of threads: " << numThread << endl;
    omp_set_num_threads(numThread);

#endif //_OPENMP

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // シミュレータの準備
    bool isReady = getReadySimulator();
    if (!isReady)
    {
        cerr << "Error: failed to get ready for running simulator." << endl;
    }
}

//======================================================================
Simulator* AppMates::simulator()
{
    return _simulator;
}

//======================================================================
int AppMates::optionIndex;
string AppMates::shortOptions = "HhD:d:R:r:T:t:SsLlMmGgQq";
struct option AppMates::longOptions[] =
{
    {"help",                 0, 0, 'h'},
    {"time",                 1, 0, 't'},

    // 入力に関するフラグの処理
    {"no-input",             0, 0, 30},
    {"no-input-map",         0, 0, 31},
    {"no-input-signal",      0, 0, 32},
    {"no-input-vehicle",     0, 0, 33},

    // 出力に関するフラグの処理
    {"output-timeline",      0, 0, 'S'},
    {"no-output-timeline",   0, 0, 's'},
    {"output-tripinfo",      0, 0, 'L'},
    {"no-output-tripinfo",   0, 0, 'l'},
    {"output-gencounter",    0, 0, 'G'},
    {"no-output-gencounter", 0, 0, 'g'},
    {"output-instrument",    0, 0, 'M'},
    {"no-output-instrument", 0, 0, 'm'},
    {"verbose",              0, 0, 'Q'},
    {"no-verbose",           0, 0, 'q'},
    {"output-timeline-d",    0, 0, 20},
    {"no-output-timeline-d", 0, 0, 21},
    {"output-timeline-s",    0, 0, 22},
    {"no-output-timeline-s", 0, 0, 23},
    {"output-monitor-d",     0, 0, 24},
    {"no-output-monitor-d",  0, 0, 25},
    {"output-monitor-s",     0, 0, 26},
    {"no-output-monitor-s",  0, 0, 27},
    {"more-verbose",         0, 0, 28},

    // シミュレーションの制御に関するフラグの処理
    {"no-generate-random-vehicle",               0, 0, 40},
    {"random-od-factor",                         1, 0, 41},
    {"tabled-od-factor",                         1, 0, 42},
    {"debug-all-section-single-lane-each-side",  0, 0, 43},
    {"debug-generate-fixed-vehicle-all-at-once", 0, 0, 44},

    // シミュレーションのキャッシュに関するフラグの処理
    {"cache-routing-read",        0, 0, 50},
    {"no-cache-routing-read",     0, 0, 51},
    {"cache-routing-write",       0, 0, 52},
    {"no-cache-routing-write",    0, 0, 53},
    {"cache-routing-size",        1, 0, 54},
    {"cache-routing-probability", 1, 0, 55},

    // 以下はAppSimの処理対象
    {"view-size",        1, 0, 200},
    {"view-center",      1, 0, 201},
    {"view-direction",   1, 0, 202},
    {"view-upvector",    1, 0, 203},
    {"show-analog-clock",1, 0, 210},

    {0, 0, 0, 0}
};

//======================================================================
void AppMates::parseArgument(int argc, char** argv)
{
    int opt;

    // getoptのエラー出力を抑制する
    opterr = 0;

#ifdef USE_MINGW
    while ((opt = getopt(argc, argv,
                         AppMates::shortOptions.c_str())) != -1)
#else //USE_MINGW
    while ((opt = getopt_long(argc, argv,
                              AppMates::shortOptions.c_str(),
                              AppMates::longOptions,
                              &AppMates::optionIndex)) != -1)
#endif //USE_MINGW
    {
        switch (opt)
        {
        case 'H':
        case 'h': // ヘルプを出力する
            printUsage();
            break;
        case 'T': // シミュレーション対象時間を指定する
        case 't':
            _maxTime = static_cast<unsigned long>(atoi(optarg));
            GVManager::resetMaxTime(_maxTime);
            break;
        case 'R':
        case 'r': // 乱数の種を指定する
            _setRandomSeed(optarg);
            break;

#ifndef USE_MINGW
        case 30:  // 入力をoffに
            GVManager::resetFlag("FLAG_INPUT_MAP", false);
            GVManager::resetFlag("FLAG_INPUT_SIGNAL", false);
            GVManager::resetFlag("FLAG_INPUT_VEHICLE", false);
            break;
        case 31:  // 地図データ入力をoffに
            GVManager::resetFlag("FLAG_INPUT_MAP", false);
            break;
        case 32:  // 信号データ入力をoffに
            GVManager::resetFlag("FLAG_INPUT_SIGNAL", false);
            break;
        case 33:
            GVManager::resetFlag("FLAG_INPUT_VEHICLE", false);
             break;
#endif //USE_MINGW

        case 'S': // 時系列データ出力をonに(calcデフォルトでon)
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D", true);
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S", true);
            break;
        case 's': // 時系列データ出力をoffに(simデフォルトでoff)
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D", false);
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S", false);
            break;
        case 'L': // 走行距離，時間の出力をonに(calcデフォルトでon)
            GVManager::resetFlag("FLAG_OUTPUT_TRIP_INFO", true);
            break;
        case 'l': // 走行距離，時間の出力をoffに(simデフォルトでoff)
            GVManager::resetFlag("FLAG_OUTPUT_TRIP_INFO", false);
            break;
        case 'M': // 路側器データ出力をonに(calcデフォルトでon)
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D", true);
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S", true);
            break;
        case 'm': // 路側器データ出力をoffに(simデフォルトでoff)
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D", false);
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S", false);
            break;
#ifndef USE_MINGW
        case 20: // 時系列データのうち詳細データの出力をonに
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D", true);
            break;
        case 21: // 時系列データのうち詳細データの出力をoffに
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_D", false);
            break;
        case 22: // 時系列データのうち統計データの出力をonに
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S", true);
            break;
        case 23: // 時系列データのうち統計データの出力をoffに
            GVManager::resetFlag("FLAG_OUTPUT_TIMELINE_S", false);
            break;
        case 24: // 路側器データのうち詳細データの出力をonに
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D", true);
            break;
        case 25: // 路側器データのうち詳細データの出力をoffに
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_D", false);
            break;
        case 26: // 路側器データのうち統計データの出力をonに
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S", true);
            break;
        case 27: // 路側器データのうち統計データの出力をoffに
            GVManager::resetFlag("FLAG_OUTPUT_MONITOR_S", false);
            break;
#endif //USE_MINGW
        case 'G': // 発生データ出力をonに(calcデフォルトでon)
            GVManager::resetFlag("FLAG_OUTPUT_GEN_COUNTER", true);
            break;
        case 'g': // 発生データ出力をoffに(simデフォルトでoff)
            GVManager::resetFlag("FLAG_OUTPUT_GEN_COUNTER", false);
            break;
        case 'Q': // 情報表示をonに(デフォルトでon)
            GVManager::resetFlag("FLAG_VERBOSE", true);
            break;
        case 'q': // 情報表示をoffに
            GVManager::resetFlag("FLAG_VERBOSE", false);
            break;

#ifndef USE_MINGW
        case 28: // さらに詳しい情報表示をonに(デフォルトでoff)
            GVManager::resetFlag("FLAG_MORE_VERBOSE", true);
            GVManager::resetFlag("FLAG_VERBOSE", true);
            break;
        case 40: // 発生交通量の指定されていない端点でランダムに車両を発生させない
            GVManager::resetFlag("FLAG_GEN_RAND_VEHICLE", false);
            break;
        case 41: // ランダムに発生させる車両の交通量の係数を指定する
            GVManager::resetNumeric("RANDOM_OD_FACTOR", atof(optarg));
            break;
        case 42: // ランダムに発生させる車両の交通量の係数を指定する
            GVManager::resetNumeric("TABLED_OD_FACTOR", atof(optarg));
            break;
        case 43:
            GVManager::resetFlag("DEBUG_FLAG_ALL_SECTION_SINGLE_LANE_EACH_SIDE",
                                 true);
            break;
        case 44:
            GVManager::resetFlag("DEBUG_FLAG_GEN_FIXED_VEHICLE_ALL_AT_ONCE",
                                 true);
            break;
#endif //USE_MINGW

#ifndef USE_MINGW
        case 50: // 経路探索のキャッシュの読み込みをonに(デフォルトでon)
            GVManager::resetFlag("FLAG_CACHE_ROUTING_READ", true);
            break;
        case 51: // 経路探索のキャッシュの読み込みをoffに
            GVManager::resetFlag("FLAG_CACHE_ROUTING_READ", false);
            break;
        case 52: // 経路探索のキャッシュの書き出しをonに(デフォルトでon)
            GVManager::resetFlag("FLAG_CACHE_ROUTING_WRITE", true);
            break;
        case 53: // 経路探索のキャッシュの書き出しをoffに
            GVManager::resetFlag("FLAG_CACHE_ROUTING_WRITE", false);
            break;
        case 54: // 経路探索のキャッシュサイズを指定する(デフォルトで10000)
            GVManager::resetNumeric("VEHICLE_CACHE_ROUTING_SIZE",
                                    atof(optarg));
            break;
        case 55: // 経路探索のキャッシュを利用する確率を指定する(デフォルトで1.0)
            GVManager::resetNumeric("VEHICLE_CACHE_ROUTING_PROBABILITY",
                                    atof(optarg));
            break;

#endif //USE_MINGW

        case 'D': // 以下はここでは使用しない
        case 'd':
        case 200:
        case 201:
        case 202:
        case 203:
        default:
            break;
        }
    }

    // 派生クラスのparseArgumentを呼ぶ
    optind = 1;
    parseArgument(argc, argv);
}

//======================================================================
void AppMates::printUsage()
{
    cout <<
        "Options:\n"
        " -t                 : time to stop calculation\n"
        "                      This must be multiple number of time step.\n"
        "                      (Time step default is 100 mili second.)\n"
        "                      If this isn't given, simulator will use default value.\n"
        " -d <DataDir>       : set root path of input and output directory.\n"
        "                      (default: current directory)\n"
        " -r <Number>        : set random seed.\n"
        "                      (default: variable number on account of current time)\n"
        " -s                 : do not output timeline data\n"
        " -m                 : do not output monitoring data\n"
#ifndef USE_MINGW
        " --no-output-monitor-d\n"
        "                    : do not output monitoring data(detail)\n"
        " --no-output-monitor-s\n"
        "                    : do not output monitoring data(statistic)\n"
#endif
        " -g                 : do not output generate counter data\n"
        " -l                 : do not output trip info\n"
#ifndef USE_MINGW
        " --no-input-signal  : do not read input files for signals.\n"
        "                      All signals show blue sign.\n"
        " --no-input-vehicle : do not read input files for vehicle generation.\n"
        " --no-generate-random-vehicle\n"
        "                    : do not generate vehicles without input data.\n"
#endif //USE_MINGW
#ifdef USE_MINGW
        " -q                 : hide detail information.\n"
#else  //USE_MINGW
        " -q \n"
        " [or --no-verbose]  : hide detail information.\n"
#endif //USE_MINGW
         << endl;
}

//======================================================================
bool AppMates::getReadySimulator()
{
    bool result = true;

    //_simulatorの新規作成
    if (_simulator != NULL)
    {
        _simulator = NULL;
    }
    _simulator = new Simulator();

    // 地図を準備する
    if (GVManager::getFlag("FLAG_INPUT_MAP"))
    {
        result = _simulator->getReadyRoadMap();

        // 路側器に関するファイルを読み込む
        // 自動車以外を計測する場合もあるかもしれないので，
        // 下のifndefの外にした
        result = _simulator->getReadyRoadsideUnit();
        if (!result)
        {
            cerr << "Cannot Load RoadsideUnit Files." << endl;
            return result;
        }
#ifdef INCLUDE_VEHICLES
        // 車両に関するファイルを読み込む
        result = _simulator->getReadyVehicles();
        if (!result)
        {
            cerr << "Cannot Load Vehicle Files." << endl;
            return result;
        }
#endif //INCLUDE_VEHICLES

#ifdef INCLUDE_PEDESTRIANS
        // 車両に関するファイルを読み込む
        result = _simulator->pedExt()->getReadyPedestrians();
        if (!result)
        {
            cerr << "Cannot Load Pedestrian Files." << endl;
            return result;
        }
#endif //INCLUDE_PEDESTRIANS

    }
    else
    {
        // 地図を読み込まない場合->サンプルシナリオ
        result = false;
    }
    if (!result)
    {
        cerr << "Cannot Create RoadMap." << endl;
        return result;
    }

    // レーンチェック
    _simulator->checkLane();
    return result;
}

//======================================================================
bool AppMates::_setDataPath(int argc, char** argv)
{
    string path;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ファイル(./dataPath.txt)の処理
    /*
     * コマンドラインから起動しない場合の代替措置として用意．
     * このファイルのみ，実行ディレクトリ限定で読み込む
     * (ディレクトリを指定できないことに対処するため)．
     */

    ifstream ifs("./dataPath.txt", ios::in);
    if (ifs)
    {
        string str;
        while (ifs.good())
        {
            // 1行ごとの処理
            getline(ifs, str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                vector<string> tokens;
                AmuStringOperator::getTokens(&tokens, str, '=');
                if (tokens.size()!=2)
                {
                    continue;
                }

                // キー"DATA_DIRECTORY"のみを処理する
                if (tokens[0]=="DATA_DIRECTORY"
                    && !tokens[1].empty())
                {
                    if (tokens[1][tokens[1].length()-1] != '/')
                    {
                        tokens[1] += '/';
                    }
                    _dataPath = tokens[1];
                }
            }
        }
        ifs.close();
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // コマンドラインオプションの処理(こちらを優先する)

    int opt;

    // getoptのエラー出力を抑制する
    opterr = 0;

#ifdef USE_MINGW
    while ((opt = getopt(argc, argv,
                         AppMates::shortOptions.c_str())) != -1)
#else //USE_MINGW

    while ((opt = getopt_long(argc, argv,
                              AppMates::shortOptions.c_str(),
                              AppMates::longOptions,
                              &AppMates::optionIndex)) != -1)

#endif //USE_MINGW
    {
        switch (opt)
        {
        case 'D':
        case 'd':
            path = optarg;
            if (!path.empty())
            {
                if (path[path.length()-1] != '/')
                {
                    path += '/';
                }
                _dataPath = path;
            }
            break;
        default:
            break;
        }
    }
    optind = 1;
    return true;
}

//======================================================================
bool AppMates::_setRandomSeed(string arg)
{
    if (!arg.empty())
    {
        _key = static_cast<unsigned int>(AmuConverter::strtoul(arg));
        return true;
    }
    else
    {
        return false;
    }
}

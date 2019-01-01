/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "DetectorIO.h"
#include "DetectorUnit.h"
#include "Detector.h"
#include "Conf.h"
#include "ObjManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include "RoadMap.h"
#include "Section.h"
#include "Lane.h"
#include "Vehicle.h"
#include "TimeManager.h"
#include "GVManager.h"
#include <string>
#include <cmath>
#include <cassert>

#define TRUCK_2_PASSENGER 1.7

using namespace std;

//======================================================================
void DetectorIO::getReadyDetectors(RoadMap* roadMap)
{
    assert(roadMap);

    string fDetector;
    GVManager::getVariable("DETECTOR_FILE", &fDetector);

    ifstream inDetectorFile(fDetector.c_str(), ios::in);
    if (!inDetectorFile.good())
    {
        cout << endl
             << "no detector file:" << endl 
             << "  " << fDetector << endl;
        return;
    }
  
    string str;
    while (inDetectorFile.good())
    {
        getline(inDetectorFile, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            string id;
            string beginInterId;
            string endInterId;
            double length;
            ulint interval;

            AmuStringOperator::getTokens(&tokens, str, ',');
            assert(tokens.size()==5);
            // 1番目のカラムは感知器の識別番号
            id = tokens[0];
            // 2番目のカラムは始点となる交差点の識別番号
            beginInterId = tokens[1];
            // 3番目のカラムは終点となる交差点の識別番号
            endInterId = tokens[2];
            // 4番目のカラムは始点からの距離
            // マイナス値は終点からの距離
            length = atof(tokens[3].c_str());
            // 5番目のカラムは時間間隔
            // 0なら統計情報を出力しない
            interval = atoi(tokens[4].c_str());

            // 指定された単路が本当に存在するかチェックする
            string sectionId;
            bool isUp;
            if (atoi(beginInterId.c_str()) < atoi(endInterId.c_str()))
            {
                sectionId
                    = AmuConverter::formatId(beginInterId,
                                             NUM_FIGURE_FOR_INTERSECTION)
                    + AmuConverter::formatId(endInterId,
                                             NUM_FIGURE_FOR_INTERSECTION);
                isUp = true;
            }
            else
            {
                sectionId
                    = AmuConverter::formatId(endInterId,
                                             NUM_FIGURE_FOR_INTERSECTION)
                    + AmuConverter::formatId(beginInterId,
                                             NUM_FIGURE_FOR_INTERSECTION);
                isUp = false;
            }
            Section* section = roadMap->section(sectionId);
            if (!section)
            {
                cerr << "detector unit " 
                     << AmuConverter::formatId(id,
                                               NUM_FIGURE_FOR_DETECTORUNIT)
                     << ": invalid input - "
                     << "section " << sectionId
                     << " does not exist." << endl; 
            }
            else if (fabs(length)>section->length())
            {
                cerr << "detector unit "
                     << AmuConverter::formatId(id,
                                               NUM_FIGURE_FOR_DETECTORUNIT)
                     << ": invalid input - "
                     << "distance "<< length
                     << " of section " << sectionId << endl; 
            }
            else
            {
                DetectorUnit* ptUnit
                    = new DetectorUnit
                    (AmuConverter::formatId(id, 
                                            NUM_FIGURE_FOR_DETECTORUNIT));
                ptUnit->setPosition(section, isUp, length, interval);
                bool isAdd = ObjManager::addDetectorUnit(ptUnit);
                if (!isAdd)
                {
                    // ObjManagerに追加された場合は
                    // デストラクタでdeleteされる．
                    cerr << "detector unit "
                         << AmuConverter::formatId
                        (id, NUM_FIGURE_FOR_DETECTORUNIT)
                         << ": cannot be added." << endl;
                    delete ptUnit;
                    ptUnit = NULL;
                }
            }
        }
    }
    inDetectorFile.close();
}

//======================================================================
void DetectorIO::getReadyOutputFiles(vector<DetectorUnit*>* units)
{
    /*
     * 1ユニットにつき2ファイルを作成する．
     */
    for (unsigned int i=0; i<units->size(); i++) 
    {
        string outputDir, prefixD, prefixS;
        GVManager::getVariable("RESULT_INSTRUMENT_DIRECTORY", &outputDir);
        GVManager::getVariable("RESULT_DETECTORD_PREFIX", &prefixD);
        GVManager::getVariable("RESULT_DETECTORS_PREFIX", &prefixS);

        if (!outputDir.empty())
        {
            // Detailファイル
            string fDetail = outputDir + prefixD
                + (*units)[i]->id() + ".txt";
            // 同名のファイルが既に存在する場合は内容が破棄されるので注意．
            // 計算を何回も行う場合など
            ofstream outDetailFile(fDetail.c_str(), ios::trunc);
      
            if (!outDetailFile.fail())
            {
                outDetailFile
                    << "#section:" << (*units)[i]->section()->id()
                    << ", isUP:"   << (*units)[i]->isUp()
                    << ", length:" << (*units)[i]->length()
                    << ", detectors:" << (*units)[i]->detectors()->size()
                    << endl
                    << "#time, lane, vehicle_id, vehicle_type,"
                    << " origin, destination, velocity"
                    << endl;
            }
            outDetailFile.close();
    
            // Statisticファイル
            string fStat = outputDir + prefixS
                + (*units)[i]->id() + ".txt";
            // 同名のファイルが既に存在する場合は内容が破棄されるので注意．
            // 計算を何回も行う場合など
            ofstream outStatFile(fStat.c_str(), ios::trunc);
      
            if (!outStatFile.fail())
            {
                DetectorUnit::StatVehicleData svd
                    = (*units)[i]->statVehicleData();
                outStatFile
                    << "#section:" << (*units)[i]->section()->id()
                    << ", isUP:"   << (*units)[i]->isUp()
                    << ", length:" << (*units)[i]->length()
                    << ", interval:" << (*units)[i]->interval()
                    << ", detectors:" << (*units)[i]->detectors()->size()
                    << endl;
                for (unsigned int j=0; j<svd.laneIds.size(); j++)
                {
                    outStatFile
                        << "#lane" << j << ":" << svd.laneIds[j] << endl;
                }
                outStatFile
                    << "#begintime, endtime,"
                    << " total(conv), total(simple), total_p,total_t,"
                    << " volume_p,volume_t,"
                    << " volume0p,volume0t,"
                    << " volume1p,volume1t, ..."
                    << endl;
            }
            outStatFile.close(); 
        }
    }
}

//======================================================================
void DetectorIO::writeTrafficData(vector<DetectorUnit*>* units)
{
    for (unsigned int i=0; i<units->size(); i++)
    {
        string outputDir, prefixD;
        GVManager::getVariable("RESULT_INSTRUMENT_DIRECTORY", &outputDir);
        GVManager::getVariable("RESULT_DETECTORD_PREFIX", &prefixD);

        if (!outputDir.empty()
            && GVManager::getFlag("FLAG_OUTPUT_MONITOR_D"))
        {
            // Detailデータの出力
            /*
             * 出力形式は，
             *   time, laneID, vehicleID, vehicleType,
             *   origin, destination, velocity
             * とする．ここを変更したらgetReadyOutputFilesの出力も変更．
             */
            vector<Detector::PassedVehicleData> result_pvd;
            if ((*units)[i]->getPassedVehicleData(&result_pvd))
            {
                // 出力ファイルを開く
                string fDetail = outputDir + prefixD
                    + (*units)[i]->id() + ".txt";
                ofstream outDetailFile(fDetail.c_str(), ios::app);

                if (!outDetailFile.fail())
                {
                    for (unsigned int j=0; j<result_pvd.size(); j++)
                    {
                        outDetailFile
                            << AmuConverter::itos
                            (TimeManager::time(), 9) << ","
                            << result_pvd[j].lane->id() << ","
                            << result_pvd[j].vehicle->id() << ","
                            << result_pvd[j].vehicle->type() << ","
                            << result_pvd[j].vehicle
                            ->route()->start()->id() << ","
                            << result_pvd[j].vehicle
                            ->route()->goal()->id()  << ","
                            << result_pvd[j].vehicle->velocity()
                            << endl;
                    }
                }
                outDetailFile.close();
            }
            // 出力した情報のクリア
            (*units)[i]->clearPassedVehicleData();
        }
        if (TimeManager::time()%(*units)[i]->interval()==0
            && GVManager::getFlag("FLAG_OUTPUT_MONITOR_S"))
        {
            // Staticticデータの出力
            writeStatData((*units)[i]);
            // 出力した情報のクリア
            (*units)[i]->clearStatVehicleData();
        }
    }
}

//======================================================================
void DetectorIO::writeStatData(DetectorUnit* unit)
{
    // Statisticデータの出力
    /*
     * 出力形式は，
     *   begintime, endtime,
     *   total(conv), total(simple), total(p,t),
     *   volume(p,t), volume0(p,t), ...
     * とする．ここを変更したらgetReadyOutputFilesの出力も変更．
     */
    string outputDir, prefixS;
    GVManager::getVariable("RESULT_INSTRUMENT_DIRECTORY", &outputDir);
    GVManager::getVariable("RESULT_DETECTORS_PREFIX", &prefixS);

    if (!outputDir.empty())
    {
        DetectorUnit::StatVehicleData svd = unit->statVehicleData();

        string fStat = outputDir + prefixS + unit->id() + ".txt";
        ofstream outStatFile(fStat.c_str(), ios::app);

        if (!outStatFile.fail())
        {
            outStatFile
                << AmuConverter::itos(svd.beginTime, 9) << ", "
                << AmuConverter::itos(TimeManager::time(), 9) << ", "
                << svd.totalAllPassengers+svd.totalAllTrucks 
                * TRUCK_2_PASSENGER << ", "
                << svd.totalAllPassengers+svd.totalAllTrucks << ", "
                << svd.totalAllPassengers << ","
                << svd.totalAllTrucks << ", "
                << svd.sumPassengers << ","
                << svd.sumTrucks;
            for (unsigned int i=0; i<svd.numPassengers.size(); i++)
            {
                outStatFile
                    << ", "
                    << svd.numPassengers[i] << ","
                    << svd.numTrucks[i];
            }
            outStatFile
                << endl;
        }
        outStatFile.close();
    }
}

//======================================================================
void DetectorIO::print()
{
    vector<DetectorUnit*>* units = ObjManager::detectorUnits();
    if ((*units).size()==0)
    {
        return;
    }
    cout << endl
         << "*** Detector Information ***" << endl;
    for (int i=0; i<static_cast<signed int>(units->size()); i++)
    {
        (*units)[i]->print();
    }
    cout << endl;
}

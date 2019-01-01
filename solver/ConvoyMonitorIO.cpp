/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "ConvoyMonitorIO.h"
#include "RoadMap.h"
#include "Section.h"
#include "Lane.h"
#include "Vehicle.h"
#include "RoadOccupant.h"
#include "GVManager.h"
#include "TimeManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include "Conf.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#ifdef INCLUDE_TRAMS
#include "tram/TramLaneInSection.h"
#endif //INCLUDE_TRAMS
using namespace std;

//======================================================================
void ConvoyMonitorIO::getReadyMonitors(RoadMap* roadMap)
{
    assert(roadMap);

    string finName
        = GVManager::getString("CONVOY_MONITOR_FILE");
    string outputDir
        = GVManager::getString("RESULT_INSTRUMENT_DIRECTORY");
    string prefix
        = GVManager::getString("RESULT_CONVOY_MONITOR_PREFIX");

    ifstream fin(finName.c_str(), ios::in);
    if (!fin.good())
    {
        cout << endl
             << "no convoy monitor file:" << endl
             << "  " << finName << endl;
        return;
    }

    // ファイルの読み込み
    while (fin.good())
    {
        string str;
        getline(fin, str);
        AmuStringOperator::getAdjustString(&str);
        if (str.empty())
        {
            continue;
        }
        
        vector<string> tokens;
        AmuStringOperator::getTokens(&tokens, str, ',');
        if (tokens.size()==2)
        {
            // 第1カラムは始点ID
            string beginInterId
                = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            // 第2カラムは終点ID
            string endInterId
                = AmuConverter::formatId
                (tokens[1], NUM_FIGURE_FOR_INTERSECTION);

            // 単路IDの決定
            string sectionId;
            bool isUp;
            string foutName;
            if (beginInterId < endInterId)
            {
                sectionId = beginInterId + endInterId;
                isUp = true;
                foutName
                    = outputDir + prefix + "u"
                    + sectionId + ".txt"; 
            }
            else
            {
                sectionId = endInterId + beginInterId;
                isUp = false;
                foutName
                    = outputDir + prefix + "d"
                    + sectionId + ".txt"; 
            }
            Section* section = roadMap->section(sectionId);

            if (!section)
            {
                cerr << "convoy monitor: invalid input - "
                     << "section " << sectionId << " does not exist."
                     << endl;
                continue;
            }
            section->setConvoyMonitored(isUp, true);

            ofstream fout(foutName.c_str(), ios::out);
            if (!fout.fail())
            {
                fout << "#time, lane id, length to front intersection"
                     << endl;
                fout.close();
            }
        }
        else
        {
            cerr << "ERROR: in file " << finName
                 << ", invalid line - " << str << endl;
        }
    }
    fin.close();
}

//======================================================================
void ConvoyMonitorIO::writeConvoyData(RoadMap* roadMap)
{
    if (TimeManager::time()
        % (static_cast<int>(GVManager::getNumeric
                           ("INTERVAL_CONVOY_MONITOR")) * 1000)
        !=0)
    {
        return;
    }

    const RMAPS* sections = roadMap->sections();
    for (CITRMAPS its = sections->begin();
         its != sections->end();
         its++)
    {
        if ((*its).second->isConvoyMonitored(false))
        {
            writeConvoyData((*its).second, false);
        }
        if ((*its).second->isConvoyMonitored(true))
        {
            writeConvoyData((*its).second, true);
        }
    }

}

//======================================================================
void ConvoyMonitorIO::writeConvoyData(Section* section, bool isUp)
{
    string outputDir
        = GVManager::getString("RESULT_INSTRUMENT_DIRECTORY");
    string prefix
        = GVManager::getString("RESULT_CONVOY_MONITOR_PREFIX");
    string foutName;
    if (isUp)
    {
        foutName
            = outputDir + prefix + "u"
            + section->id() + ".txt"; 
    }
    else
    {
        foutName
            = outputDir + prefix + "d"
            + section->id() + ".txt"; 
    }

    ofstream fout(foutName.c_str(), ios::app);
    if (!fout.good())
    {
        cerr << "ERROR: file "<< foutName
             << " not found.";
        return;
    }
    fout << TimeManager::time();

    // レーンの取得
    const RMAPLAN* lanes = section->lanes();
    for (CITRMAPLAN citl = lanes->begin();
         citl != lanes->end();
         citl++)
    {
        // レーンごとの処理
        if (section->isUp((*citl).second)!=isUp)
        {
            continue;
        }
#ifdef INCLUDE_TRAMS
        if (dynamic_cast<TramLaneInSection*>
            (const_cast<Lane*>((*citl).second)))
        {
            // 路面電車レーンを集計対象としない
            continue;
        }
#endif //INCLUDE_TRAMS

        fout << ", " << (*citl).second->id();

        bool isStopVehicleFound = false;
        // エージェントの取得
        vector<RoadOccupant*>* agents
            = (*citl).second->agents();
        for (unsigned int i=0; i<agents->size(); i++)
        {
            /*
             * agentsは始点に近い順に並んでいるため，
             * 車列最後尾の停車車両とは
             * iの小さい順にサーチして最初に速度が0になったもの
             */
            if ((*agents)[i]->velocity() < 1.0e-6)
            {
                fout << ", "
                     << section->lengthToNext
                    ((*citl).second, (*agents)[i]->length());
                isStopVehicleFound = true;
                break;
            }
        }
        if (!isStopVehicleFound)
        {
            fout << ", 0";
        }
    }
    fout << endl;
    fout.close();
}

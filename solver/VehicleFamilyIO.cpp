/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "VehicleFamilyIO.h"
#include "VehicleFamilyManager.h"
#include "GVManager.h"
#include "AmuStringOperator.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

using namespace std;

//======================================================================
void VehicleFamilyIO::getReadyVehicleFamily()
{
    string fVehicleFamily;
    GVManager::getVariable("VEHICLE_FAMILY_FILE", &fVehicleFamily);

    ifstream inVehicleFamilyFile(fVehicleFamily.c_str(), ios::in);

    if (!inVehicleFamilyFile.good())
    {
        cout << endl
             << "no vehicle family file:" << endl
             << "  " << fVehicleFamily << endl;
        return;
    }

    // ファイルの読み込み
    string str;
    while (inVehicleFamilyFile.good())
    {
        getline(inVehicleFamilyFile, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            AmuStringOperator::getTokens(&tokens, str, ',');
            if (tokens.size()==10
                || tokens.size()==11)
            {
                int index = 0;
                int id;
                double bodyLength, bodyWidth, bodyHeight;
                int bodyArticulation = 1;
                double maxAcceleration, maxDeceleration;
                double bodyColorR, bodyColorG, bodyColorB;

                id = atoi(tokens[index].c_str());
                index++;
                bodyLength = atof(tokens[index].c_str());
                index++;
                bodyWidth  = atof(tokens[index].c_str());
                index++;
                bodyHeight = atof(tokens[index].c_str());
                index++;
                if (tokens.size()==11)
                {
                    bodyArticulation = atoi(tokens[index].c_str());
                    if (bodyArticulation<0
                        || bodyArticulation>3)
                    {
                        cerr << "WARNING: bodyArticulation "
                             << " must be 1, 2, or 3" << endl;
                        cerr << "  4th column in line: "
                             << str << endl;
                        bodyArticulation = 1;
                    }                    
                    index++;
                }

                // 次のカラムは現状で使用しない
                // bodyWeight = atof(tokens[index].c_str());
                index++;
                
                maxAcceleration = atof(tokens[index].c_str());
                index++;
                maxDeceleration = atof(tokens[index].c_str());
                index++;
                
                bodyColorR = atof(tokens[index].c_str());
                index++;
                bodyColorG = atof(tokens[index].c_str());
                index++;
                bodyColorB = atof(tokens[index].c_str());
                index++;

                VFAttribute vehicleFamily
                    (id,
                     bodyLength, bodyWidth, bodyHeight,
                     bodyArticulation,
                     0.0,
                     maxAcceleration, maxDeceleration,
                     bodyColorR, bodyColorG, bodyColorB);
                VehicleFamilyManager::addVehicleFamily(vehicleFamily);
            }
        }
    }
}

//======================================================================
void VehicleFamilyIO::print()
{
    cout << endl
         << "*** Vehicle Family Parameters ***" << endl;

    map<int, VFAttribute>* families = VehicleFamilyManager::families();
    for (map<int, VFAttribute>::iterator itvf = families->begin();
         itvf!=families->end(); itvf++)
    {
        int id;
        double length, width, height;
        double weight = 0.0;
        double accel, decel;
        double r, g, b;

        id = itvf->second.id();
        itvf->second.getSize(&length, &width, &height);
        itvf->second.getPerformance(&accel, &decel);
        itvf->second.getBodyColor(&r, &g, &b);

        cout << id << ": "
             << "(" << length << ", " << width << ", " << height
             << "/" << weight << "), ("
             << accel << ", " << decel << "), ("
             << r << ", " << g << ", " << b << ")" << endl; 
    }
}

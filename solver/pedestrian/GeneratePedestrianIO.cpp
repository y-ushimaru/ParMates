#ifdef INCLUDE_PEDESTRIANS

#include "GeneratePedestrianIO.h"
#include "Zebra.h"
#include "../RoadMap.h"
#include "../Intersection.h"
#include "../GVManager.h"
#include "../TimeManager.h"
#include "../AmuStringOperator.h"
#include "../AmuConverter.h"
#include "../Conf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>

using namespace std;

//======================================================================
GeneratePedestrianIO& GeneratePedestrianIO::instance()
{
    static GeneratePedestrianIO instance;
    return instance;
}

//======================================================================
void GeneratePedestrianIO::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

//======================================================================
bool GeneratePedestrianIO::readPedGenVolume()
{
    assert(_roadMap);

    string fileName = GVManager::getString("GENERATE_PEDESTRIAN_FILE");
    fstream fin;
    fin.open(fileName.c_str(), ios::in);
    if (!fin.good())
    {
        cout << endl
             << "no pedestrian generate file:" << endl
             << "  " << fileName << endl;
        return false;
    }

    string str;
    while (fin.good())
    {
        getline(fin, str);

        // 文字列の整形
        AmuStringOperator::getAdjustString(&str);
        if (str.empty())
        {
            continue;
        }

        vector<string> tokens;
        AmuStringOperator::getTokens(&tokens, str, ',');
        if (tokens.size()==4)
        {
            // 第1カラムは交差点ID
            string interId
                = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            Intersection* inter
                = _roadMap->intersection(interId);
            if (!inter)
            {
                cerr << "ERROR: intersection "
                     <<  interId << " not found." << endl;
                continue;
            }

            // 第2カラムは境界番号
            int dir
                = atoi(tokens[1].c_str());
            Zebra* zebra
                = inter->pedExt()->zebra(dir);
            if (!zebra)
            {
                cerr << "ERROR: zebra "
                     << dir << " in intersection "
                     << interId << " not found." << endl;
                continue;
            }

            // 第3カラムは横断方向
            int zebraDir
                = atoi(tokens[2].c_str());
            bool isUp;
            if (zebraDir <= 0)
            {
                isUp = false;
            }
            else
            {
                isUp = true;
            }

            // 第4カラムは確率
            double p
                = atof(tokens[3].c_str())
                * TimeManager::unit()
                / 60.0 / 60.0 / 1000.0;

            // 横断歩道に確率を設定
            zebra->setPedGenProbability(isUp, p);
        }
        else
        {
            cerr << "ERROR: invalid line/"
                 << str << endl;
        }
    }
}

#endif //INCLUDE_PEDESTRIANS

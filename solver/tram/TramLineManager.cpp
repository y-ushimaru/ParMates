/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "TramLineManager.h"
#include "TramLine.h"
#include "../RoadMap.h"
#include "../Intersection.h"
#include "../GVManager.h"
#include "../AmuStringOperator.h"
#include "../AmuConverter.h"
#include "../Conf.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

//======================================================================
TramLineManager::TramLineManager()
{
    _tramLines.clear();
}

//======================================================================
TramLineManager::~TramLineManager()
{
    for (unsigned int i=0; i<_tramLines.size(); i++)
    {
        delete _tramLines[i];
    }
    _tramLines.clear();
}

//======================================================================
TramLineManager& TramLineManager::instance()
{
    static TramLineManager instance;
    return instance;
}

//======================================================================
void TramLineManager::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

//======================================================================
void TramLineManager::readTramLineFile()
{
    string fileName = GVManager::getString("TRAM_LINE_FILE");

    fstream fin;
    fin.open(fileName.c_str());
    if (!fin.good())
    {
        cout << endl
             << "no tram line file:" << endl
             << "  " << fileName << endl;
        return;
    }

    while (fin.good())
    {
        string str;
        vector<string> tokens;

        getline(fin, str);
        AmuStringOperator::getAdjustString(&str);
        if (str.empty())
        {
            continue;
        }

        TramLine* tramLine = new TramLine();
        _tramLines.push_back(tramLine);

        AmuStringOperator::getTokens(&tokens, str, ',');
        for (unsigned int i=0; i<tokens.size(); i+=3)
        {
            TramLineSide prevSide;
            TramLineSide nextSide;
            Intersection* inter = NULL;
            Intersection* prev  = NULL;
            Intersection* next  = NULL;

            // 3カラムごとのデータ

            // 第3nカラムは流入レーンの位置(prevSide)
            if (tokens[i]=="N" || tokens[i]=="n")
            {
                prevSide = NONE;
            }
            else if (tokens[i]=="L" || tokens[i]=="l")
            {
                prevSide = LEFT;
            }
            else if (tokens[i]=="C" || tokens[i]=="c")
            {
                prevSide = CENTER;
            }
            else if (tokens[i]=="R" || tokens[i]=="r")
            {
                prevSide = RIGHT;
            }
            else
            {
                prevSide = UNDEFINED;
            }

            // 第3n+1カラムは交差点ID
            inter
                = _roadMap->intersection
                (AmuConverter::formatId
                 (tokens[i+1], NUM_FIGURE_FOR_INTERSECTION));

            // 第3n+2カラムは流出レーンの位置(nextSide)
            if (tokens[i+2]=="N" || tokens[i+2]=="n")
            {
                nextSide = NONE;
            }
            else if (tokens[i+2]=="L" || tokens[i+2]=="l")
            {
                nextSide = LEFT;
            }
            else if (tokens[i+2]=="C" || tokens[i+2]=="c")
            {
                nextSide = CENTER;
            }
            else if (tokens[i+2]=="R" || tokens[i+2]=="r")
            {
                nextSide = RIGHT;
            }
            else
            {
                nextSide = UNDEFINED;
            }

            // 第3n-2カラムは前の交差点ID
            if (static_cast<int>(i)-2>0)
            {
                prev = _roadMap->intersection
                    (AmuConverter::formatId
                     (tokens[i-2], NUM_FIGURE_FOR_INTERSECTION));
            }

            // 第3n+4カラムは次の交差点ID
            if (i+4<tokens.size())
            {
                next = _roadMap->intersection
                    (AmuConverter::formatId
                     (tokens[i+4], NUM_FIGURE_FOR_INTERSECTION));
            }

            tramLine->createTramLineInter(inter,
                                          prev, prevSide,
                                          next, nextSide);
        }
    }

    fin.close();
}

//======================================================================
void TramLineManager::getNumTramConnectors
(Intersection* inter, int dir,
 int* result_numInLeft,   int* result_numOutLeft,
 int* result_numInCenter, int* result_numOutCenter,
 int* result_numInRight,  int* result_numOutRight)
{
    for (unsigned int i=0; i<_tramLines.size(); i++)
    {
        _tramLines[i]->getNumTramConnectors
            (inter, dir,
             result_numInLeft,   result_numOutLeft,
             result_numInCenter, result_numOutCenter,
             result_numInRight,  result_numOutRight);
    }
}

//======================================================================
bool TramLineManager::isTramLineConnected(Intersection* inter,
                                          int from, int to)
{
    for (unsigned int i=0; i<_tramLines.size(); i++)
    {
        if (_tramLines[i]->isTramLineConnected(inter, from, to))
        {
            return true;
        }
    }
    return false;
}

//======================================================================
TramLine* TramLineManager::tramLineWithDesignatedGates
(const vector<const Intersection*>* gates)
{
    TramLine* result = NULL;

    for (unsigned int i=0; i<_tramLines.size(); i++)
    {
        vector<const Intersection*> tramLineInters;
        _tramLines[i]->getIntersections(&tramLineInters);

        // 起点が一致しなければならない
        if (tramLineInters[0] != (*gates)[0])
        {
            continue;
        }
        // 終点が一致しなければならない
        if (tramLineInters[tramLineInters.size()-1]
            != (*gates)[(*gates).size()-1])
        {
            continue;
        }
        if ((*gates).size()==2)
        {
            // gatesのサイズが2である場合は起点・終点チェックのみで良い
            result = _tramLines[i];
            break;
        }
        else
        {
            // 通過交差点が指定されている場合
            unsigned int j=1;
            unsigned int k=1;
            bool isTramLineFound = false;
            for (;j<tramLineInters.size()-1; j++)
            {
                if (tramLineInters[j]==(*gates)[k])
                {
                    k++;
                    if (k==(*gates).size()-1)
                    {
                        isTramLineFound = true;
                        break;
                    }
                }
            }
            if (isTramLineFound)
            {
                result = _tramLines[i];
                break;
            }
        }
    }
    return result;
}

//======================================================================
void TramLineManager::print() const
{
    if (_tramLines.size()>0)
    {
        cout << endl << "*** Tram Lines ***" << endl;
    } 
    for (unsigned int i=0; i<_tramLines.size(); i++)
    {
        cout << "line " << i << ":" << endl;
        _tramLines[i]->print();
        cout << endl;
    }
}

#endif //INCLUDE_TRAMS

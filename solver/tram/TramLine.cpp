/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_TRAMS

#include "TramLine.h"
#include "../RoadMap.h"
#include "../Intersection.h"

using namespace std;

//======================================================================
TramLine::TramLine()
{
    _inters.clear();
}

//======================================================================
TramLine::~TramLine()
{
    for (unsigned int i=0; i<_inters.size(); i++)
    {
        delete _inters[i];
    }
    _inters.clear();
}

//======================================================================
bool TramLine::createTramLineInter(Intersection* inter,
                                   Intersection* prev,
                                   TramLineSide prevSide,
                                   Intersection* next,
                                   TramLineSide nextSide)
{
    TramLineInter* tramInter = new TramLineInter;

    tramInter->inter = inter;

    if (!prev)
    {
        // prevがNULL -> 始点
        tramInter->prev     = NULL;
        tramInter->prevDir  = -1;
        tramInter->prevSide = NONE;
    }
    else
    {
        tramInter->prev     = prev;
        tramInter->prevDir  = inter->direction(prev);
        tramInter->prevSide = prevSide;
    }

    if (!next)
    {
        tramInter->next     = NULL;
        tramInter->nextDir  = -1;
        tramInter->nextSide = NONE;
    }
    else
    {
        tramInter->next     = next;
        tramInter->nextDir  = inter->direction(next);
        tramInter->nextSide = nextSide;
    }

    if (!inter
        || (prev && tramInter->prevDir == -1)
        || (next && tramInter->nextDir == -1))
    {
        cerr << "ERROR: tramline is invalid." << endl;
        return false;
    }

    _inters.push_back(tramInter);
    return true;
}

//======================================================================
void TramLine::getIntersections
(vector<const Intersection*>* result_inters)
{
    for (unsigned int i=0; i<_inters.size(); i++)
    {
        result_inters->push_back(_inters[i]->inter);
    }
}

//======================================================================
void TramLine::getNumTramConnectors
(Intersection* inter, int dir,
 int* result_numInLeft,   int* result_numOutLeft,
 int* result_numInCenter, int* result_numOutCenter,
 int* result_numInRight,  int* result_numOutRight)
{
    for (unsigned int i=0; i<_inters.size(); i++)
    {
        if (_inters[i]->inter!=inter)
        {
            continue;
        }
        if (_inters[i]->prevDir == dir)
        {
            switch (_inters[i]->prevSide)
            {
            case LEFT:
                *result_numInLeft = 1;
                break;
            case CENTER:
                *result_numInCenter = 1;
                break;
            case RIGHT:
                *result_numInRight = 1;
                break;
            case UNDEFINED:
            default:
                break;
            }
        }
        if (_inters[i]->nextDir == dir)
        {
            switch (_inters[i]->nextSide)
            {
                // 交差点の外から見る場合は左右が反転
            case LEFT:
                *result_numOutRight = 1;
                break;
            case CENTER:
                *result_numOutCenter = 1;
                break;
            case RIGHT:
                *result_numOutLeft = 1;
                break;
            case UNDEFINED:
            default:
                break;
            }
        }
    }
}

//======================================================================
bool TramLine::isTramLineConnected(Intersection* inter,
                                   int from, int to)
{
    for (unsigned int i=0; i<_inters.size(); i++)
    {
        if (_inters[i]->inter == inter
            && _inters[i]->prevDir == from
            && _inters[i]->nextDir == to)
        {
            return true;
        }
    }
    return false;
}

//======================================================================
void TramLine::print() const
{
    for (unsigned int i=0; i<_inters.size(); i++)
    {
        switch(_inters[i]->prevSide)
        {
        case NONE:
            cout << "*/";
            break;
        case LEFT:
            cout << "L/";
            break;
        case CENTER:
            cout << "C/";
            break;
        case RIGHT:
            cout << "R/";
            break;
        default:
            cout << "?/";
            break;
        }
        cout << _inters[i]->inter->id();
        switch(_inters[i]->nextSide)
        {
        case NONE:
            cout << "/* ";
            break;
        case LEFT:
            cout << "/L-";
            break;
        case CENTER:
            cout << "/C-";
            break;
        case RIGHT:
            cout << "/R-";
            break;
        default:
            cout << "/?-";
            break;
        }
    }
}

#endif //INCLUDE_TRAMS

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "IntersectionPedExt.h"
#include "../Intersection.h"

using namespace std;

//======================================================================
IntersectionPedExt::IntersectionPedExt(Intersection* inter)
{
    _inter = inter;
}

//======================================================================
void IntersectionPedExt::setZebra(Zebra* zebra)
{
    _zebras[zebra->id()] = zebra;
}

//======================================================================
const RMAPZBR* IntersectionPedExt::zebras() const
{
    return &_zebras;
}

//======================================================================
Zebra* IntersectionPedExt::zebra(int dir) const
 {
     CITRMAPZBR it;
     for (it = _zebras.begin(); it != _zebras.end(); it++)
     {
         Zebra *zebra = it->second;

         if (zebra->getDirectionInIntersection() == dir)
         {
             return zebra;
         }
     }

     return NULL;
}

#endif //INCLUDE_PEDESTRIANS

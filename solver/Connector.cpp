/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Connector.h"
#include "Conf.h"
#include <iostream>

using namespace std;

//======================================================================
Connector::Connector(int id, const AmuPoint point)
{
    setIdGlobal(id);
    setPoint(point);
}

//======================================================================
Connector::~Connector(){}

//======================================================================
void Connector::setPoint(const AmuPoint point)
{
    _point = point;
}

//======================================================================
const AmuPoint Connector::point() const
{
    return _point;
}

//======================================================================
void Connector::setIdGlobal(int id)
{
    _idGlobal = id;
}

//======================================================================
int Connector::idGlobal() const
{
    return _idGlobal;
}

//======================================================================
double Connector::x() const
{
    return _point.x();
}

//======================================================================
double Connector::y() const
{
    return _point.y();
}

//======================================================================
double Connector::z() const
{
    return _point.z();
}

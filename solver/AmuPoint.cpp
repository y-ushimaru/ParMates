/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AmuPoint.h"
#include "AmuVector.h"

using namespace std;

//======================================================================
AmuPoint::AmuPoint(double x, double y, double z)
{
  _x = x;
  _y = y;
  _z = z;
  _flag = true;
}

//======================================================================
AmuPoint::AmuPoint(double x, double y, double z, bool f)
{
  _x = x;
  _y = y;
  _z = z;
  _flag = f;
}

//======================================================================
void AmuPoint::setX(double x)
{
  _x = x;
}

//======================================================================
void AmuPoint::setY(double y)
{
  _y = y;
}

//======================================================================
void AmuPoint::setZ(double z)
{
  _z = z;
}

//======================================================================
void AmuPoint::setXYZ(double x, double y, double z)
{
  _x = x;
  _y = y;
  _z = z;
}

//======================================================================
void AmuPoint::setFlag(bool f)
{
  _flag = f;
}

//======================================================================
double AmuPoint::x() const
{
  return _x;
}

//======================================================================
double AmuPoint::y() const
{
  return _y;
}

//======================================================================
double AmuPoint::z() const
{
  return _z;
}

//======================================================================
bool AmuPoint::flag() const
{
  return _flag;
}

//======================================================================
double AmuPoint::distance(const AmuPoint anotherPoint) const
{
  double d = 0;
  d = (anotherPoint.x() - this->x()) * (anotherPoint.x() - this->x())
    + (anotherPoint.y() - this->y()) * (anotherPoint.y() - this->y())
    + (anotherPoint.z() - this->z()) * (anotherPoint.z() - this->z());
  d = sqrt(d);
  return d;
}

//======================================================================
const AmuPoint AmuPoint::z0() const
{
  return AmuPoint(_x, _y, 0, true);
}

//======================================================================
AmuPoint& AmuPoint::operator+=(const AmuVector& vec)
{
  _x += vec.x();
  _y += vec.y();
  return *this;
}

//======================================================================
AmuPoint& AmuPoint::operator-=(const AmuVector& vec)
{
  _x -= vec.x();
  _y -= vec.y();
  return *this;
}

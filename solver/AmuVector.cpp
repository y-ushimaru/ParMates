/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AmuVector.h"
#include "AmuPoint.h"
#include <cmath>
#include <cassert>
#include <iostream>

using namespace std;

//======================================================================
AmuVector::AmuVector(){}

//======================================================================
AmuVector::AmuVector(const AmuPoint& beginPoint,
                     const AmuPoint& endPoint)
{
  _x = endPoint.x() - beginPoint.x();
  _y = endPoint.y() - beginPoint.y();
  _z = endPoint.z() - beginPoint.z();
}

//======================================================================
AmuVector::AmuVector(double x, double y, double z)
{
  _x = x;
  _y = y;
  _z = z;
}

//======================================================================
AmuVector::~AmuVector(){}

//======================================================================
void AmuVector::setX(double x)
{
  _x = x;
}

//======================================================================
void AmuVector::setY(double y)
{
  _y = y;
}

//======================================================================
void AmuVector::setZ(double z)
{
  _z = z;
}

//======================================================================
void AmuVector::setXYZ(double x, double y, double z)
{
  _x = x;
  _y = y;
  _z = z;
}

//======================================================================
void AmuVector::setPoints(const AmuPoint& beginPoint,
                          const AmuPoint& endPoint)
{
  _x = endPoint.x() - beginPoint.x();
  _y = endPoint.y() - beginPoint.y();
  _z = endPoint.z() - beginPoint.z();
}

//======================================================================
double AmuVector::x() const
{
  return _x;
}

//======================================================================
double AmuVector::y() const
{
  return _y;
}

//======================================================================
double AmuVector::z() const
{
  return _z;
}

//======================================================================
void AmuVector::revoltXY(double theta)
{
  // cos -sin
  // sin  cos
  //という行列で一次変換する。
  double tmp_x, tmp_y;
  const double c = cos(theta), s = sin(theta);
  tmp_x = _x * c - _y * s;
  tmp_y = _x * s + _y * c;
  this->setX(tmp_x);
  this->setY(tmp_y);
}

//======================================================================
double AmuVector::calcScalar(const AmuVector& crV) const
{
  double x,y,z;
  x = this->x() * crV.x();
  y = this->y() * crV.y();
  z = this->z() * crV.z();
  return x + y + z;
}

//======================================================================
double AmuVector::calcAngle(const AmuVector& crV) const
{
  // 外積からsinを、内積からcosを算出し、atan2を用いて角度を求める。
  // crVをどれだけ回転させたら自分に重なるかを求める
  double x0 = _x;
  double y0 = _y;
  double z0 = _z;
  double x1 = crV.x();
  double y1 = crV.y();
  double z1 = crV.z();

  // 3次元版はまだ十分にテストしていないので
  // バグが出たらここを疑ってほしい
  double c = calcScalar(crV);
  AmuVector cross(y1*z0-y0*z1, z1*x0-z0*x1, x1*y0-x0*y1);
  double s = cross.size();
  double theta = atan2(s, c);
  // 外積の方向と視線方向(0,0,1)との内積を求める
  double dir = cross.calcScalar(AmuVector(0,0,1));
  if (dir<0)
    theta *= -1;
  return theta;
}

//======================================================================
double AmuVector::size() const
{
  return sqrt(_x * _x + _y * _y + _z * _z);
}

//======================================================================
void AmuVector::normalize()
{
  double s = size();
  _x = _x / s;
  _y = _y / s;
  _z = _z / s;
}

//======================================================================
void AmuVector::invert(const AmuVector& crv)
{
  double angle = calcAngle(crv);
  // XY平面に限定される
  revoltXY(angle*2);
}

//======================================================================
AmuVector& AmuVector::operator+=(const AmuVector& rhs)
{
  _x += rhs._x;
  _y += rhs._y;
  _z += rhs._z;
  return *this;
}

//======================================================================
AmuVector& AmuVector::operator-=(const AmuVector& rhs)
{
  _x -= rhs._x;
  _y -= rhs._y;
  _z -= rhs._z;
  return *this;
}

//======================================================================
AmuVector& AmuVector::operator*=(double k)
{
  _x *= k;
  _y *= k;
  _z *= k;
  return *this;
}

//======================================================================
AmuVector& AmuVector::operator/=(double d)
{
  _x /= d;
  _y /= d;
  _z /= d;
  return *this;
}

//======================================================================
AmuVector& AmuVector::operator=(const AmuVector& rhs)
{
  _x = rhs._x;
  _y = rhs._y;
  _z = rhs._z;
  return *this;
}

//======================================================================
const AmuVector AmuVector::operator-() const
{
  return AmuVector(-_x, -_y, -_z);
}

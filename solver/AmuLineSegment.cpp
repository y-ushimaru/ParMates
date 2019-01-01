/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <cmath>
#include <cassert>
#include "AmuLineSegment.h"
#include "AmuMatrix2D.h"
#include "AmuVector.h"

using namespace std;

//======================================================================
AmuLineSegment::AmuLineSegment()
    : _pointBegin(), _pointEnd(), _mvec(0, 0, 0){}

//======================================================================
AmuLineSegment::AmuLineSegment(const AmuPoint beginPoint, 
                               const AmuPoint endPoint) : _mvec(0, 0, 0)
{
    this->setPoints(beginPoint, endPoint);
}

//======================================================================
AmuLineSegment::~AmuLineSegment(){}

//======================================================================
AmuPoint AmuLineSegment::createInteriorPoint(double d0, double d1) const
{
    // 内分点・外分点の計算
    // エラーの場合には、返値であるAmuPointのフラグを false に設定する
    if(d0 == 0 && d1 == 0)
    {
        // 0:0は拒否する。
        cerr << "WARNING: AmuLineSegment:createInteriorPoint : "
             << "Can't calc. interior point of 0:0" <<endl;
        return AmuPoint(_pointBegin.x()
                        ,_pointBegin.y()
                        ,_pointBegin.z(),false);
    }
    else
    {
        double x = (d1 * _pointBegin.x()
                    + d0 * _pointEnd.x())/ (d0 + d1);
        double y = (d1 * _pointBegin.y()
                    + d0 * _pointEnd.y())/ (d0 + d1);
        double z = (d1 * _pointBegin.z()
                    + d0 * _pointEnd.z())/ (d0 + d1);
        return AmuPoint(x, y, z);
    }
}

//======================================================================
bool AmuLineSegment::createIntersectionPoint
(const AmuLineSegment& crSline, AmuPoint * result_point) const
{
    //線分と線分の交点を考える
    /*
      l1:(x,y,z) = (a0,b0,c0) + t1*(v1x,v1y,v1z)
      l2:(x,y,z) = (p0,q0,r0) + t2*(v2x,v2y,v2z)
      という2つの直線の交点を求めることは
      t1またはt2というパラメータを求めることと同値である．
      ただし，|t1|=<1かつ|t2|=<1であり，
      (a0,b0,c0),(p0,q0,r0)はそれぞれの線分の
      始点である。この2式から(x,y,z)を消去すると

      |a0-p0|   |-v1x v2x||t1|
      |b0-q0| = |-v1y v2y||t2|
      |c0-r0|   |-v1z v2z|

      となる．あとは逆行列の存在の有無で場合分けをする．
    */
    bool result = false;
    double a0 = _pointBegin.x();
    double b0 = _pointBegin.y();
    double c0 = _pointBegin.z();
    double a1 = _pointEnd.x();
    double b1 = _pointEnd.y();
    double c1 = _pointEnd.z();
    double v1x = a1 - a0;
    double v1y = b1 - b0;
    double v1z = c1 - c0;

    double p0 = crSline.pointBegin().x();
    double q0 = crSline.pointBegin().y();
    double r0 = crSline.pointBegin().z();
    double p1 = crSline.pointEnd().x();
    double q1 = crSline.pointEnd().y();
    double r1 = crSline.pointEnd().z();
    double v2x = p1 - p0;
    double v2y = q1 - q0;
    double v2z = r1 - r0;

    AmuMatrix2D mxy(-v1x, v2x, -v1y, v2y);
    AmuMatrix2D myz(-v1y, v2y, -v1z, v2z);
    AmuMatrix2D mzx(-v1z, v2z, -v2x, v2x);

    AmuVector tmp_vector(a0 - p0, b0 - q0, c0 - r0);
    double t1=0,t2=0;
  
    if(mxy.inverse())
    {
        t1 = mxy.getItem(0,0)*tmp_vector.x()
            +mxy.getItem(0,1)*tmp_vector.y();
        t2 = mxy.getItem(1,0)*tmp_vector.x()
            +mxy.getItem(1,1)*tmp_vector.y();
        if(0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1)
        {
            double z1=c0+t1*v1z, z2=r0+t2*v2z;
            if( abs(z1 - z2) < 10e-6)
            {
                //交点は存在する。
                result_point->setX(a0 + t1 * v1x);
                result_point->setY(b0 + t1 * v1y);
                result_point->setZ( (z1+z2)/2 );
                return result = true;
            }
        }
    }
    if(myz.inverse())
    {
        t1 = myz.getItem(0,0)*tmp_vector.y()
            +myz.getItem(0,1)*tmp_vector.z();
        t2 = myz.getItem(1,0)*tmp_vector.y()
            +myz.getItem(1,1)*tmp_vector.z();
        if(0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1)
        {
            double x1=a0+t1*v1x, x2=p0+t2*v2x;
            if( abs(x1 - x2) < 10e-6)
            {
                //交点は存在する。
                result_point->setX( (x1+x2)/2 );
                result_point->setY(b0 + t1 * v1y);
                result_point->setZ(c0 + t1 * v1z);
                return result = true;
            }
        }
    }
    if(mzx.inverse())
    {
        t1 = mzx.getItem(0,0)*tmp_vector.z()
            +mzx.getItem(0,1)*tmp_vector.x();
        t2 = mzx.getItem(1,0)*tmp_vector.z()
            +mzx.getItem(1,1)*tmp_vector.x();
        if(0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1)
        {
            double y1=b0+t1*v1y, y2=q0+t2*v2y;
            if( abs(y1 - y2) < 10e-6)
            {
                //交点は存在する。
                result_point->setX(a0 + t1 * v1x);
                result_point->setY( (y1+y2)/2 );
                result_point->setZ(c0 + t1 * v1z);
                return result = true;
            }
        }
    }
    if(mxy.delta() == 0)
    {
        if(myz.delta() == 0)
        {
            if(mzx.delta() == 0)
            {
                //交点は存在しない
                return result = false;
            }
        }
    }
    return result;
}

//======================================================================
bool AmuLineSegment::isLeftSide(const AmuPoint& point) const
{
    AmuVector v(_pointBegin, point);
    double s = (-_mvec.y() * v.x() + _mvec.x() * v.y())
        / (_mvec.size() * v.size());
    return (s > 0); // とりあえず0自体は含まない
}

//======================================================================
double AmuLineSegment::calcFootOfPerpendicularLength
(const AmuPoint& point) const
{
    AmuVector v(_pointBegin, point);
    double l = (_mvec.x() * v.x() + _mvec.y() * v.y()) / _mvec.size();
    return l;
}

//======================================================================
const AmuPoint AmuLineSegment::calcFootOfPerpendicular
(const AmuPoint& point) const
{
    const double l = calcFootOfPerpendicularLength(point);
    return createInteriorPoint(l, length() - l);
}

//======================================================================
double AmuLineSegment::calcNearestLength(const AmuPoint& point) const
{
    const double lmax = length();
    double l = calcFootOfPerpendicularLength(point);
    if (l < 0)
    {
        return 0;
    }
    else if (l > lmax)
    {
        return lmax;
    }
    else
    {
        return l;
    }
}

//======================================================================
const AmuPoint AmuLineSegment::calcNearestPoint
(const AmuPoint& point) const
{
    const double l = calcFootOfPerpendicularLength(point);
    if (l < 0)
    {
        return pointBegin();
    }
    else if (l > length())
    {
        return pointEnd();
    }
    else
    {
        return createInteriorPoint(l, length() - l);
    }
}

//======================================================================
double AmuLineSegment::distance(const AmuPoint& point) const
{
    return point.distance(calcNearestPoint(point));
}

//======================================================================
void AmuLineSegment::setPoints(const AmuPoint point_begin,
                               const AmuPoint point_end)
{
    _setVectorFromPoints(point_begin, point_end);
    _pointBegin = point_begin;
    _pointEnd = point_end;
}

//======================================================================
const AmuPoint AmuLineSegment::pointBegin() const
{
    return _pointBegin;
}

//======================================================================
const AmuPoint AmuLineSegment::pointEnd() const
{
    return _pointEnd;
}


//======================================================================
double AmuLineSegment::length() const
{
    double l = 0;
    l = (_pointEnd.x() - _pointBegin.x())
        * (_pointEnd.x() - _pointBegin.x())
        + (_pointEnd.y() - _pointBegin.y())
        * (_pointEnd.y() - _pointBegin.y())
        + (_pointEnd.z() - _pointBegin.z())
        * (_pointEnd.z() - _pointBegin.z());
    l = sqrt(l);
    return l;
}

//======================================================================
void  AmuLineSegment::_setVectorFromPoints(const AmuPoint pt0,
                                           const AmuPoint pt1)
{
    _mvec.setX(pt1.x() - pt0.x());
    _mvec.setY(pt1.y() - pt0.y());
    _mvec.setZ(pt1.z() - pt0.z());
}

//======================================================================
const AmuVector AmuLineSegment::directionVector() const
{
    return _mvec;
}

//======================================================================
void AmuLineSegment::_setVector(const AmuVector& cr_mvec)
{
    _mvec.setX(cr_mvec.x());
    _mvec.setY(cr_mvec.y());
    _mvec.setZ(cr_mvec.z());
}

//======================================================================
const AmuLineSegment AmuLineSegment::reversal() const
{
    return AmuLineSegment(_pointEnd, _pointBegin);
}

//======================================================================
const AmuLineSegment AmuLineSegment::z0() const
{
    return AmuLineSegment(_pointBegin.z0(), _pointEnd.z0());
} 

//======================================================================
bool AmuLineSegment::checkIntersectionZ0(const AmuLineSegment& crSline,
                                          AmuPoint *crossPoint)
{
    AmuLineSegment line1Z0 = z0();
    AmuLineSegment line2Z0 = crSline.z0();
    AmuPoint crossPointZ0;

    if (!line1Z0.createIntersectionPoint(line2Z0, &crossPointZ0))
    {
        return false;
    }
    *crossPoint = createInteriorPoint
        (crossPointZ0.distance(line1Z0._pointBegin),
         crossPointZ0.distance(line1Z0._pointEnd));
    return true;
}


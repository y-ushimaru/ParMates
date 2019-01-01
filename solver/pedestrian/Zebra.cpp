/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "Zebra.h"
#include "Pedestrian.h"
#include "VehiclePedExt.h"
#include "../Vehicle.h"
#include "../SubIntersection.h"
#include "../TimeManager.h"
#include "../AmuMatrix.h"
#include "../AmuMatrix2D.h"
#include "../AmuPoint.h"
#include "../AmuVector.h"
#include <algorithm>
#include <cassert>
#include <cfloat>

using namespace std;


//######################################################################
/// エージェントのソートに用いるクラス
class PedestrianLess
{
public:
    PedestrianLess (double transMat[][2])
    {
        _transMat[0][0] = transMat[0][0];
        _transMat[0][1] = transMat[0][1];
        _transMat[1][0] = transMat[1][0];
        _transMat[1][1] = transMat[1][1];
    }

    bool operator()(const Pedestrian* rl, const Pedestrian* rr) const
    {
        double xl = _transMat[0][0] * rl->x() + _transMat[0][1] * rl->y();
        double xr = _transMat[0][0] * rr->x() + _transMat[0][1] * rr->y();

        return xl < xr;
    }
private:
    AmuPoint _origin;
    double _transMat[2][2];
};

//######################################################################
Zebra::Zebra(const std::string& id)
    : SubIntersection(id, SUBROAD_TYPE_CROSSWALK)
{
    _transMat[0][0] = _transMat[1][1] = 1.0;
    _transMat[1][0] = _transMat[0][1] = 0.0;

    // 歩行者発生率のデフォルト値
    _pedGenProbability[0] = _pedGenProbability[1]
        = 100.0 * TimeManager::unit()
        / 60.0 / 60.0 / 1000.0; // [ped/h]->[ped/step]
}

//======================================================================
Zebra::~Zebra()
{
}

//======================================================================
int Zebra::getDirectionInIntersection ()
{
    return _dirInIntersection;
}

//======================================================================
void Zebra::initialize (int dirInIntersection)
{
    _dirInIntersection = dirInIntersection;

    _generateTransposeMatrix();
    _generateSideLineFactors();
}

//======================================================================
void Zebra::_generateSideLineFactors()
{
    assert(numVertices() >= 4);

    const AmuPoint v0 = vertex(0);
    const AmuPoint v1 = vertex(1);
    const AmuPoint v2 = vertex(2);
    const AmuPoint v3 = vertex(3);

    _A01 = v0.y() - v1.y();
    _B01 = v1.x() - v0.x();
    _C01 = v0.x()*v1.y() - v1.x()*v0.y();

    _A23 = v2.y() - v3.y();
    _B23 = v3.x() - v2.x();
    _C23 = v2.x()*v3.y() - v3.x()*v2.y();
}

//======================================================================
void Zebra::_generateTransposeMatrix()
{
    AmuPoint _origin = vertex(2);
    AmuVector udir(_origin, vertex(3));
    AmuVector vdir(_origin, vertex(1));
    AmuMatrix2D transMat (udir.x(), vdir.x(), udir.y(), vdir.y());
    transMat.inverse();

    _transMat[0][0] = transMat.getItem(0, 0);
    _transMat[0][1] = transMat.getItem(0, 1);
    _transMat[1][0] = transMat.getItem(1, 0);
    _transMat[1][1] = transMat.getItem(1, 1);
}

//======================================================================
bool Zebra::putPedestrian(Pedestrian* pds)
{
    bool flag = false;

    vector<Pedestrian*>& rvec = _tmpPedestrians;
    vector<Pedestrian*>::iterator where
        = find(rvec.begin(), rvec.end(), pds);

    if (where == rvec.end())
    {
        // 同じpdsが見付からなかったら追加する
        rvec.push_back(pds);

#ifdef PDS_DEBUG
        cerr << "z[" << id() << "](" << _tmpPedestrians.size()
             << ") puts pds[" << pds->id() << "]$(" << pds << ")" << endl;
#endif
        flag = true;
    }
    else
    {
        // this pedestrian already exists here!!
#ifdef PDS_DEBUG
        cerr << "Zebra#putPedestrian(): pds=[" << pds->id()
             << "]$(" << pds << ")@" << *(pds->point()) << endl;
        cerr << "      zebra[" << id()
             << "]._tmpPededestrians.size()=" << _tmpPedestrians.size()
             << endl;
        for (unsigned int i = 0; i < _tmpPedestrians.size(); i++)
        {
            Pedestrian* tmpPds = _tmpPedestrians[i];
            cerr << "         [" << i << "] pds[" << tmpPds->id()
                 << "]$(" << tmpPds << ")@" << *(tmpPds->point())
                 << " : isZebra=" << tmpPds->isOnZebra() << endl;
        }
#endif
        assert(0);
    }

    return flag;
}

//======================================================================
vector<Pedestrian*>* Zebra::pedestrians()
{
    return &_pedestrians;
}

//======================================================================
void Zebra::renewPedestrianLine()
{
#ifdef PDS_DEBUG
    cerr << "renewPedestrianLine(): z[" << id() << "] has "
         << _tmpPedestrians.size() << " tmp_pedestrian(s)." << endl;
#endif

    //_tmpPedestrians.clear();
    //clean up _tmpPedestrians
    vector<Pedestrian*>::iterator it = _tmpPedestrians.begin();
    while (it != _tmpPedestrians.end())
    {
        if (!(*it)->isOnZebra())
        {
            /*
             * 前のステップのObjManager#deleteFinishPedestrians()で
             * deleteすると今ステップにこのループ内でのisOnZebra()に
             * 失敗する
             */
            delete *it;

            it = _tmpPedestrians.erase(it);
        }
        else
        {
            it++;
        }
    }

    _pedestrians.clear();
    _pedestrians.resize(_tmpPedestrians.size());
    assert(_pedestrians.size()==_tmpPedestrians.size());

    sort(_tmpPedestrians.begin(), _tmpPedestrians.end(),
         PedestrianLess (_transMat));

    for (unsigned int i = 0; i<_tmpPedestrians.size(); i++)
    {
        _pedestrians[i] = _tmpPedestrians[i];
    }

#ifdef PDS_DEBUG
    cerr << "renewPedestrianLine(): z[" << id() << "] has "
         << _pedestrians.size() << " pedestrian(s)." << endl;
    for (unsigned int i = 0; i < _pedestrians.size(); i++)
    {
        Pedestrian* pds = _pedestrians[i];
        string id = "";
        if (pds->hasId())
        {
            id = pds->id();
        }
        cerr << "   [" << i << "] p[" << id << "]@"
             << *(pds->point()) << "%" << *(pds->velocity())
             << ": onZebra="<<pds->isOnZebra() << endl;
    }
#endif
}

//======================================================================
void Zebra::clearPedestrian()
{
    _pedestrians.clear();
    _tmpPedestrians.clear();
    _waitingPedestrians[0].clear();
    _waitingPedestrians[1].clear();
}

//======================================================================
void Zebra::putWaitingPds(Pedestrian* pds, int i)
{
    vector<Pedestrian*>& rvec = _waitingPedestrians[i];
    vector<Pedestrian*>::iterator where
        = find(rvec.begin(), rvec.end(), pds);

    if (where != rvec.end())
    {
        // double put... why?
        cerr << "z[" << id() << "](" << i << ":size=" << rvec.size()
             << ") already has pds%(" << pds << endl;
        for (unsigned int iw = 0; iw < rvec.size(); iw++)
        {
            Pedestrian* wp = rvec[iw];
            cerr << "     [" << iw << "] pds%(" << wp << ")" << endl; 
        }
    }
    assert(where == rvec.end());
  
    _waitingPedestrians[i].push_back(pds);
}

//======================================================================
Pedestrian* Zebra::popPdsFromWaiting(int i)
{
    int size = _waitingPedestrians[i].size();
    Pedestrian* tmpPds = NULL;

    if (size > 0) {
        //tmpPds = _waitingPedestrians[i][size - 1];
        tmpPds = _waitingPedestrians[i].back();
        _waitingPedestrians[i].pop_back();
    }

    return tmpPds;
}

//======================================================================
static bool isCrossBorder(double A, double B, double C,
                          const AmuPoint* p0, const AmuPoint* p1)
{
    double side0 = A * p0->x() + B * p0->y() + C;
    double side1 = A * p1->x() + B * p1->y() + C;

    return !((side0<0.0 && side1<0.0) || (side0>=0.0 && side1>=0.0));
}

//======================================================================
static AmuPoint calcNearestPoint(double A, double B, double C,
                                 const AmuPoint* p1)
{
    AmuPoint pc;

    // An*x + Bn*y+Cn=0 : Ax+By+C=0と直交してp1を通る直線
    double An = -B;
    double Bn = A;
    double Cn = B * p1->x() - A * p1->y();
    double Anb_BnA = An * B - Bn * A;

    // 交点の座標を求める
    pc.setX ((-B*Cn + Bn*C) / Anb_BnA);
    pc.setY (( A*Cn - An*C) / Anb_BnA);

    return pc;
}

//======================================================================
const AmuPoint Zebra::nextPedestrianPoint(const Pedestrian* pds,
                                          bool* result_isReflected) const
{
    const AmuPoint* p0 = pds->point();
    const AmuVector* v0 = pds->velocity();
    AmuPoint p1 = *p0 + *v0;
    AmuPoint pc = p1;
    AmuPoint p2 = p1;

    *result_isReflected = false;
    if (isCrossBorder (_A01, _B01, _C01, p0, &p1))
    {
        pc = calcNearestPoint (_A01, _B01, _C01, &p1);
        *result_isReflected = true;
    }
    else if (isCrossBorder (_A23, _B23, _C23, p0, &p1))
    {
        pc = calcNearestPoint (_A23, _B23, _C23, &p1);
        *result_isReflected = true;
    }

    if (*result_isReflected)
    {
        p2.setXYZ(2.0*pc.x()-p1.x(), 2.0*pc.y()-p1.y(), 2.0*pc.z()-p1.z());
    }

    return p2;
}

//======================================================================
Pedestrian* Zebra::searchNearestPedestrian(Pedestrian* pds) const
 {
    Pedestrian* nearestPds = NULL;
    double distance = DBL_MAX;
    //double viewLength = pds->viewLength();
    //double ps = pds->psRadius();
    const AmuVector* velocity = pds->velocity();

    if (_pedestrians.size() == 1) {
        /* 自分以外にいない場合 */
        return NULL;
    }

    for (vector<Pedestrian*>::const_iterator it = _pedestrians.begin();
         it != _pedestrians.end();
         it++)
    {
        Pedestrian* tmpPds = *it;

        if (tmpPds == pds || !tmpPds->isOnZebra())
        {
            // 自分自身 or 横断歩道内にいない -> 無視
            continue;
        }

        double tmpDist = distanceBetween (pds, tmpPds);
        if (tmpDist < 0)
        {
            // 後ろにいるので無視
            continue;
        }
        if (tmpDist > distance)
        {
            // 最接近他者ではない
            continue;
        }

        AmuVector relPos;
        relPos.setPoints(*(tmpPds->point()), *(pds->point()));
        double angle = relPos.calcAngle(*velocity);
        if (angle < pds->viewAngle()/2.0)
        {
            // 視界の中にいる
            nearestPds = tmpPds;
            distance = tmpDist;
        }
    }

#ifdef PDS_DEBUG
    if (nearestPds != NULL) {
        cerr << "z[" << id() << "] : pds[" << pds->id()
             << "] found nearest pds[" << nearestPds->id()
             << "]" << endl;
        cerr << "        distance = " << distance << endl;
    }
#endif

    return nearestPds;
}

//======================================================================
double Zebra::distanceBetween(Pedestrian* pds1, Pedestrian* pds2) const
 {
    if (pds1 == NULL || pds2 == NULL)
    {
        cerr << "Zebra::distanceBetween() received NULL-Pointer."
             << endl;
        exit(1);
    }

    double distance = -1;

    const AmuPoint* p1 = pds1->point();
    const AmuPoint* p2 = pds2->point();

    distance = p1->distance(*p2);

    /*
     * AmuPoint#distance()では前後関係を無視したただの距離を出してしまう。
     * そこで、自分の進行方向と相対位置ベクトルの内積の符号を付与することで、
     * 向きを考慮した値とする。
     */
    double xd = pds1->velocity()->x();
    double yd = pds1->velocity()->y();
    if (pds1->isAlmostStopped ())
    {
        xd = pds1->directionVector()->x();
        yd = pds1->directionVector()->y();
    }

    double xr = pds2->x() - pds1->x();
    double yr = pds2->y() - pds1->y();
    if (xd*xr + yd*yr < 0)
    {
        distance *= -1;
    }

    return distance;
}

//======================================================================
bool Zebra::canPass(Vehicle* vehicle) const
{
    const  vector<Pedestrian*>& rvec = _pedestrians;
    vector<Pedestrian*>::const_iterator who = rvec.begin();

#ifdef PDS_DEBUG
    cerr << "Zebra#canPass(): z[" << this->id() << "] has "
         << rvec.size() << " pedestrian(s)." << endl;
#endif

    double time = ((double)TimeManager::unit ())/1000.0;

    // check all pedestrians are not cross with the object
    // (x,y, length, width)
    // in from now till now+time.
    while (who != rvec.end())
    {
        Pedestrian* pds = (*who);

        if (vehicle->pedExt()->isCrossingWith(pds, time))
        {
            return true;
        }

        who++;
    }
    return true;
}

//======================================================================
AmuPoint Zebra::initialPositionOf (int dir, double baselinePosition)
{
    /// @attention: 横断歩道が四角形であることを仮定している。
    AmuPoint start_point;

    AmuPoint point[2][2] = {
        {vertex(1), vertex(2)},
        {vertex(3), vertex(0)}
    };

    start_point
        = point[dir][0] * baselinePosition
        + point[dir][1] * (1.0-baselinePosition);

    return start_point;
}

//======================================================================
AmuVector Zebra::initialVelocityOf (int dir, double speedRatio)
{
    AmuVector velocity;
    // TODO : how to define?
    double speed = 1.0 * speedRatio;

    AmuPoint point[2][2] =
        {
            {vertex(1), vertex(2)},
            {vertex(3), vertex(0)}
        };

    AmuVector baseline(point[dir][0], point[dir][1]);
    baseline.normalize();
    baseline *= speed;
    velocity.setX(-baseline.y());
    velocity.setY(baseline.x());
    velocity.setZ((point[dir][0].z() + point[dir][1].z())/2.0);

    return velocity;
}

//======================================================================
double Zebra::pedGenProbability(bool isUp) const
{
    if (isUp)
    {
        return _pedGenProbability[1];
    }
    else
    {
        return _pedGenProbability[0];
    }
}

//======================================================================
void Zebra::setPedGenProbability(bool isUp, double p)
{
    if (isUp)
    {
        _pedGenProbability[1] = p;
    }
    else
    {
        _pedGenProbability[0] = p;
    }
}

#endif //INCLUDE_PEDESTRIANS

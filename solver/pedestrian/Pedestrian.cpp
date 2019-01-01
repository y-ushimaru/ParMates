/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "Pedestrian.h"
#include "VehiclePedExt.h"
#include "Zebra.h"
#include "../SignalColor.h"
#include "../Vehicle.h"
#include "../Lane.h"
#include "../TimeManager.h"
#include "../Random.h"
#include <cassert>
#include <cmath>

using namespace std;

/*
 * 歩行者から自動車認知->判断を行う場合はマクロを有効にすること。
 * ただし、判断部分(changeVelocityFor(Vehicle*))が未実装なので
 * 実装してから有効にすること
 */
// #define ENABLE_PEDESTRIAN_SEARCH_VEHICLE

//======================================================================
Pedestrian::Pedestrian()
{
    _recogStatus = RecogStatus_NoChange;
    _foundVehicle = NULL;
    _foundPedestrian = NULL;

    _radius = 0.5;
    _personal = 1.2;

    _direction = 0;
    _directionVector.setXYZ (0,0,0);
    _velocity.setXYZ (0.0, 0.0, 0.0);
    _accel = 1.5;
    _rnd = Random::randomGenerator();

    // TODO : re-implement me!
    _cV = 1.0+static_cast<double>(rand()%41-20)/100.0; // 0.8 - 1.2

    _viewLength = 2.5; // 単位：(m)
    //_viewAngle = 160.0*0.017453292; // 単位：(rad)
    _viewAngle = 160.0 * M_PI / 180.0;

    _intersection = NULL;
    _zebra = NULL;
    _onZebra = false;

    _hasId = false;
}

//======================================================================
Pedestrian::~Pedestrian()
{
    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
void Pedestrian::setId(const string& id)
{
    _hasId = true;
    _id = id;
}

//======================================================================
RandomGenerator* Pedestrian::randomGenerator()
{
    assert(_rnd);
    return _rnd;
}

//======================================================================
bool Pedestrian::hasId() const
{
    return _hasId;
}

//======================================================================
const string& Pedestrian::id() const
{
    return _id;
}

//======================================================================
double Pedestrian::radius() const
{
    return _radius;
}

//======================================================================
double Pedestrian::psRadius() const
{
    return _radius*_personal;
}

//======================================================================
double Pedestrian::viewLength() const
{
    return _viewLength;
}

//======================================================================
double Pedestrian::viewAngle() const
{
    return _viewAngle;
}

//======================================================================
bool Pedestrian::isOnZebra() const
{
    return _onZebra;
}

//======================================================================
void Pedestrian::setOnZebra()
{
    _onZebra = true;
}

//======================================================================
void Pedestrian::setDirection (int direction)
{
     //0:[03]->[12], 1:[12]->[03]
     assert(direction == 0 || direction == 1);
    _direction = direction;
}

//======================================================================
int Pedestrian::direction() const
{
    return _direction;
}

//======================================================================
const AmuVector* Pedestrian::velocity() const
{
    return &_velocity;
}

//======================================================================
void Pedestrian::setVelocity (const AmuVector &velocity)
{
    _velocity = velocity;

    // 初期方向を正規化して取っておく。
    _directionVector.setX (_velocity.x());
    _directionVector.setY (_velocity.y());
    _directionVector.setZ (_velocity.z());
    //_directionVector.normalize();
}

//======================================================================
double Pedestrian::x() const
{
    return _point.x();
}

//======================================================================
double Pedestrian::y() const
{
    return _point.y();
}

//======================================================================
double Pedestrian::z() const
{
    return _point.z();
}

//======================================================================
const AmuPoint* Pedestrian::point() const
{
    return &_point;
}

//======================================================================
void Pedestrian::setPoint(double x, double y, double z)
{
    _point.setX(x);
    _point.setY(y);
    _point.setZ(z);
}

//======================================================================
void Pedestrian::setPoint(const AmuPoint* p)
{
    double x = p->x();
    double y = p->y();
    double z = p->z();
    setPoint(x, y, z);
}

//======================================================================
void Pedestrian::setIntersection(const Intersection* inter)
{
    _intersection = inter;
}

//======================================================================
const Intersection* Pedestrian::intersection() const
{
    return _intersection;
}

//======================================================================
void Pedestrian::setZebra(Zebra* zebra)
{
    _zebra = zebra;
}

//======================================================================
Zebra* Pedestrian::zebra() const
{
    return _zebra;
}

//======================================================================
void Pedestrian::setStartTime (ulint startTime)
{
    _startTime = startTime;
}

//======================================================================
ulint Pedestrian::startTime() const
{
    return _startTime;
}

//======================================================================
void Pedestrian::recognize ()
{
    // 状態初期化
    _recogStatus = RecogStatus_NoChange;
    _foundVehicle = NULL;
    _foundPedestrian = NULL;

#ifdef PDS_DEBUG
    if (isAlmostStopped ())
    {
        cerr << "pds[" << id()
             << "]@" << _point
             << ",v" << _velocity
             << " recognize: onZebra=" << _onZebra
             << ", almostStopped=" << isAlmostStopped() << endl;
    }
#endif

    // 横断歩道上にいないので判断する必要がない
    if (!_onZebra)
    {
        return;
    }

#ifdef ENABLE_PEDESTRIAN_SEARCH_VEHICLE
    // 前方に自動車がいるかどうか
    Vehicle* vehicle = searchVehicle();
    if (vehicle != NULL)
    {
        /* 前方に自動車が見つかった場合、停止 */
        _recogStatus = RecogStatus_VehicleFound;
        _foundVehicle = vehicle;
        return;
    }
#endif

    // 前方に他の歩行者がいるかどうか
    Pedestrian* pds = searchPedestrian();
    if (pds != NULL)
    {
        /* 前方に歩行者が見つかった場合、避けるかどうかする */
        _recogStatus = RecogStatus_PedestrianFound;
        _foundPedestrian = pds;
        return;
    }
}

//======================================================================
int Pedestrian::recognitionStatus() const
{
    return _recogStatus;
}

//======================================================================
double Pedestrian::maxVelocity()
{
    double vMax = TimeManager::unit()*1.3/1000*_cV;
    return vMax;
}

//======================================================================
void Pedestrian::determineToChangeVelocity()
{
    if (_recogStatus == RecogStatus_PedestrianFound)
    {
        // 近傍歩行者が存在する
        changeVelocityFor(_foundPedestrian);
    }
    else if (_recogStatus == RecogStatus_VehicleFound)
    {
        // 近傍自動車が存在する
#ifdef ENABLE_PEDESTRIAN_SEARCH_VEHICLE
        changeVelocityFor(_foundVehicle);
#endif
    }
    else
    {
        // 近くに何もいないので自由歩行

        /*
         * 旧MATESでは、readyToWalk()内で近傍歩行者がいない場合にのみ
         * 歩道内に収まるような処理を行っていたが、
         * 移植にあたってはみ出し禁止処理をwalk()に移動したので、
         * ここでは単に速度制御のみ行う
         */
        double vMax = maxVelocity();

        if (_zebra->signal()->walkerColor(_direction)
            != SignalColor::blue())
        {
            vMax *= _accel;
        }
        _recoverDirection (); // 止まっていた時は初期速度で再初期化
        _velocity.normalize();
        _velocity *= vMax;
    }

    return;
}

//======================================================================
void Pedestrian::changeVelocityFor(const Vehicle* foundVehicle)
{
    // 旧MATESに、これに相当する実装が存在しないため、
    // 未実装のままとしている。
}

//======================================================================
double Pedestrian::calcSideDistance(Pedestrian* pds)
 {
    // Ax+By+C=0 : 進行方向ベクトルの向きで現在位置を通る直線
    double A, B, C;
    double distance;

    double px, py;

    if (isAlmostStopped ())
    {
        A = _directionVector.y();
        B = -_directionVector.x();
    }
    else
    {
        A = _velocity.y();
        B = -_velocity.x();
    }
    C = - (A*_point.x() + B*_point.y());

    px = pds->point()->x();
    py = pds->point()->y();
    distance = (A*px + B*py + C) / sqrt (A*A + B*B);

#ifdef PDS_DEBUG
    if (std::isnan(distance))
    {
        cerr << "pds[" << id()
             << "]@" << _point
             <<" side distance is nan: target=[" << pds->id()
             << "]@" << pds->point() << endl;
        cerr << "          -> A=" << A
             << ", B=" << B 
             << ", C=" << C << endl;
    }
#endif

    assert(!std::isnan(distance));

    return distance;
}

//======================================================================
double Pedestrian::calcFrontDistance(Pedestrian* pds)
{
    // Ax+By+C=0 : 進行方向の右手方向で現在位置を通る直線
    double A, B, C;
    double distance;

    double px, py;

    A = _velocity.x();
    B = _velocity.y();
    C = - (A*_point.x() + B*_point.y());

    px = pds->point()->x();
    py = pds->point()->y();
    distance = (A*px + B*py + C) / sqrt (A*A + B*B);

    return distance;
}

//======================================================================
void Pedestrian::_recoverDirection()
{
    if (isAlmostStopped ())
    {

#ifdef PDS_DEBUG
        cerr << "pds[" << id() << "]@" << _point
             << " : _velocity=" << _velocity
             << " is too slow to recover initial vector "
             << _directionVector << "..." << endl;
#endif

        // 遅すぎるので、初期方向に初期化
        _velocity.setX(_directionVector.x());
        _velocity.setY(_directionVector.y());
        _velocity.setZ(_directionVector.z());
    }
}

//======================================================================
bool Pedestrian::isAlmostStopped() const
{
    return _velocity.size() < 1e-6;
}

//======================================================================
const AmuVector* Pedestrian::directionVector() const
{
    return &_directionVector;
}

//======================================================================
void Pedestrian::changeVelocityFor(const Pedestrian* foundPedestrian)
{
    double vMax = maxVelocity ();
    double psRad = psRadius ();

    if (_zebra->signal()->walkerColor(_direction)
        != SignalColor::blue())
    {
        vMax *= _accel;
    }

    // 最接近他者がいる場合
    Pedestrian* nearestPds = _foundPedestrian;

    if (nearestPds->direction()==_direction)
    {
        // ******** 同方向 *******
        double distance = _zebra->distanceBetween(this, nearestPds);
        assert(!std::isnan(distance));

        if ( distance < 0 )
        {
            // 後ろにいるので無視 -> 何もしない
            _recoverDirection ();
            _velocity.normalize();
            _velocity *= vMax;
        }
        else if ( distance < psRad*2 )
        {
            // 近すぎる(PersonalSpace内にいる)場合 → 一旦停止
            _velocity.setXYZ(0.0, 0.0, 0.0);
            //_sleepTime = 1.5*1000;
        }
        else
        {
            // ある程度離れている
            if ( fabs(_velocity.size())
                 > fabs(nearestPds->velocity()->size()) )
            {
                // 自分の方が速い → 追い越し
                double sideDistance = calcSideDistance (nearestPds);
                assert(!std::isnan(sideDistance));

                // 追い越すために必要な間隔
                double avoidDis = psRad*2 - fabs (sideDistance);
                assert(!std::isnan(avoidDis));
                if (avoidDis <= 0)
                {
                    // 十分離れている -> そのまま直進
                    /*
                     * (ただし、速さだけは更新する
                     */
                    //double v = _velocity.size();
                    _recoverDirection ();
                    _velocity.normalize();
                    _velocity *= vMax;
                    // avoidDis = 0.0;
                }
                else
                {
                    // 近いので左右どちらかに避けつつ前進
                    if (sideDistance > 0)
                    {
                        // 自分の方が右なら右に避ける
                        // avoidDisは正のままで良い
                    }
                    else
                    {
                        // 自分の方が左なら左に避ける
                        // avoidDisの符号を反転
                        avoidDis = -1*avoidDis;
                    }
                    // 横方向の移動速度を計算
                    _recoverDirection ();
                    double frontDistance
                        = calcFrontDistance (nearestPds);
                    assert(!std::isnan(frontDistance));
                    double errorVelocity
                        = avoidDis
                        / fabs (frontDistance / _velocity.size());
                    assert(!std::isnan(errorVelocity));

                    if (fabs (errorVelocity) > vMax)
                    {
                        /**
                         * @todo frontVelocityがnanにならないように、
                         * errorVelocityを少しだけvMaxより小さくしている。
                         * どのくらい小さくすればよいか調整が必要。
                         */
                        errorVelocity
                            = vMax * avoidDis / fabs (avoidDis) * 0.9;
                        assert(!std::isnan(frontDistance));
                    }
                    // 元の進行方向の速度は、横方向に移動する分遅くなる
                    double frontVelocity
                        = sqrt (vMax * vMax
                                - errorVelocity * errorVelocity);

                    // 元の進行方向の成分を修正
                    AmuVector fDir = _velocity;
                    fDir.normalize();
                    fDir *= frontVelocity;
                    assert(!std::isnan(fDir.x()));
                    assert(!std::isnan(fDir.y()));
                    assert(!std::isnan(fDir.z()));

                    // 横方向成分を修正
                    AmuVector eDir = fDir;
                    eDir.revoltXY(-0.5*M_PI);
                    eDir *= errorVelocity;
                    assert(!std::isnan(eDir.x()));
                    assert(!std::isnan(eDir.y()));
                    assert(!std::isnan(eDir.z()));

                    // 元の進行方向+横方向=新しい進行方向
                    _velocity.setX (fDir.x()+eDir.x());
                    _velocity.setY (fDir.y()+eDir.y());
                    _velocity.setZ (fDir.z()+eDir.z());

                    assert(!std::isnan(_velocity.x()));
                    assert(!std::isnan(_velocity.y()));
                    assert(!std::isnan(_velocity.z()));

                }
            }
            else
            {
                // 自分の方が遅い → 追従
                double sideDistance = calcSideDistance (nearestPds);
                assert(!std::isnan(sideDistance));

                if (fabs (sideDistance) < psRad)
                {
                    // 既に追従している場合 -> 横方向に移動する必要はない
                    /**
                     * そのまま直進 (速さだけ調整)
                     */
                    _recoverDirection ();
                    _velocity.normalize();
                    _velocity *= vMax;
                }
                else
                {
                    // 追従する時に保つ間隔
                    double followDis;

                    if (sideDistance < 0)
                    {
                        /*
                         * 自分が外にいる場合、外側を追従
                         * -> 内側に移動する必要がある
                         */
                        followDis = psRad + sideDistance; //負
                    }
                    else
                    {
                        /* 
                         * 自分が中にいる場合、内側を追従
                         *  -> 外側に移動する必要がある
                         */
                        followDis =	sideDistance - psRad; //正
                    }

                    _recoverDirection();

                    // 横方向の移動速度を計算
                    double errorVelocity = followDis / 1000.0;
                    if (fabs (errorVelocity) > vMax*0.5)
                    {
                        errorVelocity
                            = vMax * 0.5 * followDis
                            / fabs (followDis);
                        assert(!std::isnan(errorVelocity));
                    }
                    // 元の進行方向の速度は、横方向に移動する分遅くなる
                    double frontVelocity 
                        = sqrt (vMax * vMax
                                - errorVelocity * errorVelocity);
                    assert(!std::isnan(frontVelocity));

                    // 元の進行方向の成分を修正
                    AmuVector fDir = _velocity;
                    fDir.normalize();
                    fDir *= frontVelocity;

                    assert(!std::isnan(fDir.x()));
                    assert(!std::isnan(fDir.y()));
                    assert(!std::isnan(fDir.z()));

                    // 横方向成分を修正
                    AmuVector eDir = fDir;
                    eDir.revoltXY(-0.5*M_PI);
                    eDir *= errorVelocity;

                    assert(!std::isnan(eDir.x()));
                    assert(!std::isnan(eDir.y()));
                    assert(!std::isnan(eDir.z()));

                    // 元の進行方向+横方向=新しい進行方向
                    _velocity.setX (fDir.x()+eDir.x());
                    _velocity.setY (fDir.y()+eDir.y());
                    _velocity.setZ (fDir.z()+eDir.z());

                    assert(!std::isnan(_velocity.x()));
                    assert(!std::isnan(_velocity.y()));
                    assert(!std::isnan(_velocity.z()));

                }
            }
        }
    }
    else {   // if (direction)
    	// 逆方向 → 回避
    	double sideDistance = calcSideDistance (nearestPds);
    	assert(!std::isnan(sideDistance));

        // 回避するために必要な間隔
    	double avoidDis = psRad*2 - fabs(sideDistance);
    	assert(!std::isnan(avoidDis));

    	if (avoidDis <= 0)
        {
            // 十分離れている -> そのまま直進
            _recoverDirection ();
            _velocity.normalize();
            _velocity *= vMax;
    	}
        else
        {
            if (sideDistance < 0)
            {
                // 自分の方が外なら外に避ける -> avoidDisは正のままで良い
            }
            else
            {
                // 自分の方が中なら中に避ける -> avoidDisの符号を反転
                avoidDis = -1*avoidDis;
            }

            _recoverDirection ();

            // 歩行者との進行方向距離
            double frontDistance = calcFrontDistance (nearestPds);
            assert(!std::isnan(frontDistance));

            // 横方向の移動速度を計算
            double errorVelocity
                = avoidDis / fabs (frontDistance / _velocity.size());
            assert(!std::isnan(errorVelocity));

            if (fabs (errorVelocity) >= vMax)
            {
                /**
                 * @todo frontVelocityがnanにならないように、
                 * errorVelocityを少しだけvMaxより小さくしている。
                 * どのくらい小さくすればよいか調整が必要。
                 */
                errorVelocity
                    = vMax * avoidDis
                    / fabs (avoidDis) * 0.9;
                assert(!std::isnan(errorVelocity));
            }
            // 元の進行方向の速度は、横方向に移動する分遅くなる
            double frontVelocity = sqrt (vMax * vMax
                                         - errorVelocity * errorVelocity);
            assert(!std::isnan(frontVelocity));

            // 元の進行方向の成分を修正
            AmuVector fDir = _velocity;
            fDir.normalize();
            fDir *= frontVelocity;

            assert(!std::isnan(fDir.x()));
            assert(!std::isnan(fDir.y()));
            assert(!std::isnan(fDir.z()));

            // 横方向成分を修正
            AmuVector eDir = _velocity;
            eDir.revoltXY(-0.5*M_PI);
            eDir.normalize();
            eDir *= errorVelocity;

            assert(!std::isnan(eDir.x()));
            assert(!std::isnan(eDir.y()));
            assert(!std::isnan(eDir.z()));

            // 元の進行方向+横方向=新しい進行方向
            _velocity.setX (fDir.x()+eDir.x());
            _velocity.setY (fDir.y()+eDir.y());
            _velocity.setZ (fDir.z()+eDir.z());

            assert(!std::isnan(_velocity.x()));
            assert(!std::isnan(_velocity.y()));
            assert(!std::isnan(_velocity.z()));
    	}
    }
}

//======================================================================
const AmuPoint Pedestrian::calcNextPoint(AmuPoint* line0,
                                         AmuPoint* line1,
                                         bool* result_isReflected) const
{
    // Ax+By+C=0 : line0 - line1
    double A = line1->y() - line0->y();
    double B = line1->x() - line0->x();
    double C = line1->x()*line0->y() - line0->x()*line1->y();

    AmuPoint p1 = _point + _velocity;

    double side0 = A * _point.x() + B * _point.y() + C;
    double side1 = A * p1.x() + B * p1.y() + C;

    if ((side0<0.0 && side1<0.0)
        || (side0>=0.0 && side1>=0.0))
    {
        *result_isReflected = false;
        return p1;
    }

    // calc nearest point
    // An*x + Bn*y+Cn=0 : Ax+By+C=0と直交してp1を通る直線
    double An = -B;
    double Bn = A;
    double Cn = B * p1.x() - A * p1.y();
    double AnB_BnA = An * B - Bn * A;

    assert(!std::isnan(AnB_BnA));
    assert(AnB_BnA != 0);

    // 交点の座標を求める
    p1.setX ((-B*Cn + Bn*C) / AnB_BnA);
    p1.setY (( A*Cn - An*C) / AnB_BnA);

    *result_isReflected = true;
    return p1;
}

/*
 *                     flag[2]
 *             v3        /        v2
 *              +-----------------+
 *  flag[3] ->  |  |  |  |  |  |  |  <- flag[1]
 *              +-----------------+
 *             v0        /        v1
 *                     flag[0]
 */

//======================================================================
const AmuPoint Pedestrian::updatePointAndVelocity
(vector<AmuPoint*>* vertices, bool* result_flags) const
{
    AmuPoint nextPoint;
    bool isReflected;

    // check [0-1] side
    nextPoint = calcNextPoint((*vertices)[0], (*vertices)[1],
                              &isReflected);
    if (isReflected)
    {
        result_flags[0] = true;
        result_flags[2] = false;
    }
    else
    {
        // check [2-3] side
        nextPoint = calcNextPoint((*vertices)[2], (*vertices)[3],
                                  &isReflected);

        result_flags[0] = false;
        result_flags[2] = isReflected;
    }

    return nextPoint;
}

//======================================================================
void Pedestrian::walk()
{
    // ATTENTION : 横断歩道は四角形と仮定
    bool isReflected;

    if (_onZebra)
    {
        assert(_zebra != NULL);

        // 横断歩道の左右をはみ出ない位置に更新
        // ただし、始終点のラインは超えても良い
        AmuPoint nextPoint
            = _zebra->nextPedestrianPoint(this, &isReflected);

        if (isReflected)
        {
            /*
             * 横断歩道をはみ出していたら、横断歩道の端線に沿って
             * 対称な点に移動するような速度ベクトルに更新する。*/
            /*_velocity.setX(nextPoint.x()-_point.x());
              _velocity.setY(nextPoint.y()-_point.y());
              _velocity.setZ(nextPoint.z()-_point.z());*/
            _velocity.setXYZ(0.0, 0.0, 0.0);

        }
        _point = nextPoint;
    }

    // 横断歩道の範囲を越えたら停止
    if (!_zebra->isInside(_point))
    {
        _velocity.setXYZ(0.0, 0.0, 0.0);
        _onZebra = false;
        _zebra = NULL;
    }
}

//======================================================================
bool Pedestrian::isCrossingWithVehicle(Vehicle* vehicle) const
{
    return vehicle->pedExt()->isCrossingWith (this, 0.0);
}

//======================================================================
Pedestrian* Pedestrian::searchPedestrian ()
{
    return _zebra->searchNearestPedestrian(this);
}

//======================================================================
Vehicle* Pedestrian::searchVehicle()
{
    const RMAPLAN* collisionLanes = _zebra->laneBundle()->lanes();
    CITRMAPLAN lit;

    lit = collisionLanes->begin();
    while (lit != collisionLanes->end())
    {
        Lane* nextLane;
        Vehicle* vehicle;

        nextLane = lit->second;
        vehicle = nextLane->headVehicle();

        // 交錯レーンに自動車がいた場合
        if (vehicle != NULL)
        {
            if (isCrossingWithVehicle (vehicle))
            {
                return vehicle;
            }
        }
        lit++;
    }

    return NULL;
}

#endif //INCLUDE_PEDESTRIANS

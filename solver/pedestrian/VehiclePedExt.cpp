/**************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS
#include "VehiclePedExt.h"
#include "Pedestrian.h"
#include "Zebra.h"
#include "../VirtualLeader.h"
#include "../Vehicle.h"
#include "../Intersection.h"
#include "../Section.h"
#include "../Lane.h"
#include "../AmuPoint.h"
#include "../AmuVector.h"
#include <vector>

using namespace std;

//======================================================================
VehiclePedExt::VehiclePedExt(Vehicle* vehicle)
{
    _vehicle = vehicle;
}

//======================================================================
bool VehiclePedExt::recognizePedestrian(Intersection* nextInter)
{
    bool result = true;

    //------------------------------------------------------------------
    // 単路内
    if (_vehicle->_section)
    {
        // 先頭車両だけ判定する。
        if (_vehicle->_lane->headVehicle() == _vehicle)
        {
            // 現在のレーンは次の交差点から見てどの方向か?
            vector<int> dirs
                = nextInter->directionsTo(_vehicle->_lane);
          
#ifdef PDS_DEBUG
            {
                cerr << "v[" << this->id()
                     << "] : sect dirs.size=" << dirs.size() << endl;
                for (vector<int>::iterator it = dirs.begin();
                     it!= dirs.end();
                     it++)
                {
                    cerr << "    dir=" << *it << endl;
                }
            }
#endif
          
            if (dirs.size() == 1)
            {
                // 方向が1つに特定されている -> 対応する横断歩道を取得
                Zebra* zebra 
                    = nextInter->pedExt()->zebra(dirs[0]);
            
                // 停止位置
                double lengthToNext
                    = _vehicle->_section->lengthToNext
                    (_vehicle->_lane, _vehicle->_length)
                    - _vehicle->_bodyLength/2;

                result
                    = !(_isStoppedByCrossWithPedestrian
                        (zebra, lengthToNext));
            }
        }
    }

    //------------------------------------------------------------------
    // 交差点内
    else if (_vehicle->_intersection)
    {
        Zebra* zebra = NULL;

        // 交差点流出方向
        vector<int> dirs
            = _vehicle->_intersection->directionsTo
            (_vehicle->_lane);

#ifdef PDS_DEBUG
        {
            cerr << "v[" << this->id()
                 << "] : inter dirs.size=" << dirs.size() << endl;
            for (vector<int>::iterator it = dirs.begin();
                 it!= dirs.end();
                 it++)
            {
                cerr << "    dir=" << *it << endl;
            }
        }
#endif

        if (dirs.size() == 1)
        {
            // 方向が1つに特定されている -> 対応する横断歩道を取得
            zebra
                = _vehicle
                ->_intersection->pedExt()->zebra(dirs[0]);
        }

        if (zebra)
        {
#ifdef PDS_DEBUG
            cerr << "v[" << this->id()
                 << "] target zebra=" << zebra->id() << endl;
#endif
                
            double lengthToNext
                = _vehicle->_intersection->lengthToNext
                (_vehicle->_lane, _vehicle->_length)
                -_vehicle->_intersection->crosswalkWidth(dirs[0])
                -_vehicle->_bodyLength/2;
         
            // lengthToNextが負の場合にはすでに横断歩道上にいる
            if (lengthToNext>0)
            {
                result=
                    !(_isStoppedByCrossWithPedestrian
                      (zebra, lengthToNext));
            }
        }
    }
    return result;
}

//======================================================================
bool VehiclePedExt::isCrossingWith(const Pedestrian* pds,
                                   double time) const
{
    AmuPoint vcenter, vleft, vright, vfront, vback;
    AmuPoint vcenter_next, vleft_next, vright_next;
    AmuPoint vfront_next, vback_next;

    const AmuVector vdir;
    AmuVector vlength, vwidth;
    const AmuPoint* pds_point;
    int side[2][4];

    /*
     * 0: 過去
     * 1: 歩行者のスピード半分
     * 2: 歩行者のスピード1.5倍
     */
    int side_virtual[3][4];
    pds_point = pds->point();

    vcenter.setXYZ(_vehicle->x(), _vehicle->y(), _vehicle->z());
    vlength.setXYZ(_vehicle->directionVector().x(),
                   _vehicle->directionVector().y(),
                   _vehicle->directionVector().z());
    vlength.normalize();
    vlength *= _vehicle->bodyLength()/4.0;

    vwidth = vlength;
    vwidth.revoltXY(-1.0);
    vwidth.normalize();
    vwidth *= _vehicle->bodyWidth()/4.0;

    vleft = vcenter + ((vlength/4.0)*3.0)- vwidth;
    vright = vcenter + ((vlength/4.0)*3.0) + vwidth;
    vfront = vcenter + vlength;
    // vback = vcenter - vlength;
    vback = vcenter+(vlength/2.0);

    vcenter_next
        = vcenter + _vehicle->velocityVector()*time*1000;
    vleft_next = vcenter_next - vwidth;
    vright_next = vcenter_next + vwidth;
    vfront_next = vcenter_next + vlength;
    // vback_next = vcenter_next - vlength;
    vback_next = vcenter_next;      

    // 車両形状の4辺の直線のどちら側かを判定。
    side[0][0] = _detectSide(&vleft, &vlength, pds_point);
    side[0][1] = _detectSide(&vright, &vlength, pds_point);
    side[0][2] = _detectSide(&vfront, &vwidth, pds_point);
    side[0][3] = _detectSide(&vback, &vwidth, pds_point);

    // 現位置が車両と交差
    if (side[0][0] != side[0][1] && side[0][2] != side[0][3])
    {
#ifdef PDS_DEBUG
        cerr << "Pedestrian::isCrossingWithVehicle(): " << endl;
        cerr << "  v[" << _vehicle->id()
             << "] vcenter=" << vcenter
             << ", vfront=" << vfront << "" << endl;
        cerr << "  p[" << pds->id()
             << "]   point=" << *(pds->point()) << "" << endl;
        cerr << "  -> current position is corssing to vehicle."
             << endl;
#endif
        return true;
    }

    // 歩行者移動後の位置(と経路)も確認する
    if (0.0<time)
    {
        /*
         *次のステップの位置ではなく、一定時間経過後の
         * 予測地点を見たいので、Δtで割っている
         */
        AmuPoint pds_next
            = *pds_point  + *pds->velocity() * time * 1000;
        /*
         * AmuPoint pds_next
         * = *pds_point + *pds->velocity() / time;
         */
        /*
         * AmuPoint pds_before
         * = *pds_point  - *pds->velocity() * time * 1000;
         */
        AmuPoint pds_next_half
            = *pds_point + *pds->velocity() * time * 500;
        AmuPoint pds_next_twice
            = *pds_point + *pds->velocity() * time * 1000;
        
        // 車両形状の4辺の直線のどちら側かを判定。
        side[1][0] = _detectSide(&vleft, &vlength, &pds_next);
        side[1][1] = _detectSide(&vright, &vlength, &pds_next);
        side[1][2] = _detectSide(&vfront, &vwidth, &pds_next);
        side[1][3] = _detectSide(&vback, &vwidth, &pds_next);

        // side_virtual[0][0]
        // = _detectSide(&vleft, &vlength, &pds_before);
        // side_virtual[0][1]
        // = _detectSide(&vright, &vlength, &pds_before);
        // side_virtual[0][2]
        // = _detectSide(&vfront, &vwidth, &pds_before);
        // side_virtual[0][3]
        // = _detectSide(&vback, &vwidth, &pds_before);

        side_virtual[1][0] = _detectSide(&vleft, &vlength,
                                         &pds_next_half);
        side_virtual[1][1] = _detectSide(&vright, &vlength,
                                         &pds_next_half);
        side_virtual[1][2] = _detectSide(&vfront, &vwidth,
                                         &pds_next_half);
        side_virtual[1][3] = _detectSide(&vback, &vwidth,
                                         &pds_next_half);

        side_virtual[2][0] = _detectSide(&vleft, &vlength,
                                         &pds_next_twice);
        side_virtual[2][1] = _detectSide(&vright, &vlength,
                                         &pds_next_twice);
        side_virtual[2][2] = _detectSide(&vfront, &vwidth,
                                         &pds_next_twice);
        side_virtual[2][3] = _detectSide(&vback, &vwidth,
                                         &pds_next_twice);

        // 移動後の位置が車両と交差
        if (side[1][0] != side[1][1]
            && side[1][2] != side[1][3]
            && side [1][3] == 1 )
        {
        
#ifdef PDS_DEBUG
            cerr << "Pedestrian::isCrossingWithVehicle(): " << endl;
            cerr << "  v[" << _vehicle->id()
                 << "] vcenter=" << vcenter
                 << ", vfront=" << vfront << "" << endl;
            cerr << "  p[" << pds->id()
                 << "]   point=" << *(pds->point()) << "" << endl;
            cerr << "  -> next position is corssing to vehicle."
                 << endl;
#endif
            return true;
        }

        if (side_virtual[0][0] != side_virtual[0][1]
            && side_virtual[0][2] != side_virtual[0][3])
        {
            return true;
        }       
        if (side_virtual[1][0] != side_virtual[1][1]
            && side_virtual[1][2] != side_virtual[1][3])
        {
            return true;
        }    
        if (side_virtual[2][0] != side_virtual[2][1]
            && side_virtual[2][2] != side_virtual[2][3])
        {
            return true;
        }       

        // 移動途中のどこかで車両と交差
        if (side[0][0] != side[1][0]
            && side[0][1] != side[1][1] )
        {

#ifdef PDS_DEBUG
            cerr << "Pedestrian::isCrossingWithVehicle():" << endl;
            cerr << "  v[" << _vehicle->id()
                 << "] vcenter=" << vcenter
                 << ", vfront=" << vfront << "" << endl;
            cerr << "  p[" << pds->id()
                 << "]   point=" << *(pds->point()) << "" << endl;
            cerr << "  -> moving position is corssing to vehicle."
                 << endl;
#endif
            return true;
        }
    }

    //自動車の未来位置とも比較
    if (0.0<time)
    {
        /*
         *次のステップの位置ではなく、一定時間経過後の
         * 予測地点を見たいので、Δtで割っている
         */
        AmuPoint pds_next
            = *pds_point  + *pds->velocity() * time * 1000;
        /*
         * AmuPoint pds_next = *pds_point + *pds->velocity() / time;
         */
        AmuPoint pds_before
            = *pds_point  - *pds->velocity() * time * 1000;
        AmuPoint pds_next_half
            = *pds_point  + *pds->velocity() * time * 500;
        AmuPoint pds_next_twice
            = *pds_point  + *pds->velocity() * time * 1500;

        // 車両形状の4辺の直線のどちら側かを判定。
        side[1][0] = _detectSide(&vleft_next, &vlength,
                                 &pds_next);
        side[1][1] = _detectSide(&vright_next, &vlength,
                                 &pds_next);
        side[1][2] = _detectSide(&vfront_next, &vwidth,
                                 &pds_next);
        side[1][3] = _detectSide(&vback_next, &vwidth,
                                 &pds_next);

        side_virtual[0][0] = _detectSide(&vleft_next, &vlength,
                                         &pds_before);
        side_virtual[0][1] = _detectSide(&vright_next, &vlength,
                                         &pds_before);
        side_virtual[0][2] = _detectSide(&vfront_next, &vwidth,
                                         &pds_before);
        side_virtual[0][3] = _detectSide(&vback_next, &vwidth,
                                         &pds_before);

        side_virtual[1][0] = _detectSide(&vleft_next, &vlength,
                                         &pds_next_half);
        side_virtual[1][1] = _detectSide(&vright_next, &vlength,
                                         &pds_next_half);
        side_virtual[1][2] = _detectSide(&vfront_next, &vwidth,
                                         &pds_next_half);
        side_virtual[1][3] = _detectSide(&vback_next, &vwidth,
                                         &pds_next_half);

        side_virtual[2][0] = _detectSide(&vleft_next, &vlength,
                                         &pds_next_twice);
        side_virtual[2][1] = _detectSide(&vright_next, &vlength,
                                         &pds_next_twice);
        side_virtual[2][2] = _detectSide(&vfront_next, &vwidth,
                                         &pds_next_twice);
        side_virtual[2][3] = _detectSide(&vback_next, &vwidth,
                                         &pds_next_twice);
        // 移動後の位置が車両と交差
        if (side[1][0] != side[1][1]
            && side[1][2] != side[1][3]
            && side [1][3] == 1)
        {

#ifdef PDS_DEBUG
            cerr << "Pedestrian::isCrossingWithVehicle(): " << endl;
            cerr << "  v[" << _vehicle->id()
                 << "] vcenter=" << vcenter
                 << ", vfront=" << vfront << "" << endl;
            cerr << "  p[" << pds->id()
                 << "]   point=" << *(pds->point()) << endl;
            cerr << "  -> next position is corssing to vehicle."
                 << endl;
#endif
            return true;
        }
        if (side_virtual[0][0] != side_virtual[0][1]
            && side_virtual[0][2] != side_virtual[0][3])
        {
            return true;
        }       
        if (side_virtual[1][0] != side_virtual[1][1]
            && side_virtual[1][2] != side_virtual[1][3])
        {
            return true;
        }       
        if (side_virtual[2][0] != side_virtual[2][1]
            && side_virtual[2][2] != side_virtual[2][3])
        {
            return true;
        }       

        // 移動途中のどこかで車両と交差
        if (side[0][0] != side[1][0]
            && side[0][1] != side[1][1])
        {

#ifdef PDS_DEBUG
            cerr << "Pedestrian::isCrossingWithVehicle():" << endl;
            cerr << "  v[" << _vehicle->id()
                 << "] vcenter=" << vcenter
                 << ", vfront=" << vfront << "" << endl;
            cerr << "  p[" << pds->id()
                 << "]   point=" << *(pds->point()) << endl;
            cerr << "  -> moving position is corssing to vehicle."
                 << endl;
#endif
            return true;
        }
    }

    return false;
}

//======================================================================
bool VehiclePedExt::_isStoppedByCrossWithPedestrian
(Zebra* zebra, double lengthToNext)
{
    bool isCrossingwith = false;
 
    if (zebra != NULL)
    {
        if(zebra->pedestrians()->size() > 0)
        {
	    const vector<Pedestrian*>* pvec_p = zebra->pedestrians();
	    const vector<Pedestrian*>& pvec   = *pvec_p;
	    vector<Pedestrian*>::const_iterator exist = pvec.begin();
	   	   	     
	    while (exist != pvec.end())
            {
                Pedestrian *pds = (*exist);
                for(int i=1; i<=10; i++)
                {
                    double dt_cross = 0.1;
                    isCrossingwith
                        = isCrossingwith
                        || isCrossingWith(pds, i*dt_cross);

                    if(isCrossingwith)
                    {
                        break;
                    }
                }
                if (isCrossingwith)
                {
                    break;
		}		
                exist++;
	    }
        }
    }	

    if(!isCrossingwith)
    {
        return false;
    }

    if (zebra != NULL)
    {
        if (zebra->pedestrians()->size() > 0)
        {
	    // 歩行者がいる -> 止まる
	    
	    // ad hoc
            
	    //要調整	
	    /*if (lengthToNext > 18.0) {
            // 次の交差点が遠ければ気にしない
            return false;
	    }*/

	    // 停止用の仮想先行車
	    VirtualLeader* leader
                = new VirtualLeader(lengthToNext, 0,
                                    "CLPDS:"+_vehicle->_id);
	    //VirtualLeader* leader = new VirtualLeader(0, 0, "CLPDS:"+id());
	    
	    _vehicle->_leaders.push_back(leader);
	    
            /* 止まれない場合は加速？？
	    if (_vehicle->_velocity<1.0e-6)
            {
		_vehicle->_accel
                    = _vehicle->_maxAcceleration*1.5;
		_vehicle->_sleepTime
                    = 0.2*1000;
            }
            */
	    
	    return true;
        }
    }
    return false;
}


//======================================================================
int VehiclePedExt::_detectSide (const AmuPoint* p, const AmuVector* dir,
                                const AmuPoint* point) const
{
    // line equation parameters : Ax+By+C=0
    double a, b, c;
    double r;
    double vx, vy, px, py;
    double x, y;
    
    const double EPS_DETECT_SIDE = 1e-10;
    
    vx = dir->x();
    vy = dir->y();
    px = p->x();
    py = p->y();

    a = vy;
    b = -vx;
    c = vx*py - vy*px;

    // calc Ax+By+C for current position of pds
    x = point->x();
    y = point->y();
    r = a*x + b*y + c;

    if (r < -EPS_DETECT_SIDE)
    {
        return -1;
    }
    else if (EPS_DETECT_SIDE < r)
    {
        return 1;
    }
    return 0;
}

#endif //INCLUDE_PEDESTRIANS

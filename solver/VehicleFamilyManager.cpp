/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "VehicleFamilyManager.h"
#include <iostream>
#include <vector>

using namespace std;

//######################################################################
// VFAttributeクラス
VFAttribute::VFAttribute(){}

//======================================================================
VFAttribute::VFAttribute(int id,
                         double bodyLength,
                         double bodyWidth,
                         double bodyHeight,
                         int    bodyArticulation,
                         double bodyWeight,
                         double maxAcceleration,
                         double maxDeceleration,
                         double bodyColorR,
                         double bodyColorG,
                         double bodyColorB)
    :_id(id),
     _bodyLength(bodyLength),
     _bodyWidth(bodyWidth),
     _bodyHeight(bodyHeight),
     _bodyArticulation(bodyArticulation),
     _bodyWeight(bodyWeight),
     _maxAcceleration(maxAcceleration),
     _maxDeceleration(maxDeceleration),
     _bodyColorR(bodyColorR),
     _bodyColorG(bodyColorG),
     _bodyColorB(bodyColorB){}

//======================================================================
VFAttribute::~VFAttribute(){}

//======================================================================
int VFAttribute::id() const
{
    return _id;
}

//======================================================================
double VFAttribute::bodyLength() const
{
    return _bodyLength;
}

//======================================================================
double VFAttribute::bodyWidth() const
{
    return _bodyWidth;
}

//======================================================================
double VFAttribute::bodyHeight() const
{
    return _bodyHeight;
}

//======================================================================
void VFAttribute::getSize(double *result_l,
                          double *result_w,
                          double *result_h) const
{
    *result_l = _bodyLength;
    *result_w = _bodyWidth;
    *result_h = _bodyHeight;
}

//======================================================================
int VFAttribute::bodyArticulation() const
{
    return _bodyArticulation;
}

//======================================================================
double VFAttribute::maxAcceleration() const
{
    return _maxAcceleration;
}

//======================================================================
double VFAttribute::maxDeceleration() const
{
    return _maxDeceleration;
}

//======================================================================
void VFAttribute::getPerformance(double *result_a,
                                 double *result_d) const
{
    *result_a = _maxAcceleration;
    *result_d = _maxDeceleration;
}

//======================================================================
void VFAttribute::getBodyColor(double *result_r,
                               double *result_g,
                               double *result_b) const
{
    *result_r = _bodyColorR;
    *result_g = _bodyColorG;
    *result_b = _bodyColorB;
}

//######################################################################
// VFManagerクラス
map<int, VFAttribute> VehicleFamilyManager::_families;

//======================================================================
void VehicleFamilyManager::addVehicleFamily(VFAttribute vehicleFamily)
{
    // 重複した場合はメッセージを出して上書きする
    int id = vehicleFamily.id();
    map<int, VFAttribute, less<int> >::iterator itvf
        = _families.find(id);
    
    if (itvf != _families.end())
    {
        cout << "VehicleFamilyAttribute: " << id
             << " has been overwritten." << endl;
    }
    _families[id] = vehicleFamily;
}

//======================================================================
map<int, VFAttribute, less<int> >* VehicleFamilyManager::families()
{
    return &_families;
}

//======================================================================
VFAttribute* VehicleFamilyManager::vehicleFamilyAttribute(int id)
{
    map<int, VFAttribute, less<int> >::iterator itvf
        = _families.find(id);
    if (itvf != _families.end())
    {
        return &(itvf->second);
    }
    else
    {
        return NULL;
    }
}

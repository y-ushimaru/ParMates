/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <vector>
#include <sstream>
#include <iomanip>
#include "VirtualOccupant.h"
#include "AmuConverter.h"
#include "GVManager.h"
#include "RoadMap.h"
#include "Section.h"
#include "Lane.h"

using namespace std;
//======================================================================
VirtualOccupant::VirtualOccupant()
{
  _bodyLength = 4.400;
  _bodyWidth  = 1.830;
  _bodyHeight = 1.315;

  _length = 0;
  _totalLength = 0;
  _velocity = 0;
  _accel = 0;
}
//======================================================================
const string&  VirtualOccupant::id() const
{
    return _id;
}

//======================================================================
double VirtualOccupant::bodyWidth() const
{
    return _bodyWidth;
}

//======================================================================
double VirtualOccupant::bodyLength() const
{
    return _bodyLength;
}

//======================================================================
double VirtualOccupant::bodyHeight() const
{
    return _bodyHeight;
}

//======================================================================
double VirtualOccupant::length() const
{
    return _length;
}

//======================================================================
double VirtualOccupant::totalLength() const
{
    return _totalLength;
}

//======================================================================
double VirtualOccupant::velocity() const
{
    return _velocity;
}

//======================================================================
double VirtualOccupant::accel() const
{
    return _accel;
}

//======================================================================
const AmuVector VirtualOccupant::directionVector() const
{
    return _lane->directionVector();
}

//======================================================================
void VirtualOccupant::setStatus(RoadMap* roadMap, vector<double>* status)
{
  vector<double> _status = *status;
  string id;
  int count = 0;

  //ロードマップ
  _roadMap = roadMap;

  //単路
  stringstream str;
  str << setprecision(NUM_FIGURE_FOR_SECTION) << _status[count];
  id = AmuConverter::formatId(str.str(), NUM_FIGURE_FOR_SECTION);
  _section = _roadMap->section(id);
  count++;

  //レーン
  RMAPLAN *lanes;
  lanes = const_cast<RMAPLAN*>(_section->lanes());

  id = AmuConverter::itos(_status[count], NUM_FIGURE_FOR_LANE);
  _lane = (*lanes)[id];
  count++;

  //レーン始点からの距離
  _length = _status[count];
  count++;

  //速度
  _velocity = _status[count];
  count++;

  _lane->putVirtualAgent(this);

}

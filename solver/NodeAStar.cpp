/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "NodeAStar.h"
#include "Intersection.h"

//======================================================================
NodeAStar::NodeAStar(Intersection* me, NodeAStar* pa)
{
  _top = me;
  if(pa != NULL)
  {
    _bottom = pa->top();
  }
  else
  {
    _bottom = NULL;
  }
  _parent = pa;
  _gv = _hv = 0;
  _isAble = true;
}

//======================================================================
NodeAStar::~NodeAStar() {}

//======================================================================
void NodeAStar::setGV(double g)
{
  _gv = g;
}

//======================================================================
double NodeAStar::gv() const
{
  return _gv;
}

//======================================================================
void NodeAStar::setHV(double h)
{
  _hv = h;
}

//======================================================================
double NodeAStar::hv() const
{
  return _hv;
}

//======================================================================
Intersection* NodeAStar::top() const
{
  return _top;
}

//======================================================================
Intersection* NodeAStar::bottom() const
{
  return _bottom;
}

//======================================================================
void NodeAStar::setTop(Intersection* top)
{
  if(top != NULL)
  {
    _top = top;
  }
}

//======================================================================
void NodeAStar::setBottom(Intersection* bottom)
{
  if(bottom != NULL)
  {
    _bottom = bottom;
  }
}

//======================================================================
NodeAStar* NodeAStar::parent() const
{
  return _parent;
}

//======================================================================
void NodeAStar::enable()
{
  _isAble = true;
}

//======================================================================
void NodeAStar::disable()
{
  _isAble = false;
}

//======================================================================
bool NodeAStar::isAble() const
{
  return _isAble;
}

//======================================================================
bool NodeAStar::compare(const NodeAStar& node) const
{
  bool flag = false;

  if (_top == node._top && _bottom == node._bottom)
  {
    flag = true;
  }
  return flag;
}

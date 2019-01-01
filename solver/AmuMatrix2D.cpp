/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <iostream>
#include "AmuMatrix2D.h"

using namespace std;

//======================================================================
AmuMatrix2D::AmuMatrix2D(double x00,double x01,
		       double x10,double x11)
{
  _matrix[0][0] = x00;
  _matrix[0][1] = x01;
  _matrix[1][0] = x10;
  _matrix[1][1] = x11;
}

//======================================================================
AmuMatrix2D::~AmuMatrix2D() {}

//======================================================================
double AmuMatrix2D::delta() const
{
  return _matrix[0][0] * _matrix[1][1]
      - _matrix[0][1] * _matrix[1][0];
}

//======================================================================
bool AmuMatrix2D::inverse()
{
  bool result = true;
  if(this->delta() == 0.0)
  {
    result = false;
  }
  else
  {
    double delta = this->delta();
    double tmpMatrix[2][2];
    for(int i = 0;i <= 1;i++)
    {
      for(int j = 0;j <= 1;j++)
      {
	tmpMatrix[i][j] = _matrix[i][j];
      }//for j
    }//for i
    _matrix[0][0] =  tmpMatrix[1][1] / delta;
    _matrix[0][1] = -tmpMatrix[0][1] / delta;
    _matrix[1][0] = -tmpMatrix[1][0] / delta;
    _matrix[1][1] =  tmpMatrix[0][0] / delta;
  }//else
  return result;
}

//======================================================================
double AmuMatrix2D::getItem(int i,int j) const
{
  // 成分の範囲を確認する
  if( 0 <= i && i <= 1 && 0 <= j && j <= 1)
  {
    return _matrix[i][j];
  }
  else
  {
    cerr << "try to get undefined item at AmuMatrix2D::getItem()."
	 << endl;
    return 0.0;
  }
}

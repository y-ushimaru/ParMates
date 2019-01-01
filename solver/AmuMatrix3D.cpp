/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <iostream>
#include "AmuMatrix3D.h"

using namespace std;

//======================================================================
AmuMatrix3D::AmuMatrix3D(double x00,double x01,double x02,
		   double x10,double x11,double x12,
		   double x20,double x21,double x22)
{
  _matrix[0][0] = x00;
  _matrix[0][1] = x01;
  _matrix[0][2] = x02;
  _matrix[1][0] = x10;
  _matrix[1][1] = x11;
  _matrix[1][2] = x12;
  _matrix[2][0] = x20;
  _matrix[2][1] = x21;
  _matrix[2][2] = x22;
}

//======================================================================
AmuMatrix3D::~AmuMatrix3D(){}

//======================================================================
double AmuMatrix3D::delta() const
{
  return _matrix[0][0] * _matrix[1][1] * _matrix[2][2]
      + _matrix[0][1] * _matrix[1][2] * _matrix[2][0]
      + _matrix[0][2] * _matrix[1][0] * _matrix[2][1]
      - _matrix[0][0] * _matrix[1][2] * _matrix[2][1]
      - _matrix[0][1] * _matrix[1][0] * _matrix[2][2]
      - _matrix[0][2] * _matrix[1][1] * _matrix[2][0];
}

//======================================================================
bool AmuMatrix3D::inverse()
{
  bool result = true;
  if(this->delta() == 0.0)
  {
    result = false;
  }
  else
  {
    double delta = this->delta();
    double tmpMatrix[3][3];
    for(int i = 0;i <= 2;i++)
    {
      for(int j = 0;j <= 2;j++)
      {
	tmpMatrix[i][j] = _matrix[i][j];
      }//for j
    }//for i
    _matrix[0][0] = (tmpMatrix[1][1] * tmpMatrix[2][2] 
                   - tmpMatrix[1][2] * tmpMatrix[2][1]) / delta;
    _matrix[0][1] = (tmpMatrix[0][2] * tmpMatrix[2][1] 
                   - tmpMatrix[0][1] * tmpMatrix[2][2]) / delta;
    _matrix[0][2] = (tmpMatrix[0][1] * tmpMatrix[1][2] 
                   - tmpMatrix[0][2] * tmpMatrix[1][1]) / delta;
    _matrix[1][0] = (tmpMatrix[1][2] * tmpMatrix[2][0] 
                   - tmpMatrix[1][0] * tmpMatrix[2][2]) / delta;
    _matrix[1][1] = (tmpMatrix[0][0] * tmpMatrix[2][2] 
                   - tmpMatrix[0][2] * tmpMatrix[2][0]) / delta;
    _matrix[1][2] = (tmpMatrix[0][2] * tmpMatrix[1][0] 
                   - tmpMatrix[0][0] * tmpMatrix[1][2]) / delta;
    _matrix[2][0] = (tmpMatrix[1][0] * tmpMatrix[2][1] 
                   - tmpMatrix[1][1] * tmpMatrix[2][0]) / delta;
    _matrix[2][1] = (tmpMatrix[0][1] * tmpMatrix[2][0] 
                   - tmpMatrix[0][0] * tmpMatrix[2][1]) / delta;
    _matrix[2][2] = (tmpMatrix[0][0] * tmpMatrix[1][1] 
                   - tmpMatrix[0][1] * tmpMatrix[1][0]) / delta;
  }//else
  return result;
}

//======================================================================
double AmuMatrix3D::getItem(int i,int j) const
{
  // 成分の範囲を確認する
  if( 0 <= i && i <= 2 && 0 <= j && j <= 2)
  {
    return _matrix[i][j];
  }
  else
  {
    cerr << "try to get undefined item at AmuMatrix3D::getItem()."
	 << endl;
    return 0.0;
  }
}

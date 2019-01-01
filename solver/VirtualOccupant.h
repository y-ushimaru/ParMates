/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __VIRTUAL_OCCUPANT_H__
#define __VIRTUAL_OCCUPANT_H__

#include <vector>
#include "RoadOccupant.h"


class RoadMap;
class Section;
class Lane;

/// 隣接領域に存在する仮想先行車クラス
/// 現状ではLaneのtailAgentで隣接領域のエージェントを参照する場合に使用する

class VirtualOccupant : public RoadOccupant
{

public:

  VirtualOccupant();
  virtual ~VirtualOccupant(){};

  void setStatus(RoadMap* roadMap, std::vector<double>* status);

  /// 識別番号を返す
  const std::string& id() const;

  /// 幅を返す
  double bodyWidth() const;
  /// 長さを返す
  double bodyLength() const;
  /// 高さを返す
  double bodyHeight() const;

  /// レーンの始点からの距離
  double length() const;
  /// 単路の始点からの距離
  double totalLength() const;
  /// 速度
  double velocity() const;
  /// 加速度
  double accel() const;

  /// 方向ベクトル
  const AmuVector directionVector() const;

protected:

  /// 識別番号
  std::string _id;

  /// 車体の幅
  double _bodyWidth;

  /// 車体の長さ
  double _bodyLength;

  /// 車体の高さ
  double _bodyHeight;

  /// 現在自分が存在する道路ネットワーク
  RoadMap* _roadMap;

  /// 自分がいる単路
  Section* _section;

  /// 自分がいるレーン
  Lane* _lane;

  /// レーン始点からの長さ[m]
  double _length;

  /// レーン始点からの長さ[m]
  double _totalLength;

  /// 速度[m/msec]
  double _velocity;

  /// 加速度[m/msec^2]
  double _accel;

};

#endif //__VIRTUAL_OCCUPANT_H__

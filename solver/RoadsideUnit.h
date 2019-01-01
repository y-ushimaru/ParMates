/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __ROADSIDE_UNIT_H__
#define __ROADSIDE_UNIT_H__

#include <string>

/**
 * @addtogroup RoadsideUnit
 * @brief 路側器を定義するモジュール
 */
/**
 * @addtogroup Monitoring
 * @brief シミュレーションの計測
 * @ingroup Procedure
 */

/// 路側に配置される信号機以外の機器の純粋抽象クラス
/**
 * DetectorUnitなどに継承させる
 *
 * @ingroup RoadsideUnit Monitoring
 */
class RoadsideUnit{
 public:
  virtual ~RoadsideUnit(){};

  /// 機器の識別番号を返す
  virtual const std::string& id() const = 0;
};

#endif //__ROADSIDE_UNIT_H__

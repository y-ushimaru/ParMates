/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNALCONTROLDATA_H__
#define __SIGNALCONTROLDATA_H__

#include <vector>
#include "TimeManager.h"

/// ある時刻での信号制御情報の格納用クラス
/**
 * @ingroup Signal
 */
class SignalControlData
{
public:
    SignalControlData();
    SignalControlData(ulint begin,
                      ulint end,
                      ulint cycle,
                      std::vector<ulint>& split);
    ~SignalControlData();
    ulint begin() const;
    ulint end() const;
    ulint cycle() const;
    ulint split(int splitNum) const;
  
protected:
    ulint _begin, _end, _cycle;
    std::vector<ulint> _split;
};

#endif //__SIGNALCONTROLDATA_H__

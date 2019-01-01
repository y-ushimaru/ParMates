/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "TimeManager.h"
#include "Clocker.h"
#include "ParallelConf.h"
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

using namespace std;

// メンバ変数の定義
ulint TimeManager::_time = 0;
ulint TimeManager::_unit = 100;
ulint TimeManager::_step = 0;

// parallel 計測の開始と終了時間の定義
ulint TimeManager::_startTime = START_MEASURE_TIME;
ulint TimeManager::_endTime = END_MEASURE_TIME;

MAPCLK TimeManager::_clockers;

//======================================================================
ulint TimeManager::time()
{
    return _time;
}

//======================================================================
void TimeManager::setTime(ulint presentTime)
{
    _time = presentTime;
}

//======================================================================
ulint TimeManager::unit()
{
    return _unit;
}

//======================================================================
void TimeManager::setUnit(ulint unit)
{
    _unit = unit;
}

//======================================================================
ulint TimeManager::step()
{
    return _step;
}

//======================================================================
void TimeManager::setStep(ulint step)
{
    _step = step;
}

//======================================================================
void TimeManager::increment()
{
    _step++;
    _time += _unit;
}

//======================================================================
bool TimeManager::startClock(const std::string clockName)
{
#ifdef MEASURE_TIME
    // マルチスレッド処理の中では，マスタースレッドでのみ計時する
    /*
     * 計時の中にマルチスレッド処理が含まれるのは構わない
     */
    bool result = false;

    if(_time < _startTime || _endTime < _time)
    {
      return result;
    }

#pragma omp master
    {
        // 該当する時計の検索
        Clocker* clocker;
        ITRMAPCLK iclk = _clockers.find(clockName);
        if (iclk==_clockers.end())
        {
            // 見つからなければ新たに作成する
            clocker = new Clocker();
            _clockers[clockName] = clocker;
            cout << "clock " << clockName
                 << " created." << endl;
        }
        else
        {
            clocker = (*iclk).second;
        }

        result = clocker->startClock();
        if (!result)
        {
            cerr << "clock " << clockName
                 << " has already started" << endl;
        }
    }
    return result;
#else  //MEASURE_TIME
    return false;
#endif //MEASURE_TIME
}

//======================================================================
bool TimeManager::stopClock(const std::string clockName)
{
#ifdef MEASURE_TIME
    // マルチスレッド処理の中では，マスタースレッドでのみ計時する
    /*
     * 計時の中にマルチスレッド処理が負組まれるのは構わない
     */
    bool result = false;

    if(_time < _startTime || _endTime < _time)
    {
      return result;
    }

#pragma omp master
    {
        // 該当する時計の検索
        Clocker* clocker;
        ITRMAPCLK iclk = _clockers.find(clockName);
        if (iclk==_clockers.end())
        {
            // 見つからない場合
            cerr << "clock " << clockName
                 << " is not found" << endl;
        }
        else
        {
            clocker = (*iclk).second;

            result = clocker->stopClock();
            if (!result)
            {
                cerr << "clock " << clockName
                     << " has not started" << endl;
            }
        }
    }
    return result;
#else  //MEASURE_TIME
    return false;
#endif //MEASURE_TIME
}

//======================================================================
void TimeManager::printAllClockers()
{
#ifdef MEASURE_TIME
    cout << "clock name/ total cpu time[sec], total wallclock time[sec]"
         << endl;
    // 表示のためにソートする
    map<string, Clocker*> mapClockers;
    for (CITRMAPCLK where=_clockers.begin();
         where!=_clockers.end();
         where++)
    {
        mapClockers.insert(make_pair((*where).first, (*where).second));
    }
    for (map<string, Clocker*>::const_iterator
             where=mapClockers.begin();
         where!=mapClockers.end();
         where++)
    {
        cout << (*where).first << "/ "
             << (*where).second->totalProcessorTime() << ", "
             << (*where).second->totalTime() << endl;
    }
#endif //MEASURE_TIME
}

//======================================================================
void TimeManager::deleteAllClockers()
{
    ITRMAPCLK iclk = _clockers.begin();
    while (iclk != _clockers.end())
    {
        delete (*iclk).second;
        iclk++;
    }
    _clockers.erase(_clockers.begin(), _clockers.end());
}

//======================================================================
double TimeManager::outputClocker(const std::string clockName)
{
  double totalTime = 0;

#ifdef MEASURE_TIME

  // 該当する時計の検索
  Clocker* clocker;
  ITRMAPCLK iclk = _clockers.find(clockName);
  if (iclk==_clockers.end())
  {
      // 見つからない場合
      cerr << "clock " << clockName
          << " is not found" << endl;
  }
  else
  {
      clocker = (*iclk).second;

      totalTime = clocker->totalTime();
  }

#endif //MEASURE_TIME

  return totalTime;
}

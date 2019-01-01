/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Clocker.h"
#include <iostream>
#include <unistd.h>
#ifndef USE_MINGW
#include <sys/time.h>
#include <sys/resource.h>
#endif //USE_MINGW

using namespace std;

//======================================================================
Clocker::Clocker()
{
    _totalTime          = 0.0;
    _totalProcessorTime = 0.0;
    _isClocking         = false;
}

//======================================================================
bool Clocker::startClock()
{
    if (_isClocking)
    {
        return false;            
    }

    /**
     * @note MinGWの場合はすべてclock関数で計時する．
     * それ以外の場合，経過時間はgettimeofdayで計時，
     * CPU時間はgetrusageで計時する．
     */
#ifdef USE_MINGW
    _startTimeClock = clock();
#else //USE_MINGW
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    _startTimeRu
        = ru.ru_utime.tv_sec
        + (double)ru.ru_utime.tv_usec * 1e-6;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    _startTimeTod = tv.tv_sec + tv.tv_usec * 1e-6;
#endif //USE_MINGW

    _isClocking = true;
    return true;
}

//======================================================================
bool Clocker::stopClock()
{
    if (!_isClocking)
    {
        return false;
    }

#ifdef USE_MINGW
    clock_t stopTimeClock = clock();
    _totalProcessorTime
        += static_cast<double>((stopTimeClock - _startTimeClock)) / CLOCKS_PER_SEC;
    _totalTime
        += static_cast<double>((stopTimeClock - _startTimeClock)) / CLOCKS_PER_SEC; 
#else //USE_MINGW
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    double stopTimeRu
        = ru.ru_utime.tv_sec
        + (double)ru.ru_utime.tv_usec * 1e-6;
    _totalProcessorTime
        += stopTimeRu - _startTimeRu;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double stopTimeTod = tv.tv_sec + tv.tv_usec * 1e-6;
    _totalTime
        += stopTimeTod - _startTimeTod;
#endif //USE_MINGW

    _isClocking = false;
    return true;
}

//======================================================================
double Clocker::totalProcessorTime() const
{
    return _totalProcessorTime;
}

//======================================================================
double Clocker::totalTime() const
{
    return _totalTime;
}

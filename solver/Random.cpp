/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "Random.h"
#include <limits>
#include <limits.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <functional>
#include <algorithm>
#include <mt19937ar.h>
#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

using namespace std;

//######################################################################
void StdRand::setSeed(unsigned long seed)
{
#ifdef _OPENMP
#pragma omp critical (StdRand_setSeed)
    {
#endif //_OPENMP

        srand(seed);

#ifdef _OPENMP
    }
#endif //_OPENMP
}

//----------------------------------------------------------------------
unsigned long StdRand::nextInt()
{
    unsigned long result;

#ifdef _OPENMP
#pragma omp critical (StdRand_nextInt)
    {
#endif //_OPENMP

        result = rand();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
unsigned long StdRand::nextInt(unsigned long max)
{
    return static_cast<unsigned long>(nextReal() * max);
}

//----------------------------------------------------------------------
unsigned long StdRand::maxInt() const
{
    return RAND_MAX;
}

//----------------------------------------------------------------------
double StdRand::nextReal()
{
    double result;

#ifdef _OPENMP
#pragma omp critical (StdRand_nextReal)
    {
#endif //_OPENMP

        result = ((double)std::rand() / (double)(RAND_MAX));

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
double StdRand::nextReal(double max)
{
    return nextReal() * max;
}

//######################################################################
void LinearCongruential::setSeed(unsigned long seed)
{
#ifdef _OPENMP
#pragma omp critical (LC_setSeed)
    {
#endif //_OPENMP

        _x = seed; 

#ifdef _OPENMP
    }
#endif //_OPENMP
}

//----------------------------------------------------------------------
unsigned long LinearCongruential::nextInt()
{
#ifdef _OPENMP
#pragma omp critical (LC_nextInt)
    {
#endif //_OPENMP

        _x *= 1566083941UL;
        _x += 1;

#ifdef _OPENMP
    }
#endif //_OPENMP

    return _x;
}

//----------------------------------------------------------------------
unsigned long LinearCongruential::nextInt(unsigned long max)
{
    return (unsigned long)(max * nextReal());
}

//----------------------------------------------------------------------
unsigned long LinearCongruential::maxInt() const
{
    return numeric_limits<unsigned long>::max();
} 

//----------------------------------------------------------------------
double LinearCongruential::nextReal()
{
    return (double)(nextInt()) / (double)(maxInt());
}

//----------------------------------------------------------------------
double LinearCongruential::nextReal(double max)
{
    return max * nextReal();
}

//######################################################################
void MT19937::setSeed(unsigned long seed)
{
#ifdef _OPENMP
#pragma omp critical (MT_setSeed)
    {
#endif //_OPENMP
    
        init_genrand(seed);

#ifdef _OPENMP
    }
#endif //_OPENMP
}

//----------------------------------------------------------------------
unsigned long MT19937::nextInt()
{
    unsigned long result;

#ifdef _OPENMP
#pragma omp critical (MT_nextInt)
    {
#endif //_OPENMP

        result = nextInt31();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
unsigned long MT19937::maxInt() const
{
    return 0x7fffffff;
}

//----------------------------------------------------------------------
unsigned long MT19937::nextInt(unsigned long max)
{
    return static_cast<unsigned long>(nextReal2() * max);
    // 半開区間版を利用する必要
}

//----------------------------------------------------------------------
double MT19937::nextReal()
{
    return nextReal1();
}

//----------------------------------------------------------------------
double MT19937::nextReal(double max)
{
    return nextReal1() * max;
}

//----------------------------------------------------------------------
unsigned long MT19937::nextInt32(void)
{
    unsigned long result;

#ifdef _OPENMP
#pragma omp critical (MT_nextInt32)
    {
#endif //_OPENMP
        result = genrand_int32();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
long MT19937::nextInt31(void)
{
    long result;

#ifdef _OPENMP
#pragma omp critical (MT_nextInt31)
    {
#endif //_OPENMP
        result = genrand_int31();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
double MT19937::nextReal1(void)
{
    double result;

#ifdef _OPENMP
#pragma omp critical (MT_nextReal1)
    {
#endif //_OPENMP

        result = genrand_real1();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
double MT19937::nextReal2(void)
{
    double result;

#ifdef _OPENMP
#pragma omp critical (MT_nextReal2)
    {
#endif //_OPENMP

        result = genrand_real2();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
double MT19937::nextReal3(void)
{
    double result;

#ifdef _OPENMP
#pragma omp critical (MT_nextReal3)
    {
#endif //_OPENMP

        result = genrand_real3();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
}

//----------------------------------------------------------------------
double MT19937::nextRealRes53(void) 
{ 
    double result;

#ifdef _OPENMP
#pragma omp critical (MT_nextRealRes53)
    {
#endif //_OPENMP
        result = genrand_res53();

#ifdef _OPENMP
    }
#endif //_OPENMP

    return result;
} 

//----------------------------------------------------------------------
istream& operator>>(istream& is, MT19937& gen)
{
    unsigned long* it = gen.mt;

#ifdef _OPENMP
#pragma omp critical (MT_istream)
    {
#endif //_OPENMP

        for (size_t i = 0; i < gen.N; i++) 
        {
            is >> *(it++);
        }

#ifdef _OPENMP
    }
#endif //_OPENMP

    return is;
}

//----------------------------------------------------------------------
ostream& operator<<(ostream& os, MT19937& gen)
{

#ifdef _OPENMP
#pragma omp critical (MT_ostream)
    {
#endif //_OPENMP

        copy(gen.mt, gen.mt + gen.N,
             ostream_iterator<unsigned long>(os, " "));

#ifdef _OPENMP
    }
#endif //_OPENMP

    return os;
}

//######################################################################
void XSadd::setSeed(unsigned long seed)
{
    xsadd_init(&xsa, seed);
}

//----------------------------------------------------------------------
unsigned long XSadd::nextInt()
{
    return xsadd_uint32(&xsa);
}

//----------------------------------------------------------------------
unsigned long XSadd::nextInt(unsigned long max)
{
    return xsadd_uint32(&xsa) % max;
}

//----------------------------------------------------------------------
unsigned long XSadd::maxInt() const
{
    return UINT_MAX;
}

//----------------------------------------------------------------------
double XSadd::nextReal()
{
    return (double)nextInt() / maxInt();
}

//----------------------------------------------------------------------
double XSadd::nextReal(double max)
{
    return nextReal() * max;
}

//######################################################################
double NormalDist::operator()(RandomGenerator* gen,
                              double mu, double sigma)
{
    return mu + sigma * generate_0_1(gen);
}

//----------------------------------------------------------------------
double NormalDist::generate_0_1(RandomGenerator* gen)
{
    double retval;

#ifdef _OPENMP
#pragma omp critical (NormalDist_generate_0_1)
    {
#endif //_OPENMP

        if (!phase)
        {
            double s;
            do
            { 
                r1 = 2 * gen->nextReal() - 1;
                r2 = 2 * gen->nextReal() - 1;
                s = r1 * r1 + r2 * r2;
            } while(s >= 1 || s == 0);
            
            retval = r1 * sqrt(-2 * log(s) / s);
            phase = true;
        }
        else
        {
            double s = r1 * r1 + r2 * r2;
            retval = r2 * sqrt(-2 * log(s) / s);
            phase = false;
        }

#ifdef _OPENMP
    }
#endif //_OPENMP

    return retval;
}

//######################################################################
int PoissonDist::operator()(RandomGenerator* gen,
                            double lambda)
{
    int k;
    double p = exp(lambda) * gen->nextReal();
    for (k = 0; p > 1.0; k++)
    {
        p *= gen->nextReal();
    }
    return k;
}

//######################################################################
double ExponentialDist::operator()(RandomGenerator* gen,
                                   double lambda)
{
    return - log(gen->nextReal()) / lambda;
}

//######################################################################
int BinomialDist::operator()(RandomGenerator* gen,
                             int n, double p)
{
    // 定義どおりの実装．n 回繰り返す．
    int retval = 0;
    for (int i = 0; i < n; i++)
    {
        if (gen->nextReal() < p)
            retval++;
    }
    return retval;
}

//######################################################################
#ifdef USE_INDEPENDENT_RANDOM_GENERATOR

unsigned long    Random::_numAssigned = 0;
unsigned long    Random::_numReleased = 0;

#else //USE_INDEPENDENT_RANDOM_GENERATOR not defined

RandomGenerator* Random::_gen = NULL;

#endif //USE_INDEPENDENT_RANDOM_GENERATOR

NormalDist       Random::_normal;
BinomialDist     Random::_binom;
PoissonDist      Random::_poisson;
ExponentialDist  Random::_exp;
unsigned long    Random::_seed = 2;

//======================================================================
void Random::setSeed(unsigned long seed)
{
    _seed = seed;
}

//======================================================================
void Random::initialize()
{
#ifdef USE_INDEPENDENT_RANDOM_GENERATOR //..............................
    
    //生成は個々のgenerateで行う

#else  //USE_INDEPENDENT_RANDOM_GENERATOR not defined ..................
    // デフォルトでメルセンヌ・ツイスタを使用する
    //_gen = new StdRand();
    //_gen = new LinearCongruential();
    _gen = new MT19937();
    _gen->setSeed(_seed);
#endif //USE_INDEPENDENT_RANDOM_GENERATOR ..............................
}

//======================================================================
void Random::finalize()
{
#ifdef USE_INDEPENDENT_RANDOM_GENERATOR //..............................

    // 削除は個々のreleaseで行う
    
#else  //USE_INDEPENDENT_RANDOM_GENERATOR not defined ..................
    if (_gen)
    {
        delete _gen;
    }
#endif //USE_INDEPENDENT_RANDOM_GENERATOR ..............................
}

//======================================================================
RandomGenerator* Random::randomGenerator()
{
#ifdef USE_INDEPENDENT_RANDOM_GENERATOR //..............................
    
    RandomGenerator* gen = new XSadd();
    gen->setSeed((_seed+_numAssigned)%UINT_MAX);

#ifdef _OPENMP
#pragma omp atomic
#endif //_OPENMP
    _numAssigned++;

    return gen;
   
#else  //USE_INDEPENDENT_RANDOM_GENERATOR not defined ..................

    // 生成は全体のinitializeで行う
    return _gen;
   
#endif //USE_INDEPENDENT_RANDOM_GENERATOR ..............................
}

//======================================================================
void Random::releaseRandomGenerator(RandomGenerator* gen)
{
#ifdef USE_INDEPENDENT_RANDOM_GENERATOR //..............................
 
    if (gen)
    {
        delete gen;
    }

#ifdef _OPENMP
#pragma omp atomic
#endif //_OPENMP
    _numReleased++;

#else //USE_INDEPENDENT_RANDOM_GENERATOR not defined ...................

    // 削除は全体のfinalizeで行う

#endif //USE_INDEPENDENT_RANDOM_GENERATOR ..............................
}

//======================================================================
int Random::uniform(RandomGenerator* gen,
                    int max)
{
    return gen->nextInt(max);
}

//======================================================================
int Random::uniform(RandomGenerator* gen,
                    int min, int max)
{
    assert(min <= max);
    return min + uniform(gen, max - min);
}

//======================================================================
double Random::uniform(RandomGenerator* gen)
{
    return gen->nextReal();
}

//======================================================================
double Random::uniform(RandomGenerator* gen,
                       double min, double max)
{
    return min + gen->nextReal(max - min);
}

//======================================================================
double Random::normal(RandomGenerator* gen,
                      double mu, double sigma)
{
    return _normal(gen, mu, sigma);
}

//======================================================================
int Random::poisson(RandomGenerator* gen,
                    const double lambda)
{
    return _poisson(gen, lambda);
}

//----------------------------------------------------------------------
double Random::exponential(RandomGenerator* gen,
                           double lambda)
{
    return _exp(gen, lambda);
}

//======================================================================
bool Random::biasedCoinFlip(RandomGenerator* gen,
                            double p)
{
    return (uniform(gen) < p);
}

//======================================================================
vector<int> Random::randomOrder(RandomGenerator* gen,
                                int max)
{
    vector<int> order;
    order.reserve(max);
    for (int i=0; i<max; i++)
    {
        order.push_back(i);
    }
    if (order.size()>1)
    {
        // Fisher-Yatesアルゴリズム
        for (int i=order.size(); i>1; i--)
        {
            swap(order[i-1], order[uniform(gen, i)]); 
        }
    }
    return order;
}

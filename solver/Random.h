/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <xsadd.h>
#include <iosfwd>
#include <functional>
#include <cstddef>
#include <vector>
#include <functional>
#include <limits.h>
#include <cassert>
#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

// オブジェクトごとに独立した乱数の使用
/*
 * マルチスレッドで実行する場合でも再現性が必要な場合は
 * 以下のマクロを定義する
 */
#define USE_INDEPENDENT_RANDOM_GENERATOR

/**
 * @addtogroup RandomNumber
 * @brief 乱数生成モジュール
 */

//######################################################################
// 乱数生成器

/**
 * @addtogroup RandomGenerator
 * @brief 擬似乱数生成器
 * @ingroup RandomNumber
 */

//----------------------------------------------------------------------
/// 乱数生成器の抽象基底クラス
/** 
 * @ingroup RandomGenerator
 */
class RandomGenerator
{
public:
    RandomGenerator() {};
    virtual ~RandomGenerator() {};

    /// 乱数の種を設定する
    /**
     * @param seed 乱数の種
     */
    virtual void setSeed(unsigned long seed) = 0;

    /// [0, maxInt()] を返す
    /**
     * @return 0以上maxInt()以下のunsigned long型乱数
     */
    virtual unsigned long nextInt() = 0;

    /// [0, @p max) を返す
    /**
     * @param max 乱数の最大値を決める整数
     * @return 0以上@p max【未満】のunsigned long型乱数
     *
     * @attention 最大値は @p max - 1 であることに注意
     */
    virtual unsigned long nextInt(unsigned long max) = 0;

    /// nextInt() で返せる最大値を返す
    virtual unsigned long maxInt() const = 0;

    /// [0, 1] を返す
    /**
     * @return 0以上1以下のdouble型乱数
     */
    virtual double nextReal() = 0;

    /// [0, @p max] を返す
    /**
     * @param max 乱数の最大値
     * @return 0以上@p max 以下のdouble型乱数
     */
    virtual double nextReal(double max) = 0;
};

//----------------------------------------------------------------------
/// 標準乱数による乱数生成器
/** 
 * 標準乱数生成関数 std::rand() のラッパ．
 *
 * @attention 
 * たいていのプラットフォームでは線形合同法によるため質がよくない．
 * また内部の static 変数を状態変数として利用するので注意が必要
 *
 * @ingroup RandomGenerator
 */
class StdRand : public RandomGenerator
{
public:
    StdRand() {}
    virtual ~StdRand() {}
    virtual void setSeed(unsigned long seed);
    virtual unsigned long nextInt();
    virtual unsigned long nextInt(unsigned long max);
    virtual unsigned long maxInt() const;
    virtual double nextReal();
    virtual double nextReal(double max);
};

//----------------------------------------------------------------------
/// 線形合同法による乱数生成器
/**
 * std::rand() とは独自の実装．
 * 移植性のため作成． 
 *
 * @ingroup RandomGenerator
 */
class LinearCongruential : public RandomGenerator
{
public:
    LinearCongruential(long seed = 0) : _x(seed) {}
    virtual ~LinearCongruential() {}
    virtual void setSeed(unsigned long seed);
    virtual unsigned long nextInt();
    virtual unsigned long nextInt(unsigned long max);
    virtual unsigned long maxInt() const;
    virtual double nextReal();
    virtual double nextReal(double max);
    
private:
    unsigned long _x;
};

//----------------------------------------------------------------------
/// メルセンヌ・ツイスタ法による乱数生成器
/**
 * 松本眞 ・西村拓士による乱数生成アルゴリズム
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt.html
 *
 * 詳細は lib/mt19937ar を参照．
 *
 * @ingroup RandomGenerator
 */
class MT19937 : public RandomGenerator
{
public:
    MT19937() : mti(N+1) {}
    virtual ~MT19937() {}
    virtual void setSeed(unsigned long seed);
    virtual unsigned long nextInt();
    virtual unsigned long nextInt(unsigned long max);
    virtual unsigned long maxInt() const;
    virtual double nextReal();
    virtual double nextReal(double max);

    /** 閉区間[0,0xffffffff].
     * used by another function */
    unsigned long nextInt32();
    /** 閉区間[0,0x7fffffff] */
    long nextInt31();
    /** 閉区間[0,1]の実数 */
    double nextReal1();
    /** 半閉区間[0,1)の実数 */
    double nextReal2();
    /** 開区間(0,1)の実数 */
    double nextReal3();
    /** 半閉区間[0,1)の実数．53bit の解像度を持つ */
    double nextRealRes53(void);
  
    friend std::istream& operator>>(std::istream& is, MT19937& gen);
    friend std::ostream& operator<<(std::ostream& os, MT19937& gen);
    
private:
    static const std::size_t N = 624;
    static const std::size_t M = 397;  
    /// constant vector a
    static const unsigned long MATRIX_A = 0x9908b0dfUL;
    /// most significant w-r bits
    static const unsigned long UPPER_MASK = 0x80000000UL;
    /// least significant r bits/
    static const unsigned long LOWER_MASK = 0x7fffffffUL;
    
    /// 内部状態を表す配列
    unsigned long mt[N];
    /// どうやって初期化されたかどうかを表す
    int mti;
};

//----------------------------------------------------------------------
/// XORSHIFT-add による乱数生成器
/**
 * 斎藤睦夫・松本眞による乱数生成アルゴリズム
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/XSADD/index-jp.html
 *
 * 詳細は lib/XSadd を参照．
 *
 * @ingroup RandomGenerator
 */
class XSadd : public RandomGenerator
{
public:
    XSadd() {}
    virtual ~XSadd() {}
    virtual void setSeed(unsigned long seed);
    virtual unsigned long nextInt();
    virtual unsigned long nextInt(unsigned long max);
    virtual unsigned long maxInt() const;
    virtual double nextReal();
    virtual double nextReal(double max);
private:
    // 内部状態を表す構造体
    xsadd_t xsa;
};

//######################################################################
// 確率分布に応じた乱数を生成するオブジェクト

/**
 * @addtogroup RandomDistribution
 * @brief 乱数の分布関数
 * @ingroup RandomNumber
 */

//----------------------------------------------------------------------
/// 正規分布関数オブジェクト
/** 
 * @f[
 *   f(x) = \frac{1}{\sqrt{2 \pi} \sigma} e^{-(x-\mu)^2 / 2 {\sigma}^2}
 * @f]
 *
 * Box-Muller 法（極座標法）による．
 * 独立した二系列の正規分布を生成する．
 *
 * @ingroup RandomDistribution
 */
class NormalDist
{
public:
    explicit NormalDist() : phase(false) {}

    /// 平均 @p mu, 標準偏差 @p sigma の分布
    double operator()(RandomGenerator* gen, double mu, double sigma);
    
private:
    /// 平均 0, 標準偏差 1 の分布
    double generate_0_1(RandomGenerator* gen);
  
private:
    double r1, r2;
    bool phase;
};

//----------------------------------------------------------------------
/// ポアソン分布関数オブジェクト
/**
 * 確率関数は以下になる.
 * @f[ f(x) = \frac{e^{-\lambda} {\lambda}^x}{x!} @f]
 * 平均 @f$ \lambda @f$,分散 @f${\lambda}^2 @f$.
 *
 * @ingroup RandomDistribution
 */
class PoissonDist
{
public:
    explicit PoissonDist() {}

    /// 平均 @p lambda の分布
    int operator()(RandomGenerator* gen, double lambda);
};

//----------------------------------------------------------------------
/// 指数分布関数オブジェクト
/**
 * 次の確率密度関数に従った分布.
 * @f[ f(x) = \lambda e^{- \lambda x} @f]
 * ただし,@f$ x \leq 0,\; \lambda > 0 @f$
 *
 * このとき平均 @f$ 1 / \lambda @f$,分散 @f$ 1 / {\lambda}^2 @f$ になる.
 *
 * @ingroup RandomDistribution
 */
class ExponentialDist
{
public:
    explicit ExponentialDist() {}

    /// 平均 1/@p lambda の分布
    double operator()(RandomGenerator* gen, double lambda);
};

//----------------------------------------------------------------------
/// 二項分布関数オブジェクト
/**
 *  p の確率で起こる事象が n 回繰り返されるときに，それが x 回起こる確率．
 *  以下の数式で与えられる．
 * @f[
 *   f(x) = {}_n\mathrm{C}_x p^x (1-p)^{n-x},\; x=0,1,\ldots,n,\; 0 < p < 1
 * @f]
 *
 * @ingroup RandomDistribution
 */
class BinomialDist
{
public:
    explicit BinomialDist() {}

    /// 試行回数@p nのうち，確率@p pの事象が発生する回数を返す
    int operator()(RandomGenerator* gen, int n, double p);
};

//######################################################################
// 乱数のインターフェース

/// さまざまな分布を扱える乱数クラス
/**
 *  利便性のため，このファイル中のモジュールをまとめて
 *  static メンバのみを持つクラスとして提供する．
 *  乱数生成器はデフォルトでXORSHIFT-addを用いる．
 *
 * @ingroup RandomNumber
 */
class Random
{
public:
    /// 乱数の種を設定する
    /**
     * @param seed 乱数の種
     */
    static void setSeed(unsigned long seed);

    /// 使用前の処理を行う
    static void initialize();

    /// 使用後の処理を行う
    static void finalize();

    /** @name 乱数生成器の操作 */
    //@{

    /// 乱数生成器を取得する
    /**
     * 個別の乱数生成器であればnewし，
     * 共通の乱数生成器であれば_refCountをインクリメントする
     * _refCountが0の場合はnewする
     */
    static RandomGenerator* randomGenerator();

    /// 乱数生成器を開放する
    /**
     * 個別の乱数生成器であればdeleteし，
     * 共通の乱数生成器であれば_refCountをデクリメントする
     * _refCountが0になった場合はdeleteする
     */
    static void releaseRandomGenerator(RandomGenerator* gen);

    //@}
    
    /** @name 様々な条件に従った乱数を返す */
    //@{

    /// [0, @p max) の一様分布に従う乱数を返す
    /**
     * @param max 乱数の最大値を決める整数
     * @return 0以上@p max【未満】の一様分布に従うint型乱数
     *
     * @attention RandomGenerator::nextInt(long) と異なりこちらは半開区間
     */
    static int uniform(RandomGenerator* gen, int max);

    /// [@p min, @p max) の一様分布に従う乱数を返す
    /**
     * @param min 乱数の最小値
     * @param max 乱数の最大値を決める整数
     * @return @p min 以上 @p max 【未満】の一様分布に従うint型乱数
     */
    static int uniform(RandomGenerator* gen, int min, int max);
  
    /// [0, 1) の一様分布にしたがう乱数を返す
    /**
     * @return 0以上1【未満】の一様分布に従うdouble型乱数
     */
    static double uniform(RandomGenerator* gen);

    /// [@p min, @p max) の一様分布に従う乱数を返す
    /**
     * @param min 乱数の最小値
     * @param max 乱数の最大値を決める実数
     * @return @p min 以上 @p max 【未満】の一様分布に従うdouble型乱数
     */
    static double uniform(RandomGenerator* gen, double min, double max);

    /// 平均 @p mu, 標準偏差 @p sigma のガウス分布に従う乱数を返す
    static double normal(RandomGenerator* gen, double mu, double sigma);

    /// 平均 @p lamda のポアソン分布に従う乱数を返す
    static int poisson(RandomGenerator* gen, double lambda);
  
    /// 平均 1/@p lambda の指数分布に従う乱数を返す
    static double exponential(RandomGenerator* gen, double lambda);

    //@}

    /** @name ユーティリティ */
    //@{

    /// 確率 @p p のテスト
    /**
     * 「表が出る確率@p pのコインを投げて表が出たかどうか」に相当する
     *
     * @param p trueを返す確率
     */
    static bool biasedCoinFlip(RandomGenerator* gen, double p);

    /// [0,max)の間の整数がランダムに入った数列を生成する
    /**
     * @p param max 並べ替える数の最大値を決める整数
     * @return ランダムに並べ替えられたvector
     */
    static std::vector<int> randomOrder(RandomGenerator* gen, int max);

    //@}

private:
    /// 乱数の種
    static unsigned long _seed;

#ifdef USE_INDEPENDENT_RANDOM_GENERATOR

    /// 割り当てた乱数生成器の個数
    static unsigned long _numAssigned;

    /// 開放した乱数生成器の個数
    static unsigned long _numReleased;

#else //USE_INDEPENDENT_RANDOM_GENERATOR not defined

    /// 共通の乱数生成器
    static RandomGenerator* _gen;

#endif //COMMON_RANDOM_GENERATOR

    /** @name 分布関数オブジェクト*/
    //@{
    static NormalDist _normal;   //!< 正規分布
    static PoissonDist _poisson; //!< ポアソン分布
    static ExponentialDist _exp; //!< 指数分布
    static BinomialDist _binom;  //!< 二項分布
    //@}
};
#endif //__RANDOM_H__

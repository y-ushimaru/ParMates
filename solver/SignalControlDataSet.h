/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SIGNALCONTROLDATASET_H__
#define __SIGNALCONTROLDATASET_H__

#include <algorithm>
#include <vector>
#include <functional>
#include "SignalControlData.h"
#include <iterator>

/// SignalControlDataの集合
/**
 * 指定の時刻にあわせた信号制御情報を得るために用いる
 * _dataSetはソート済みであることが前提
 * （データファイルそのものがソート済みであるから）
 *
 * @sa SignalControlData
 * @ingroup Signal
 */
class SignalControlDataSet
{
public:
    SignalControlDataSet();
    SignalControlDataSet(std::vector<SignalControlData>& dataSet);
    ~SignalControlDataSet();
  
    /// コピー演算子
    SignalControlDataSet &operator=(const SignalControlDataSet &dat);
  
    /// 時刻@p timeで有効な現示を返す
    const SignalControlData& aspectData(ulint time) const;

    unsigned int columnSize() const { return _dataSet.size();}

protected:
    std::vector<SignalControlData> _dataSet;

    /// _indexで現在カーソル位置を保持する
    /**
     * Signalの要請によって指す位置が改変されてしまう(const性が維持できない)
     * iteratorなのと時間がないのでmutableとしてしまうが、いずれ改善すべき
     */
    mutable std::vector<SignalControlData>::const_iterator _index;
};

//#######################################################

// 関数オブジェクト-データのリストの中からend()を見て最初にreferenceよりも
// 大きくなった要素の反復子を返す機能を実装するために作った。
class SignalControlDataSetCmp
    : public std::binary_function<const SignalControlData&, const ulint, bool>
{
public:
    result_type operator() (first_argument_type data,
                            second_argument_type reference) const
    {
        return (result_type) (!(data.end() > reference));
    }
};

#endif

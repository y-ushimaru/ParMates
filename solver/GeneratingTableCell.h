/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATING_TABLE_CELL_H__
#define __GENERATING_TABLE_CELL_H__

#include <string>
#include <vector>
#include <deque>
#include <cassert>
#include "TimeManager.h"
#include "OD.h"

/// ある期間における車両の発生データを格納するクラス
/**
 * GeneratingTableの構成要素となる
 *
 * volume() はODノード(厳密にはODノードに接続する単路)あたりの
 * 発生率(交通量)を[veh./hr]の形で返す
 *
 * @sa GeneratingTable
 * @ingroup IO
 */
class GeneratingTableCell
{
public:
    GeneratingTableCell();
    ~GeneratingTableCell(){};

    //==================================================================
    /** @name アクセッサ */
    /// @{
    
    /// 開始時刻を返す
    ulint begin() const
    {
        return _begin;
    }

    /// 終了時刻を返す
    ulint end() const
    {
        return _end;
    }

    /// 発生交通量 [veh./hour]を返す
    double volume() const
    {
        return _volume;
    }

    /// 発生済み車両台数を返す
    unsigned int generatedVolume() const
    {
        return _generatedVolume;
    }

    /// 車種IDを返す
    int vehicleType() const
    {
        return _vehicleType;
    }

    /// 出発地の交差点の識別番号を返す
    const std::string& origin() const
    {
        assert(_gates.size()>=2);
        return _gates[0];
    }

    /// 目的地の交差点の識別番号を返す
    const std::string& destination() const
    {
        assert(_gates.size()>=2);
        return _gates[_gates.size()-1];
    }
    
    /// 通過交差点を返す
    const std::vector<std::string>* gates() const
    {
        return &_gates;
    }

    /// @}

    //==================================================================
    /** @name グループ化されたODに関するアクセッサ */
    /// @{

    /// ODペアが指定されているか
    bool isPaired() const
    {
        return _isPaired;
    }

    /// ODペアが指定されていることを設定する
    void setPaired(bool isPaired)
    {
        _isPaired = isPaired;
    }

    /// 出発地がグループ化されているか
    bool isOriginGrouped() const
    {
        return _isOriginGrouped;
    }
    
    /// 出発地がグループ化されていることを設定する
    void setOriginGrouped(bool isOriginGrouped)
    {
        _isOriginGrouped = isOriginGrouped;
    }
    
    /// 目的地がグループ化されているか
    bool isDestinationGrouped() const
    {
        return _isDestinationGrouped;
    }

    /// 目的地がグループ化されていることを設定する
    void setDestinationGrouped(bool isDestinationGrouped)
    {
        _isDestinationGrouped = isDestinationGrouped;
    }

    ///@}

    //==================================================================
    /// 内部パラメータ値を一括して設定する
    bool setValue(ulint begin, ulint end,
                  double volume, int vehicleTYpe,
                  const std::vector<std::string>& gates);
 
    /// 発生済み車両台数をインクリメントする
    void incrementGeneratedVolume()
    {
        _generatedVolume++;
    }

    /// 情報を表示する
    void print() const;

protected:
    ulint _begin;                    //!< 開始時刻
    ulint _end;                      //!< 終了時刻
    double _volume;                  //!< 交通量[veh./hour]
    unsigned int _generatedVolume;   //!< 発生済み車両台数[veh.]
    int _vehicleType;                //!< 車種ID

    /** @name グループ化されたODを扱うための変数 */
    ///@{
    bool _isPaired;             //!< ODペアかどうか
    bool _isOriginGrouped;      //!< 出発地がグループ化されているかどうか
    bool _isDestinationGrouped; //!< 目的地がグループ化されているかどうか
    /// @}

    /// 通過交差点のID
    /**
     * 最初の要素が出発地，最後の要素が目的地を表す
     */
    std::vector<std::string> _gates; 
};

#endif // __GENERATING_TABLE_CELL_H__

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __GENERATINGTABLE_H__
#define __GENERATINGTABLE_H__

#include <string>
#include <vector>
#include <deque>
#include "TimeManager.h"
#include "OD.h"

class GeneratingTableCell;

/// 車両発生データ処理クラス
/**
 * 初期設定はGenerateVehicleIOでおこなう
 */
class GeneratingTable
{
public:
    GeneratingTable();
    ~GeneratingTable();
    
    /// 外部で作成したGeneratingTableCellを追加する
    void addGTCell(GeneratingTableCell* cell);
    
    /// 外部からGeneratingTableCellを直接作成する
    void createGTCell(ulint begin, ulint end,
                      double volume, int vehicleType,
                      const std::vector<std::string>& gates);
    
    /// 現在時刻に対して効力を持つCellを抽出する
    /**
     * 抽出後は_tableから取り除かれて_activatedTableに移る
     */
    void extractActiveGTCells
    (std::vector<const GeneratingTableCell*>* result_GTCells);

    /// 有効なCellを一度に抽出する
    /**
     * fixedTableのデバッグ用に用いる．
     * 抽出後は_tableから取り除かれて_activatedTableに移る．
     */
    void extractActiveGTCellsAllAtOnce
    (std::vector<const GeneratingTableCell*>* result_GTCells);

    /// ID@p intersectionIdで指定したODノードに関するCellを取得する
    void getValidGTCells
    (const std::string& intersectionId,
     std::vector<const GeneratingTableCell*>* result_GTCells) const;
    
    /// ID@p intersectionIdで指定したODノードに関するCellを取得する
    const GeneratingTableCell* validGTCell
    (const std::string& intersectionId) const;

    /// GTCellを整列する
    void sortGTCells();
    
    //==================================================================
public:
    /// 情報を表示する
    void print() const;
    
    //==================================================================
protected:
    /// GeneratingTableCellのコンテナ (activate前のセル)
    /**
     * 要素は開始時刻でソートする．
     * 先頭の要素を頻繁に削除する必要があるが，挿入する機会は少ない．
     */
    std::deque<GeneratingTableCell*> _table;

    /// GeneratingTableCellのコンテナ (activate後のセル)
    std::vector<GeneratingTableCell*> _activatedTable;
};

#endif // __GENERATINGTABLE_H__

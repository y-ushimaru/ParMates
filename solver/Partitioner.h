/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PARTITIONER_H__
#define __PARTITIONER_H__

#include <vector>
#include <string>
#include <map>

using namespace std;

class RoadMap;

// MATESの道路情報をMetisにより分割するクラス

class Partitioner
{
public:
  Partitioner(int numberOfProcessor);
  ~Partitioner(){};

  // RoadMapオブジェクトのセット
  void setRoadMap(RoadMap* roadMap);

  // MATESの道路ネットワーク情報を変換する
  //シミュレーション開始時に一度だけ呼び出される
  void convertNetwork();

  // 初期領域分割を行う
  void initPartitioning();

  // 再分割を行う
  void repartitioning();

  // 再分割を行うかどうかを決定する
  void partitioningDecision(vector<int>* load);

  // 再分割のフラグを設定する
  void setRepartitioningFlag(bool flag);

  // 再分割のフラグを返す
  bool repartitioningFlag();

  // マスタープロセッサのIDを返す
  int masterId();

  // 再分割した回数を返す
  int partitioningCount();

  // 再分割回数を増やす
  void incrementCount();

  // 不均衡度の時間推移を返す(結果出力用)
  vector<double>* imbarance();
private:

  // マスタープロセッサのID
  int _masterId;

  // 分割数
  int _np;

  // ロードマップ
  RoadMap* _roadMap;

  // 交差点IDの変換 original format -> metis format
  map<int,int> _idConverter;

  // 交差点IDの逆変換 original format <- metis format
  map<int,int> _idRestrator;

  // 再分割の回数
  int _partitioningCount;

  // 再分割の閾値
  double _threshold;

  // 再分割のフラグ
  bool _repartitioningFlag;

  // 不均衡度の時間推移
  vector<double> _imbarance;

};

#endif //__PARTITIONER_H__

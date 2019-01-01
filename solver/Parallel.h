/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PARALLEL_H__
#define __PARALLEL_H__

#include <vector>
#include <string>
#include <map>

class Simulator;
class Partitioner;

/// 並列処理の基底クラス
class Parallel
{
 public:
  Parallel();
   ~Parallel();

  // MPIの初期化を行う
  bool initialize();

  // MPIを終了させる
  bool finalize();

  // 通信処理を行う（メイン部分）
  void communicate();

  // プロセスIDを返す
  int myId();

  // プロセス数を返す
  int commSize();

  // 同期待ちを行う
  void barrier();

  // 車両データを送受信する
  void passingVehiclesData(std::vector<std::vector<double> >* data);

  // 負荷を送受信する
  void passingLoadData(int load);

  // フラグを送受信する
  void passingFlag(bool flag);

  // 再分割時に交差点要素ごとの車両台数を送受信する
  void passingDetailLoadData();

  // 再分割結果を送受信する
  void passingResult();

  // 車両生成オブジェクトを登録する
  void setSimulator(Simulator* sim);

  // 分割オブジェクトを返す
  Partitioner* partitioner();

  // 受信データを返す
  std::vector<std::vector<double> >* recvData();

  // マスタープロセスかどうかを返す
  bool isMaster();
  
  // 結果を出力する
  void writeResult();
  
  // 重み付けを計算する
  void calcWeight();

 protected:

  // プロセスID
  int _myId;

  // 並列数
  int _commSize;

  // 送信データ
  std::vector<std::vector<double> > _sendData;

  // 受信データ
  std::vector<std::vector<double> > _recvData;

  // シミュレータ本体
  Simulator* _sim;

  // 分割オブジェクト
  Partitioner* _partitioner;

  // マスタープロセスかどうか
  bool _isMaster;

  // エージェント通信台数
  int _sendAgentsNum;

  // 参照エージェント通信台数
  int _sendVirtualAgentsNum;

  // 発生台数
  int _generateAgentsNum;

  //
};

#endif //__PARALLEL_H__

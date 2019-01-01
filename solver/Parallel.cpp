#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
#include "Parallel.h"
#include "Simulator.h"
#include "RoadMap.h"
#include "Intersection.h"
#include "Section.h"
#include "ODNode.h"
#include "Vehicle.h"
#include "GVManager.h"
#include "ObjManager.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "ParallelConf.h"
#include "TimeManager.h"
#include "GenerateVehicleController.h"
#include "Partitioner.h"

using namespace std;

//==============================================================
Parallel::Parallel()
{
  _myId = 0;
  _commSize = 1;
  _isMaster = false;

  _sendAgentsNum = 0;
  _sendVirtualAgentsNum = 0;
}
//==============================================================
Parallel::~Parallel()
{
}
//==============================================================
bool Parallel::initialize()
{
  int argc;
  char** argv;
  int ierr;

  ierr = MPI_Init(&argc,&argv);

  if(ierr == MPI_SUCCESS)
  {
    ierr = MPI_Comm_rank(MPI_COMM_WORLD,&_myId);
    ierr = MPI_Comm_size(MPI_COMM_WORLD,&_commSize);
  }

  //分割オブジェクトを作成
  _partitioner = new Partitioner(_commSize);

  //マスタープロセスかどうかを設定する
  if(_myId == _partitioner->masterId())
  {
    _isMaster = true;
  }
  else
  {
    _isMaster = false;
  }

  if(ierr != MPI_SUCCESS)
  {
    cerr << "Initialization of Massage Passing is faliure!!" << endl;
    exit(0);
  }

  if(ierr == MPI_SUCCESS)
  {
    return true;
  }
  else
  {
    cerr << "Bcast of Massage Passing is faliure!!" << endl;
    exit(0);
  }
}
//==============================================================
bool Parallel::finalize()
{
  int ierr;

  ierr = MPI_Finalize();

  if(ierr == MPI_SUCCESS)
  {
    return true;
  }
  else
  {
    cerr << "Finalize of Massage Passing is faliure!!" << endl;
    exit(0);
  }
}

//==============================================================
void Parallel::communicate()
{
  //------------------------------------------------------------
  // 待機処理
  //------------------------------------------------------------
#ifdef MEASURE_WAIT

  // TimeManager::startClock("WAIT");
  
  // 待機時間の計測用（合計の計算時間は遅くなるので注意）
  // barrier();

  // TimeManager::stopClock("WAIT");

#endif //MEASURE_WAIT
  //------------------------------------------------------------
  // 再分割処理
  //------------------------------------------------------------

  TimeManager::startClock("REPARTITIONING");

  if(TimeManager::time()%SEND_LOAD_INTERVAL == 0)
  {
    // エージェント列を仮に更新する
    _sim->roadMap()->renewAgentLineTmp();

    // 負荷を通信する
    passingLoadData(_sim->roadMap()->countAllWeight());

    // 再分割フラグを通信する
    passingFlag(_partitioner->repartitioningFlag());
  }

#ifdef DYNAMIC_LOAD_BALANCING

  if(_partitioner->repartitioningFlag() &&
     TimeManager::time() >= START_REPARTITIONING_TIME &&
     TimeManager::time() <= END_REPARTITIONING_TIME)
  {
    // 再分割の回数をインクリメントする
    _partitioner->incrementCount();

    // 重み付けを集約する
    // ファイルへの書き込み処理のため順番に行う必要がある
    for(int i = 0; i < _commSize; i++)
    {
      if(_myId == i)
      {
        _sim->roadMap()->weightToInterFactor();
      }
      barrier();
    }

    // METIS再分割
    if(_isMaster)
    {
      _partitioner->repartitioning();
    }
    barrier();

    // 送信するエージェントのデータを決定する
    _sim->roadMap()->decideSendDataInRepartitioning();

    // データを送受信する
    passingVehiclesData(_sim->roadMap()->sendData());

    // エージェントを再構築する
    for (int processId = 0; processId < _commSize; processId++)
    {
      if(_recvData[processId].size()!=0)
      _sim->generateVehicleController()->rebuildVehicles(&(_recvData[processId]));
    }

    // 担当プロセスIDを設定する
    _sim->roadMap()->setProcessId();

    // 再分割フラグをOFFにする
    _partitioner->setRepartitioningFlag(false);
  }

#endif //DYNAMIC_LOAD_BALANCING

  if(TimeManager::time()%(MEASURE_INTERVAL) == 0)
  {
    // エージェント列を仮に更新する
    _sim->roadMap()->renewAgentLineTmp();

    CITRMAPI iti = _sim->roadMap()->intersections()->begin();
    while (iti != _sim->roadMap()->intersections()->end())
    {
      if(TimeManager::time()%(SEND_LOAD_INTERVAL) == 0)
      {
        (*iti).second->clearWeight();
      }

      if((*iti).second->processId() == _myId)
      {
#ifdef WEIGHT_BY_AGENTS
         (*iti).second->addWeight((*iti).second->countAgentsNum());
#endif // WEIGHT_BY_AGENTS

#ifdef WEIGHT_BY_CALCTIME
         (*iti).second->addWeight((*iti).second->countCalcTime());
#endif // WEIGHT_BY_CALCTIME
      }
      iti++;
    }
  }

  TimeManager::stopClock("REPARTITIONING");

  //------------------------------------------------------------
  // 境界の処理
  //------------------------------------------------------------
  TimeManager::startClock("SHARED_SECTION");

  // 車両データの取得
  _sim->roadMap()->decideSendDataInSharedSection();

  // 車両データの通信
  passingVehiclesData(_sim->roadMap()->sendData());

  // エージェントを再構築する
  for (int processId = 0; processId < _commSize; processId++)
  {
    if(_recvData[processId].size()!=0)
    _sim->generateVehicleController()->rebuildVehicles(&(_recvData[processId]));
  }

  TimeManager::stopClock("SHARED_SECTION");
}

//==============================================================
int Parallel::myId()
{
  return _myId;
}

//==============================================================
int Parallel::commSize()
{
  return _commSize;
}

//==============================================================
void Parallel::barrier()
{
  MPI_Barrier(MPI_COMM_WORLD);
}

//==============================================================
void Parallel::passingVehiclesData(std::vector<std::vector<double> >* sendData)
{
  //------------------------------------------------------------
  // 車両データの初期化
  //------------------------------------------------------------
  // 送信データ本体
  _sendData = *sendData;

  // 受信データの初期化
  _recvData.clear();
  _recvData.resize(_commSize);

  // 送受信のリクエストを格納するベクター
  vector<MPI::Request> requests;

  //------------------------------------------------------------
  // データ数の送受信
  //------------------------------------------------------------
  vector<int> _sendDataNum;
  vector<int> _recvDataNum;

  _sendDataNum.clear();
  _recvDataNum.clear();

  // sendDataNumの作成とrecvDataNumの初期化
  for (unsigned int i=0; i<_commSize; i++)
  {
      _sendDataNum.push_back(_sendData[i].size());
      _recvDataNum.push_back(0);
  }

  requests.clear();

  for (unsigned int _opponentId=0; _opponentId<_commSize; _opponentId++)
  {

    if(_opponentId == _myId)
    {
      continue;
    }

    int sendTag  = _myId * _commSize + _opponentId + TimeManager::time();
    int recvTag  = _opponentId * _commSize + _myId + TimeManager::time();

    // 送信処理
    requests.push_back(MPI::COMM_WORLD.Isend(&_sendDataNum[_opponentId], 1, MPI::INT, _opponentId, sendTag));

    // 受信処理
    requests.push_back(MPI::COMM_WORLD.Irecv(&_recvDataNum[_opponentId], 1, MPI::INT, _opponentId, recvTag));
  }

  // vectorを配列に変換してWaitall処理
  MPI::Request::Waitall(requests.size(), requests.data());

  //データ数の確認
  //   if(TimeManager::time()%100000==0)
  // {
  //   cout<<"myId:"<<_myId;
  //   for (unsigned int j=0; j<_commSize; j++)
  //   {
  //     cout<<" "<<_recvDataNum[j]<<" ";
  //   }
  //   cout<<endl;
  // }

  //------------------------------------------------------------
  // データ本体の送受信
  //------------------------------------------------------------
  // 受信データの初期化
  for(int i=0; i<_commSize; i++)
  {
    for(int j=0; j<_recvDataNum[i]; j++)
    {
      _recvData[i].push_back(-1);
    }
  }

  requests.clear();

  for (unsigned int _opponentId=0; _opponentId<_commSize; _opponentId++)
  {

    if(_opponentId == _myId)
    {
      continue;
    }

    int sendTag  = _myId * _commSize + _opponentId + TimeManager::time();
    int recvTag  = _opponentId * _commSize + _myId + TimeManager::time();

    // 送信処理
    if(_sendDataNum[_opponentId]!=0)
    {
      int sendAgentsNum = 0;
      int sendVirtualAgentsNum = 0;
      for(int i = 0;i<_sendDataNum[_opponentId];i++)
      {
        if(_sendData[_opponentId][i]==REAL_VEHICLE)
        sendAgentsNum++;

        if(_sendData[_opponentId][i]==VIRTUAL_VEHICLE)
        sendVirtualAgentsNum++;
      }

      // 結果出力用
      _sendAgentsNum += sendAgentsNum;
      _sendVirtualAgentsNum += sendVirtualAgentsNum;

      // 送信先と送信数の確認
      // if(sendAgentsNum != 0)
      // {
      //   cout<< _myId << "->" << _opponentId << " "
      //       << "agents:" << sendAgentsNum << endl;
      // }
      // if(sendVirtualAgentsNum != 0)
      // {
      //   cout<< _myId << "->" << _opponentId << " "
      //       << "virtualAgents:" << sendVirtualAgentsNum << endl;
      // }

      requests.push_back(MPI::COMM_WORLD.Isend(&_sendData[_opponentId][0], _sendDataNum[_opponentId], MPI::DOUBLE, _opponentId, sendTag));
    }

    // 受信処理
    if(_recvDataNum[_opponentId]!=0)
    {
      // 受信先と受信数の確認
      // cout<<_myId<<"<-"<<_opponentId<<" "<<_recvDataNum[_opponentId]<<endl;

      requests.push_back(MPI::COMM_WORLD.Irecv(&_recvData[_opponentId][0], _recvDataNum[_opponentId], MPI::DOUBLE, _opponentId, recvTag));
    }
  }

  // vectorを配列に変換してWaitall処理
  MPI::Request::Waitall(requests.size(), requests.data());
}

//==============================================================
void Parallel::passingLoadData(int load)
{
  //------------------------------------------------------------
  // 全プロセスからマスターに負荷を送る p1,p2,・・・-> p0
  //------------------------------------------------------------

  // 送受信のリクエストを格納するベクター
  vector<MPI::Request> requests;
  // マスターID
  int _masterId = 0;
  // 送信バッファ
  int sendLoad = load;
  // 受信バッファ
  vector<int> recvLoad;

  // 送信プロセス(p1,p2,・・・)
  if(_myId != _masterId)
  {
    int sendTag = _myId;

    // 受信処理
    requests.push_back(MPI::COMM_WORLD.Isend(&sendLoad, 1, MPI::INT, _masterId, sendTag));
  }

  // 受信プロセス(p0)
  if(_myId == _masterId)
  {
    // 負荷の初期化
    for(unsigned int i=0; i<_commSize; i++)
    {
      recvLoad.push_back(0);
    }
    // 自分の負荷を格納する
    recvLoad[_masterId]=load;

    // 送信処理
    for (unsigned int _opponentId=0; _opponentId<_commSize; _opponentId++)
    {
      if(_opponentId==_masterId)
      continue;

      int recvTag  = _opponentId;

      requests.push_back(MPI::COMM_WORLD.Irecv(&recvLoad[_opponentId], 1, MPI::INT, _opponentId, recvTag));
    }
  }

  // vectorを配列に変換してWaitall処理
  MPI::Request::Waitall(requests.size(), requests.data());

  if(_myId == _masterId)
  {
    // partitioningオブジェクトに渡す
    _partitioner->partitioningDecision(&recvLoad);

    // データの確認
    int agentsNum = 0;
    for(unsigned int i=0; i<_commSize; i++)
    {
      cout<<recvLoad[i]<<" ";
      agentsNum += recvLoad[i];
    }
    cout<<agentsNum<<endl;
  }
}

//==============================================================
void Parallel::passingFlag(bool flag)
{
  //------------------------------------------------------------
  // マスターから全プロセスに再分割を行うかのフラグを送る  p0 -> p1,p2,・・・
  //------------------------------------------------------------

  // 送受信のリクエストを格納するベクター
  vector<MPI::Request> requests;
  // マスターID
  int _masterId = 0;
  // 送信バッファ
  bool sendFlag = flag;
  // 受信バッファ
  bool recvFlag = false;

  // 送信プロセス(p0)
  if(_myId == _masterId)
  {
    // 自分のフラグを格納する
    recvFlag = sendFlag;

    for (unsigned int _opponentId=0; _opponentId<_commSize; _opponentId++)
    {
      if(_opponentId==_masterId)
      continue;

      int sendTag  = _opponentId;

      requests.push_back(MPI::COMM_WORLD.Isend(&sendFlag, 1, MPI::BOOL, _opponentId, sendTag));
    }
  }

  // 受信プロセス(p1,p2,・・・)
  if(_myId != _masterId)
  {
    int recvTag = _myId;

    requests.push_back(MPI::COMM_WORLD.Irecv(&recvFlag, 1, MPI::BOOL, _masterId, recvTag));
  }

  // vectorを配列に変換してWaitall処理
  MPI::Request::Waitall(requests.size(), requests.data());

  // フラグを格納
  _partitioner->setRepartitioningFlag(recvFlag);
}

//==============================================================
void Parallel::passingDetailLoadData()
{
  //------------------------------------------------------------
  // 全プロセスからマスターに交差点ごとの負荷を送る p1,p2,・・・-> p0
  //------------------------------------------------------------
}

//==============================================================
void Parallel::passingResult()
{
  //------------------------------------------------------------
  // マスターから全プロセスに再分割結果を送る p0 -> p1,p2,・・・
  //------------------------------------------------------------
}

//==============================================================
  void Parallel::setSimulator(Simulator* sim)
  {
    _sim = sim;
  }

//==============================================================
std::vector<std::vector<double> >* Parallel::recvData()
{
  return &_recvData;
}

//==============================================================
Partitioner* Parallel::partitioner()
{
  return _partitioner;
}

//==============================================================
bool Parallel::isMaster()
{
  return _isMaster;
}

//==============================================================
void Parallel::writeResult()
{
  string dir = GVManager::getString("PARALLEL_OUTPUT_DIRECTORY");

  // 負荷の不均衡度
  if(_isMaster)
  {
    stringstream imbaranceStream;
    imbaranceStream << dir << "imbarance.txt";

    ofstream imbarance(imbaranceStream.str().c_str());

    vector<double>* _imbarance = _partitioner->imbarance();
    vector<double>::iterator iti = (*_imbarance).begin();

    int t = 1;
    while(iti != (*_imbarance).end())
    {
      imbarance << t*100 << " " << (*iti) << endl;
      cout << t*100 << " " << (*iti) << endl;
      iti++;
      t++;
    }

    imbarance << "partitioningCount:" << _partitioner->partitioningCount() << endl;
  }

  // 移動台数
  stringstream passingNumStream;
  passingNumStream << dir << "passingNum.txt";

  ofstream passingNum(passingNumStream.str().c_str(),ios::app);

  for(int i = 0; i < _commSize; i++)
  {
    if(_myId == i)
    {
      passingNum << _myId << " " << _sendAgentsNum << " " << _sendVirtualAgentsNum << endl;
      cout << _myId << " " << _sendAgentsNum << " " << _sendVirtualAgentsNum << endl;
    }
    barrier();
  }

  // 合計処理台数
  ObjManager::printNumVehicle();
}

//==============================================================
void Parallel::calcWeight()
{
  CITRMAPI iti = _sim->roadMap()->intersections()->begin();
  while (iti != _sim->roadMap()->intersections()->end())
  {
    if((*iti).second->processId() == _myId)
    {
#ifdef WEIGHT_BY_AGENTS
       (*iti).second->addWeight((*iti).second->countAgentsNum());
#endif // WEIGHT_BY_AGENTS

#ifdef WEIGHT_BY_CALCTIME
       (*iti).second->addWeight((*iti).second->countCalcTime());
#endif // WEIGHT_BY_CALCTIME
    }
    iti++;
  }
}
//==============================================================
double getTime()
{
  // 現在時刻を取得(microsecond)
  struct::timespec getTime;
  clock_gettime(CLOCK_MONOTONIC, &getTime);
 
  return (getTime.tv_sec + getTime.tv_nsec*1e-9)*1000000;
}

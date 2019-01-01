#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <map>
#include <algorithm>
#include <numeric>
#include "Partitioner.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include "GVManager.h"
#include "RoadMap.h"
#include "TimeManager.h"
#include "ParallelConf.h"

using namespace std;

//==============================================================
Partitioner::Partitioner(int numberOfProcessor)
{
  _np = numberOfProcessor;
  _partitioningCount = 0;
  _threshold = 1.10;
  _masterId = 0;
}

//======================================================================
void Partitioner::convertNetwork()
{
  string network;
  GVManager::getVariable("MAP_NETWORK_FILE", &network);
  ifstream inet1(network.c_str(), ios::in);

  string networkGraph;
  GVManager::getVariable("NETWORK_GRAPH_FILE", &networkGraph);
  ofstream onet(networkGraph.c_str());

  vector<int> ids;
  string str;
  int NumNodes = 0;
  int NumLinks = 0;
  int NumNexts = 0;

  while(inet1.good())
  {
    getline(inet1,str);
    AmuStringOperator::getAdjustString(&str);
    if(!str.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str, ',');
      assert(tokens.size() >2 );
      NumNodes += 1;
      NumNexts += tokens.size() - 2 ;

      ids.push_back(atoi(tokens[0].c_str()));
    }
  }

  assert(NumNexts % 2 == 0);

  NumLinks = NumNexts / 2;

  onet << NumNodes << " " << NumLinks << endl;

  sort(ids.begin(),ids.end());

  for(unsigned int i=0; i<ids.size();i++)
  {
    _idConverter[ids[i]] = i+1;
    _idRestrator[i+1] = ids[i];
  }

  vector<string> metisline;
  metisline.resize(NumNodes);

  ifstream inet2(network.c_str(), ios::in);

  while(inet2.good())
  {
    getline(inet2,str);
    AmuStringOperator::getAdjustString(&str);
    if(!str.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str, ',');

      int line = _idConverter[atoi(tokens[0].c_str())];
      metisline[line-1] = str;
    }
  }

  for(unsigned int i=0; i<metisline.size(); i++)
  {
    vector<string> tokens;
    AmuStringOperator::getTokens(&tokens, metisline[i], ',');
    for(unsigned int j=2; j< tokens.size(); j++)
    {
      int originalFormatId = atoi(tokens[j].c_str());
      int metisFormatId = _idConverter[originalFormatId];

      onet << " " << metisFormatId;
    }
    onet << endl;
  }

  inet1.close();
  inet2.close();
  onet.close();
}

//======================================================================
void Partitioner::initPartitioning()
{
  stringstream metisStream;
  int gpmetis;

  stringstream initStream;
  initStream << GVManager::getString("NETWORK_GRAPH_FILE") << ".init";
  ifstream ifs(initStream.str());

#ifdef INIT_NETWORK_GRAPH
  // 入力ファイルが存在する場合
  if(!ifs.is_open())
  {
    cout << "No init network graph file." << endl;
    exit(0);
  }

  metisStream << GVManager::getString("NETWORK_GRAPH_DIRECTORY") << "gpmetis " << initStream.str() << " " << _np;
  cout << "Use init network graph file." << endl;

  gpmetis = system(metisStream.str().c_str());

  stringstream mvStream;
  mvStream << "mv" << " " << GVManager::getString("NETWORK_GRAPH_FILE") << ".init.part." << _np << " " << GVManager::getString("NETWORK_GRAPH_FILE") << ".part." << _np;
  system(mvStream.str().c_str());
#endif // INIT_NETWORK_GRAPH

#ifndef INIT_NETWORK_GRAPH
  // 入力ファイルが存在しない場合
  metisStream << GVManager::getString("NETWORK_GRAPH_DIRECTORY") << "gpmetis " << GVManager::getString("NETWORK_GRAPH_FILE") << " " << _np;
  cout << "No init network graph file." << endl;
  gpmetis = system(metisStream.str().c_str());
#endif // INIT_NETWORK_GRAPH

  if(gpmetis != 0)
  {
    cout << "warning :: put gpmetis in networkGraph directory." << endl;
    exit(0);
  }

  // 分割後のオリジナルファイル
  stringstream partTmpStream;
  partTmpStream << GVManager::getString("NETWORK_GRAPH_FILE") << ".part." << _np;
  ifstream partTmp(partTmpStream.str().c_str());

  // 交差点と対応させたファイル
  stringstream partStream;
  partStream << GVManager::getString("NETWORK_GRAPH_FILE") << "." << _partitioningCount <<".part." << _np;
  ofstream part(partStream.str().c_str());

  string str;
  int line = 1;
  while(partTmp.good())
  {
    getline(partTmp,str);
    if(!str.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str, " ");

      stringstream s;
      s << _idRestrator[line];

      part <<
      AmuConverter::formatId(s.str().c_str(), NUM_FIGURE_FOR_INTERSECTION) << "," << tokens[0] << endl;
      line++;
    }
  }
  remove(partTmpStream.str().c_str());
}

//======================================================================
void Partitioner::repartitioning()
{
  //重み付けと010を入力ファイルに追加
  stringstream weightTmpStream;
  weightTmpStream.str("");
  weightTmpStream << GVManager::getString("NETWORK_GRAPH_FILE") << ".tmp";

  ifstream weightTmp(weightTmpStream.str().c_str());
  string str1;
  map<int,int> counter;
  while(weightTmp.good())
  {

    getline(weightTmp,str1);
    if(!str1.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str1, ' ');
      counter.insert(make_pair(atoi(tokens[0].c_str()),atoi(tokens[1].c_str())));
    }
  }

  // counterの確認
  // map<int,int>::iterator itc =  counter.begin();
  // while(itc != counter.end())
  // {
  //   cout<<"counter:"<<itc->first<<" "<<itc->second<<endl;
  //   itc++;
  // }

  string str2;
  ifstream network(GVManager::getString("NETWORK_GRAPH_FILE").c_str());

  stringstream weightStream;
  weightStream << GVManager::getString("NETWORK_GRAPH_FILE")
             << "." <<_partitioningCount;

  ofstream weight(weightStream.str().c_str());
  int line = 0;
  while(network.good())
  {
    getline(network,str2);
    if(!str2.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str2, ',');
      string convertId;
      if(!tokens[0].empty())
      {
        vector<string> token;
        AmuStringOperator::getTokens(&token, tokens[0], ' ');

        convertId = token[1];

      }
      if(line == 0)
      weight<<tokens[0]<<" "<<"010"<<endl;

      else
      weight<<counter[_idRestrator[line]]
      <<tokens[0]<<endl;
    }
    line++;
  }

  //再分割
  stringstream metisStream;
  metisStream << GVManager::getString("NETWORK_GRAPH_DIRECTORY")
               << "gpmetis" << " " << weightStream.str() << " " << _np;

  system(metisStream.str().c_str());
  remove(weightStream.str().c_str());
  remove(weightTmpStream.str().c_str());

  // マッピング処理
  stringstream beforePartStream;
  stringstream afterPartStream;

  beforePartStream <<  GVManager::getString("NETWORK_GRAPH_FILE")
             << "." << _partitioningCount-1 << ".part." << _np;
  afterPartStream <<  GVManager::getString("NETWORK_GRAPH_FILE")
             << "." << _partitioningCount << ".part." << _np;

  ifstream beforePart(beforePartStream.str().c_str());
  ifstream afterPart(afterPartStream.str().c_str());

  vector<string> before;
  vector<string> after;

  string str3;
  while(beforePart.good())
  {
    getline(beforePart,str3);
    AmuStringOperator::getAdjustString(&str3);
    if(!str3.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str3, ',');
      before.push_back(tokens[1]);
    }
  }

  string str4;
  while(afterPart.good())
  {
    getline(afterPart,str4);
    AmuStringOperator::getAdjustString(&str4);
    if(!str4.empty())
    {
      vector<string> tokens;
      AmuStringOperator::getTokens(&tokens, str4, ',');
      after.push_back(tokens[0]);
    }
  }

  int numNodes = before.size();
  vector<int> similarity(_np*_np,0);
  for(int i = 0; i < numNodes; i++)
  {
    int originalId = _idRestrator[i+1];

    int b = atoi(before[i].c_str());
    int a = atoi(after[i].c_str());

    similarity[b*_np + a] += counter[originalId];
  }

  vector<double> Lstatic(_np,0);
  vector<double> Ldynamic(_np,0);
  for(int i=0;i<_np;i++)
  {
    for(int j=0;j<_np;j++)
    {
      Lstatic[i] += similarity[_np*i+j];
      Ldynamic[i] += similarity[i+j*_np];
    }
  }

  double smax=*max_element(Lstatic.begin(),Lstatic.end())*0.01;
  double save=accumulate(Lstatic.begin(), Lstatic.end(), 0)/Lstatic.size()*0.01;

  double dmax=*max_element(Ldynamic.begin(),Ldynamic.end())*0.01;
  double dave=accumulate(Ldynamic.begin(), Ldynamic.end(), 0)/Ldynamic.size()*0.01;


  double staticImbarance=smax/save;
  double dynamicImbarance=dmax/dave;

  cout<<"beforeImbarance"<<staticImbarance<<endl;
  cout<<"afterImbarance"<<dynamicImbarance<<endl;


  map<int,int> mapping;

  int loadMax = 0;
  while(true)
  {
    loadMax=*max_element(similarity.begin(),similarity.end());

    if(loadMax==-1)
      break;

    for(int i=0;i<_np*_np;i++)
    {

      if(similarity[i]==loadMax)
      {
        int beforeId=i/_np;
        int afterId=i%_np;

        mapping[afterId]=beforeId;

        for(int j=0;j<_np*_np;j++)
        {
          if(j/_np==i/_np||j%_np==i%_np)
          similarity[j]=-1;
        }
      }
    }
  }

  stringstream afterMapStream;
  afterMapStream << afterPartStream.str();

  ofstream afterMap(afterMapStream.str().c_str());
  for(int i = 0;i < numNodes; i++)
  {
    stringstream s;
    s << _idRestrator[i+1];

    int beforeMap = atoi(after[i].c_str());

    afterMap <<
    AmuConverter::formatId(s.str().c_str(), NUM_FIGURE_FOR_INTERSECTION) << "," << mapping[beforeMap] << endl;
  }

  remove(beforePartStream.str().c_str());
}

//======================================================================
void Partitioner::partitioningDecision(vector<int>* load)
{
  double imbarance = 1;
  double max = *max_element(load->begin(),load->end());
  double ave = accumulate(load->begin(), load->end(), 0)/load->size();

  if(ave != 0)
    imbarance = max/ave;

  _imbarance.push_back(imbarance);

  cout<<"Imbarance:"<<imbarance<<endl;

  if(imbarance > _threshold)
  {
    _repartitioningFlag = true;
  }
  else
  {
    _repartitioningFlag = false;
  }
}
//======================================================================
void Partitioner::setRepartitioningFlag(bool flag)
{
  _repartitioningFlag = flag;
}
//======================================================================
bool Partitioner::repartitioningFlag()
{
  return _repartitioningFlag;
}
//======================================================================
int Partitioner::masterId()
{
  return _masterId;
}
//======================================================================
int Partitioner::partitioningCount()
{
  return _partitioningCount;
}

//======================================================================
void Partitioner::incrementCount()
{
  _partitioningCount++;
}

//======================================================================
vector<double>* Partitioner::imbarance()
{
  return &_imbarance;
}

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "GenerateVehicleIO.h"
#include "GenerateVehicleController.h"
#include "Simulator.h"
#include "RoadMap.h"
#include "Intersection.h"
#include "ODNode.h"
#include "ODNodeGroup.h"
#include "RoutingManager.h"
#include "GVManager.h"
#include "TimeManager.h"
#include "Conf.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <climits>

using namespace std;

//======================================================================
GenerateVehicleIO::GenerateVehicleIO()
{
    _sim = NULL;
}

//======================================================================
GenerateVehicleIO& GenerateVehicleIO::instance()
{
    static GenerateVehicleIO instance;
    return instance;
}

//======================================================================
void GenerateVehicleIO::readExcludedODNodes
(vector<string>* result_excludedStarts,
 vector<string>* result_excludedGoals)
{
    // 除外ファイルの読み込み
    string fODExFile;
    GVManager::getVariable("OD_NODE_EXCLUSION_FILE", &fODExFile);
    ifstream inODExFile(fODExFile.c_str(), ios::in);
    if (!inODExFile.good())
    {
        cout << endl
             << "no ODNode exclusion file:" << endl
             << "  " << fODExFile << endl;
    }
    
    string str;
    while (inODExFile.good())
    {
        getline(inODExFile, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            AmuStringOperator::getTokens(&tokens, str, ',');
            assert(tokens.size()==2);
            
            // 第1カラムは"O", "D", "OD"のいずれか
            // 第2カラムは交差点ID
            if (tokens[0]=="O")
            {
                result_excludedStarts->push_back
                    (AmuConverter::formatId
                     (tokens[1],
                      NUM_FIGURE_FOR_INTERSECTION));
            }
            else if (tokens[0]=="D")
            {
                result_excludedGoals->push_back
                    (AmuConverter::formatId
                     (tokens[1],
                      NUM_FIGURE_FOR_INTERSECTION));
            }
            else if (tokens[0]=="OD")
            {
                result_excludedStarts->push_back
                    (AmuConverter::formatId
                     (tokens[1],
                      NUM_FIGURE_FOR_INTERSECTION));
                result_excludedGoals->push_back
                    (AmuConverter::formatId
                     (tokens[1],
                      NUM_FIGURE_FOR_INTERSECTION));
            }
        }
    }
    inODExFile.close();
    
    cout << endl << "*** Excluded Random Origin ***" << endl;
    for (unsigned int i=0; i<result_excludedStarts->size(); i++)
    {
        cout << (*result_excludedStarts)[i] << endl;
    }
    cout << "*** Excluded Random Destination ***" << endl;
    for (unsigned int i=0; i<result_excludedGoals->size(); i++)
    {
        cout << (*result_excludedGoals)[i] << endl;
    }
    cout << endl;
}

//======================================================================
void GenerateVehicleIO::readODGroupFile
(GenerateVehicleController* controller)
{
    string fileName = GVManager::getString("OD_GROUP_FILE");

    fstream fin;
    fin.open(fileName.c_str());
    if (!fin.good())
    {
        cout << endl
             << "no od group file:" << endl
             << "  " << fileName << endl;
        return;
    }

    string str;
    vector<string> tokens;
    int numElements;

    ODNodeGroup* target;
    RoadMap* roadMap = controller->roadMap();
    
    /*
     * 指定モード(行を越えて状態を保存するため必要)
     * 0: 無指定
     * 1: _odStart
     * 2: _odGoal
     * 3: _odPair
     */
    int mode = 0;

    while (fin.good())
    {
        getline(fin, str);
        AmuStringOperator::getAdjustString(&str);
        if (str.empty())
        {
            continue;
        }

        AmuStringOperator::getTokens(&tokens, str, ',');
        if (tokens.size()==3)
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // グループの属性行
            
            // 第1カラムは"O", "D", "OD"のいずれか
            // 第2カラムはID
            if (tokens[0]=="O")
            {
                string id
                    = AmuConverter::formatId
                    (tokens[1], NUM_FIGURE_FOR_OD_GROUP);
                ODNodeGroup* group = new ODNodeGroup(id);
                controller->addODGroup("start", group);

                target = group;
                mode   = 1;
            }
            else if (tokens[0]=="D")
            {
                string id
                    = AmuConverter::formatId
                    (tokens[1], NUM_FIGURE_FOR_OD_GROUP);
                ODNodeGroup* group = new ODNodeGroup(id);
                controller->addODGroup("goal", group);

                target = group;
                mode   = 2;
            }
            else if (tokens[0]=="OD")
            {
                string id
                    = AmuConverter::formatId
                    (tokens[1], NUM_FIGURE_FOR_OD_GROUP);
                ODNodeGroup* group = new ODNodeGroup(id);
                controller->addODGroup("pair", group);
              
                target = group;
                mode   = 3;
            }
            else
            {
                cerr << "ERROR: 1st colomun must be "
                     << "\"O\", \"D\", or \"OD\" "
                     << "in OD group setting line." << endl;
                mode = 0;
                numElements = 0;
                continue;
            }

            // 第3カラムは要素の個数
            numElements = atoi(tokens[2].c_str());
        }

        else if (tokens.size()==1 && numElements > 0)
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // _odStart, _odGoal指定行
            string id = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            ODNode* node
                = dynamic_cast<ODNode*>(roadMap->intersection(id));
            if (!node)
            {
                cerr << "ERROR: non ODNode: " << id
                     << " cannot be added to OD group." << endl;
            }
            else
            {
                // 重みの算出
                if (mode==1)
                {
                    // Start
                    int level = controller->odNodeStartLevel(node);
                    if (level==-1)
                    {
                        cerr << "ERROR: origin node (" << id
                             << ") not found" << endl;
                    }
                    else
                    {
                        target->addODSolo(node,
                                          controller
                                          ->defaultTrafficVolume(level));
                    }
                }
                else if (mode==2)
                {
                    // Goal
                    int level = controller->odNodeGoalLevel(node);
                    if (level==-1)
                    {
                        cerr << "ERROR: destination node (" << id
                             << ") not found" << endl;
                    }
                    else
                    {
                        target->addODSolo(node,
                                          controller
                                          ->defaultTrafficVolume(level));
                    }
                }
            }
            numElements--;
        }
        else if (tokens.size()==2 && numElements > 0)
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // ODPair指定行
            string id0 = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            string id1 = AmuConverter::formatId
                (tokens[1], NUM_FIGURE_FOR_INTERSECTION);
            ODNode* node0
                = dynamic_cast<ODNode*>(roadMap->intersection(id0));
            ODNode* node1
                = dynamic_cast<ODNode*>(roadMap->intersection(id1));
            if (!node0 || !node1)
            {
                if (!node0)
                {
                    cerr << "ERROR: non ODNode: " << id0
                         << " cannot be added to OD group." << endl;
                }
                if (!node1)
                {
                    cerr << "ERROR: non ODNode: " << id1
                         << " cannot be added to OD group." << endl;
                }
            }
            else
            {
                // 重みの算出
                int level0 = controller->odNodeStartLevel(node0);
                int level1 = controller->odNodeGoalLevel(node1);
                if (level0==-1 || level1==-1)
                {
                    if (level0==-1)
                    {
                        cerr << "ERROR: origin node (" << id0
                             << ") not found" << endl;
                    }
                    if (level1==-1)
                    {
                        cerr << "ERROR: destination node (" << id1
                             << ") not found" << endl;
                    }
                }
                else
                {
                    double weight
                        = sqrt(controller
                               ->defaultTrafficVolume(level0)
                               * controller
                               ->defaultTrafficVolume(level1)); 
                    target->addODPair(node0, node1, weight);
                }
            }
            numElements--;
        }
    }
}

//======================================================================
void GenerateVehicleIO::setUpGeneratingTables
(GenerateVehicleController* controller)
{
    string fGenerateTable;
    string fDefaultGenerateTable;
    string fFixedGenerateTable;
    string fGroupedGenerateTable;
    GVManager::getVariable("GENERATE_TABLE",
                           &fGenerateTable);
    GVManager::getVariable("DEFAULT_GENERATE_TABLE",
                               &fDefaultGenerateTable);
    GVManager::getVariable("FIXED_GENERATE_TABLE",
                           &fFixedGenerateTable);
    GVManager::getVariable("GROUPED_GENERATE_TABLE",
                           &fGroupedGenerateTable);

    if (!fGenerateTable.empty())
    {
        _readGeneratingTableFile
            (const_cast<GeneratingTable*>(controller->table()),
             "normal", fGenerateTable);
    } 
    if (!fDefaultGenerateTable.empty())
    {
        _readGeneratingTableFile
            (const_cast<GeneratingTable*>(controller->defaultTable()),
             "normal", fDefaultGenerateTable);
    }
    if (!fFixedGenerateTable.empty())
    {
        _readGeneratingTableFile
            (const_cast<GeneratingTable*>(controller->fixedTable()),
             "fixed", fFixedGenerateTable);
    }
    if (!fGroupedGenerateTable.empty())
    {
        _readGeneratingTableFile
            (const_cast<GeneratingTable*>(controller->groupedTable()),
             "grouped", fGroupedGenerateTable);
    }
}

//======================================================================
void GenerateVehicleIO::setUpRandomGeneratingTable
(GenerateVehicleController* controller)
{
    const vector<ODNode*>* startNodes = controller->startNodes();

    for (unsigned int i=0; i<startNodes->size(); i++)
    {
        vector<string> dummyGates;
        dummyGates.clear();
        dummyGates.push_back((*startNodes)[i]->id());
        dummyGates.push_back("******");

        int startLebel
            = controller->odNodeStartLevel((*startNodes)[i]);
        if (startLebel<0)
        {
            /* 
             * ODノードからの流出点がない
             * = Destinationにしかなり得ない
             */
            continue;
        }
        double volume
            = controller->defaultTrafficVolume(startLebel)
            * GVManager::getNumeric("RANDOM_OD_FACTOR");

        /*
         * 発生交通量がファイルによって定義されている交差点は
         * ランダム発生の対象としない
         */
        vector<const GeneratingTableCell*> validGTCells;
        controller->table()
            ->getValidGTCells((*startNodes)[i]->id(),
                              &validGTCells);
        controller->defaultTable()
            ->getValidGTCells((*startNodes)[i]->id(),
                              &validGTCells);
        // _fixedTableで指定されていてもランダム発生させる
        // _groupedTableで指定されていてもランダム発生させる

        if (validGTCells.empty())
        {
            // 有効なGeneratingTableCellが見つからなかった場合
            /*
             * デフォルトの交通量を適用する
             */
            /**
             * @todo シミュレーションの最大時間をGVManagerで管理すべき
             */
            const_cast<GeneratingTable*>(controller->randomTable())
                ->createGTCell(0, 86400000, volume, 20,
                               dummyGates);
        }
        else
        {
            // 有効なGeneratingTableCellが見つかった場合
            /*
             * 指定時間外はデフォルト交通量を適用
             */
            /**
             * @todo 厳密に区間の計算をする必要があるか
             */
            ulint minStart = ULONG_MAX;
            ulint maxEnd = 0;
            for (unsigned int j=0; j < validGTCells.size(); j++)
            {
                if (validGTCells[j]->begin() < minStart)
                {
                    minStart = validGTCells[j]->begin();
                }
                if (validGTCells[j]->end() > maxEnd)
                {
                    maxEnd = validGTCells[j]->end();
                }
            }
            if (minStart > 0)
            {
             const_cast<GeneratingTable*>(controller->randomTable())
                 ->createGTCell(0, minStart, volume, 20,
                                dummyGates);
            }
            if (maxEnd < 86400000)
            {
             const_cast<GeneratingTable*>(controller->randomTable())
                 ->createGTCell(maxEnd, 86400000, volume, 20,
                                dummyGates);
            }
        }
    }
}

//======================================================================
void GenerateVehicleIO::_readGeneratingTableFile
(GeneratingTable* table, const string& type, const string fileName)
{
    fstream fin;
    fin.open(fileName.c_str(), ios::in);
    if(!fin.good())
    {
        cout << endl
             << "no vehicle generate table file [type=" << type
             << "]:" << endl
             << "  " << fileName << endl;
        return;
    }
    
    string str;
    while(fin.good())
    {
        getline(fin, str);
        AmuStringOperator::getAdjustString(&str);
        if (str.empty())
        {
            continue;
        }
        
        vector<string> tokens;
        AmuStringOperator::getTokens(&tokens, str, ',');
        if((type=="normal" && tokens.size() >= 7)
           || (type=="fixed" && tokens.size() >= 5)
           || (type=="grouped" && tokens.size() >= 9))
        {
            GeneratingTableCell* cell = new GeneratingTableCell();
            vector<string> gates;
            gates.clear();
            bool setValueResult = false;
            
            if (type=="normal")
            {
                /*
                 * [0]開始時刻,   [1]終了時刻,
                 * [2]出発地ID,   [3]目的地ID
                 * [4]交通量,     [5]車種ID
                 * [6]経由地の数, [7+]: 経由地ID*
                 */
                _getGates(&gates, &tokens, 2, 3, 6, 7);
                setValueResult =
                    cell->setValue(_str2time(tokens[0]),
                                   _str2time(tokens[1]),
                                   _str2volume(tokens[4]),
                                   _str2vehicleType(tokens[5]),
                                   gates);
            }
            else if (type=="fixed")
            {
                /*
                 * [0]開始時刻 (終了時刻は開始時刻と同一),
                 * [1]出発地ID,   [2]目的地ID,
                 * [3]車種ID (交通量は指定しない),
                 * [4]経由地の数, [5+]: 経由地ID*
                 */
                _getGates(&gates, &tokens, 1, 2, 4, 5);
                setValueResult =
                    cell->setValue(_str2time(tokens[0]),
                                   _str2time(tokens[0]),
                                   1,
                                   _str2vehicleType(tokens[3]),
                                   gates);
            }
            else if (type=="grouped")
            {
                /*
                 * [0]開始時刻,               [1]終了時刻,
                 * [2]出発地グループ化フラグ, [3]出発地ID,
                 * [4]目的地グループ化フラグ, [5]目的地ID,
                 * [6]交通量,                 [7]車種ID
                 * [8]経由地の数,             [9+]経由地ID
                 */
                _getGates(&gates, &tokens, 3, 5, 8, 9);
                setValueResult =
                    cell->setValue(_str2time(tokens[0]),
                                   _str2time(tokens[1]),
                                   _str2volume(tokens[6]),
                                   _str2vehicleType(tokens[7]),
                                   gates);
                if (tokens[2]=="P" || tokens[2]=="p")
                {
                    cell->setPaired(true);
                }
                else
                {
                    if (tokens[2]=="G" || tokens[2]=="g")
                    {
                        cell->setOriginGrouped(true);
                    }
                    if (tokens[4]=="G" || tokens[4]=="g")
                    {
                        cell->setDestinationGrouped(true);
                    }
                }
            }

            if (setValueResult)
            {
                table->addGTCell(cell);
            }
            else
            {
                cerr << "Unknown error occured in GeneratingTable"
                     << "[type=" << type << "]" << endl
                     << "line: " << endl
                     << "  " << str << endl;
                delete cell;
            }
        }
    }
    fin.close();

    // tableをソートする
    table->sortGTCells();

    return;
}

//======================================================================
ulint GenerateVehicleIO::_str2time(string str) const
{
    return AmuConverter::strtoul(str);
}

//======================================================================
double GenerateVehicleIO::_str2volume(string str) const
{
    return atof(str.c_str())
        * GVManager::getNumeric("TABLED_OD_FACTOR");
}

//======================================================================
int GenerateVehicleIO::_str2vehicleType(string str) const
{
    return atoi(str.c_str());
}

//======================================================================
void GenerateVehicleIO::_getGates(vector<string>* result_gates,
                                  vector<string>* tokens,
                                  unsigned int posOrigin,
                                  unsigned int posDestination,
                                  unsigned int posNumGates,
                                  unsigned int posMidGates) const
{
    assert(posOrigin < tokens->size()
           && posDestination < tokens->size()
           && posNumGates < tokens->size());
    result_gates->clear();

    // origin
    result_gates->push_back
        (AmuConverter::formatId
         ((*tokens)[posOrigin], NUM_FIGURE_FOR_INTERSECTION));

    // mid gates
    int numGates = atoi((*tokens)[posNumGates].c_str());
    if (numGates>0)
    {
        for (unsigned int i=posMidGates; i<(*tokens).size(); i++)
        {
            result_gates->push_back
                (AmuConverter::formatId
                 ((*tokens)[i], NUM_FIGURE_FOR_INTERSECTION));
        }
    }
        
    // destination
    /*
     * "******"の場合もあるのでformatIdでなくformatIdExtで
     */
    result_gates->push_back
        (AmuConverter::formatIdExt
         ((*tokens)[posDestination], NUM_FIGURE_FOR_INTERSECTION));
}

//======================================================================
void GenerateVehicleIO::readVehicleRoutingParams
(GenerateVehicleController* controller)
{
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     * 経路探索の効用関数に関するパラメータの設定
     * 0番目は距離に関するコスト（これが大きいと距離が短い経路が高効用）
     * 1番目は時間に関するコスト（時間が短い経路が高効用）
     * 2番目は交差点での直進に関するコスト（直進が少ない経路が高効用）
     * 3番目は交差点での左折に関するコスト（左折が少ない経路が高効用）
     * 4番目は交差点での右折に関するコスト（右折が少ない経路が高効用）
     * 5番目は道路の広さに関するコスト（道路幅が広い経路が高効用）
     *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     */
    vector<vector<double> >* vehicleRoutingParams
        = const_cast<vector<vector<double> >*>
                     (controller->vehicleRoutingParams());

    // デフォルト値1
    /*
     * ファイルが存在し，かつ指定がないあるいはすべて不正な場合
     * 配列が空になってしまうため1つだけ分岐の外で定義する．
     * 1つでも有効なパラメータ指定であればこれは上書きされる．
     */
    (*vehicleRoutingParams).resize(1);
    (*vehicleRoutingParams)[0].resize(VEHICLE_ROUTING_PARAMETER_SIZE);
    (*vehicleRoutingParams)[0][0] = 1;
    for (unsigned int i=1; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
    {
        (*vehicleRoutingParams)[0][i] = 0;
    }
    
    string fParamFile;
    GVManager::getVariable("VEHICLE_ROUTE_PARAM_FILE",
                           &fParamFile);

    ifstream inParamFile(fParamFile.c_str(), ios::in);
    if (inParamFile.good())
    {
        string str;
        int index = 0;
        while (inParamFile.good())
        {
            getline(inParamFile, str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                vector<string> tokens;
                AmuStringOperator::getTokens(&tokens, str, ',');
                if (tokens.size()==VEHICLE_ROUTING_PARAMETER_SIZE)
                {
                    // パラメータ指定が有効な行
                    (*vehicleRoutingParams).resize(index+1);
                    (*vehicleRoutingParams)[index]
                        .resize(VEHICLE_ROUTING_PARAMETER_SIZE);

                    for (unsigned int i=0; i<tokens.size(); i++)
                    {
                        (*vehicleRoutingParams)[index][i]
                            = AmuConverter::strtod(tokens[i]);
                    }
                    index++;
                }
            }
        }
        inParamFile.close();
    }
    else
    {
        if (GVManager::getFlag("FLAG_INPUT_VEHICLE"))
        {
            // 入力ファイルが存在しない場合
            cout << endl
                 << "no vehicle routing parameter file:" << endl
                 << "  " << fParamFile << endl;
        }

        (*vehicleRoutingParams).resize(3);
        for (unsigned int i=1; i<(*vehicleRoutingParams).size(); i++)
        {
            (*vehicleRoutingParams)[i]
                .resize(VEHICLE_ROUTING_PARAMETER_SIZE);
        }

        // デフォルト値2
        (*vehicleRoutingParams)[1][0] = 0;
        (*vehicleRoutingParams)[1][1] = 1;
        for (unsigned int i=2; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
        {
            (*vehicleRoutingParams)[1][i] = 0;
        }
        
        // デフォルト値3
        (*vehicleRoutingParams)[2][0] = 1;
        (*vehicleRoutingParams)[2][1] = 1;
        for (unsigned int i=2; i<VEHICLE_ROUTING_PARAMETER_SIZE; i++)
        {
            (*vehicleRoutingParams)[2][i] = 0;
        }
    }
}

//======================================================================
void GenerateVehicleIO::readVehicleRoutingPrefRank
(GenerateVehicleController* controller)
{
    map<unsigned int, double, greater<int> >* vehicleRoutingPrefRank
        = const_cast< map<unsigned int, double, greater<int> >*>
        (controller->vehicleRoutingPrefRank());
    
    string fPrefRankFile;
    GVManager::getVariable("VEHICLE_ROUTE_PREFRANK_FILE",
                           &fPrefRankFile);
    ifstream inPrefRankFile(fPrefRankFile.c_str(), ios::in);
    if (inPrefRankFile.good())
    {
        // 入力ファイルが存在する場合
        string str;
        vector<string> tokens;
        while (inPrefRankFile.good())
        {
            getline(inPrefRankFile, str);
            AmuStringOperator::getAdjustString(&str);
            if (str.empty())
            {
                continue;
            }
            AmuStringOperator::getTokens(&tokens, str, ',');
            if (tokens.size() != 2)
            {
                cerr << "invalid line - " << str << endl;
                continue;
            }
            int rank = atoi(tokens[0].c_str());
            double prob = atof(tokens[1].c_str());
            (*vehicleRoutingPrefRank)
                .insert(make_pair(rank, prob));
        }
        inPrefRankFile.close();
    }
    else
    {
        if (GVManager::getFlag("FLAG_INPUT_VEHICLE"))
        {
            // 入力ファイルが存在しない場合
            cout << endl
                 << "no vehicle routing preferred network rank file:"
                 << endl
                 << "  " << fPrefRankFile << endl;
        }
        unsigned int rank = _sim->routingManager()->maxRank();
        double prob   = 1.0;
        double except = 0.2;
        for (;rank>=2; rank--)
        {
            (*vehicleRoutingPrefRank)
                .insert(make_pair(rank, prob-except));
            prob   = except;
            except *= 0.2;
        }
        (*vehicleRoutingPrefRank)
            .insert(make_pair(rank, prob));
    }
}

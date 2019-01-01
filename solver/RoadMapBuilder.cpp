/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RoadMapBuilder.h"
#include "RoadMap.h"
#include "GVManager.h"
#include "ScheduleManager.h"
#include "AmuStringOperator.h"
#include "AmuConverter.h"
#include "LaneBundle.h"
#include "Intersection.h"
#include "ODNode.h"
#include "Section.h"
#include "Lane.h"
#include "SpeedLimitCell.h"
#include "RoadEntity.h"
#include "Signal.h"
#include "SignalIO.h"
#include "AmuPoint.h"
#include "Conf.h"
#include "Parallel.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#ifdef INCLUDE_TRAMS
#include "tram/TramLineManager.h"
#include "tram/TramLine.h"
#endif //INCLUDE_TRAMS

using namespace std;

//======================================================================
RoadMapBuilder::RoadMapBuilder()
{
    _currentRoadMap = 0;

    _isIntersectionCreated          = false;
    _isConnectionSet                = false;
    _isPositionSet                  = false;
    _isIntersectionStructInfoSet    = false;
    _isIntersectionStructureCreated = false;

    _isSectionCreated          = false;
    _isSectionStructInfoSet    = false;
    _isSectionStructureCreated = false;

    _isNetworkCreated        = false;
    _isSubnetworkCreated     = false;
    _isLaneConnectionCreated = false;
}

//======================================================================
RoadMapBuilder::~RoadMapBuilder()
{
    if (_currentRoadMap!=NULL)
    {
        delete _currentRoadMap;
    }
}

//======================================================================
void RoadMapBuilder::buildRoadMap()
{
    if (_currentRoadMap!=NULL)
    {
        delete _currentRoadMap;
    }
    _currentRoadMap = new RoadMap();

    // 交差点(Intersection)を作成する
    _isIntersectionCreated = buildIntersections();
    if (!_isIntersectionCreated)
    {
        cerr << "Error: create intersections failed." << endl;
    }

    // 単路(Section)を作成する
    _isSectionCreated = buildSections();
    if (!_isSectionCreated)
    {
        cerr << "Error: create sections failed." << endl;
    }

    // ネットワークを作成する
    _isNetworkCreated = buildNetwork();
    if (!_isNetworkCreated)
    {
        cerr << "Error: create network failed" << endl;
    }

#ifdef INCLUDE_TRAMS
    // 路面電車の路線を定義する
    /*
     * ネットワーク作成後，詳細構造設定前でなければならない
     */
    TramLineManager* tramLineManager
        = &(TramLineManager::instance());

    tramLineManager->setRoadMap(_currentRoadMap);
    tramLineManager->readTramLineFile();
    tramLineManager->print();
#endif //INCLUDE_TRAMS

    // 単路の構造情報を設定する、ファイルを読み込む、依存関係のため単路が先
    _isSectionStructInfoSet = setSectionStructInfo(true);
    if (!_isSectionStructInfoSet && _parallel->isMaster())
    {
        cerr << "Error: struct info of section not set." << endl;
    }

    // 交差点の構造情報を設定する、ファイルを読み込む
    bool readIntersectionInfoFromFile = true;
    if (GVManager::getFlag("DEBUG_FLAG_ALL_SECTION_SINGLE_LANE_EACH_SIDE"))
    {
        readIntersectionInfoFromFile = false;
    }
    _isIntersectionStructInfoSet
        = setIntersectionStructInfo(readIntersectionInfoFromFile);
    if (!_isIntersectionStructInfoSet && _parallel->isMaster())
    {
        cerr << "Error: struct info of intersection not set." << endl;
    }

    // 交差点の詳細構造を設定する
    _isIntersectionStructureCreated = createIntersectionStructure();
    if (!_isIntersectionStructureCreated && _parallel->isMaster())
    {
        cerr << "Error: internal structure of intersections not created.";
    }

    // 単路の内部構造を設定する
    _isSectionStructureCreated = createSectionStructure();
    if (!_isSectionStructureCreated && _parallel->isMaster())
    {
        cerr << "Error: internal structure of intersections not created."
             << endl;
    }

    // レーンの接続情報を設定する
    _isLaneConnectionCreated = buildLaneConnection();
    if (!_isLaneConnectionCreated && _parallel->isMaster())
    {
        cerr << "Error: create lane connection failed." << endl;
    }

    // レーンの交差判定
    // レーンの接続情報は全て設定されていなければならない
    _isLaneCollisionCreated = buildLaneCollision();
    if (!_isLaneCollisionCreated)
    {
        cerr << "Error: create lane collision failed." << endl;
    }

    // サブネットワークを作成する
    // 交差点と単路が作成済でなければならない
    _isSubnetworkCreated = buildSubnetwork();
    if (!_isSubnetworkCreated)
    {
        cerr << "Error: create subnetwork failed." << endl;
    }

    // parallelオブジェクトを設定する
    _currentRoadMap->setParallel(_parallel);

    // 交差点のprocessIdを設定する
    for(int i=0;i<_parallel->commSize();i++)
    {
      if(_parallel->myId()==i)
      {
        _currentRoadMap->setProcessId();
      }

      _parallel->barrier();
    }
}

//======================================================================
bool RoadMapBuilder::buildIntersections()
{
    // 設定ファイルの名前を取得する
    string fMapPosition, fNetwork;

    GVManager::getVariable("MAP_POSITION_FILE", &fMapPosition);
    GVManager::getVariable("MAP_NETWORK_FILE", &fNetwork);

    // 交差点の生成
    _isIntersectionCreated = createIntersection(fNetwork);
    if (!_isIntersectionCreated && _parallel->isMaster())
    {
        cerr << "Error: intersections not created." << endl;
        return false;
    }

    // 接続情報の設定
    _isConnectionSet = setConnection(fNetwork);
    if (!_isConnectionSet && _parallel->isMaster())
    {
        cerr << "Error: connection between intersections not set." << endl;
        return false;
    }

    // 座標の設定
    _isPositionSet = setPosition(fMapPosition);
    if (!_isPositionSet && _parallel->isMaster())
    {
        cerr << "Error: position of intersection not set." << endl;
        return false;
    }

    return true;
}

//======================================================================
bool RoadMapBuilder::createIntersection(const string& fNetwork)
{
    if (!_isIntersectionCreated)
    {
        assert(_currentRoadMap);

        // ファイルを読み込む
        ifstream inNetworkFile(fNetwork.c_str(), ios::in);
        if (!inNetworkFile && _parallel->isMaster())
        {
            cerr << "Error: cannot open file" << fNetwork << "." << endl;
            exit(EXIT_FAILURE);
        }

        // 交差点の生成
        string str;
        while (inNetworkFile.good())
        {
            getline(inNetworkFile, str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                vector<string> tokens;
                string id;
                string type;
                Intersection* ptInter;

                AmuStringOperator::getTokens(&tokens, str, ',');
                assert(tokens.size()>2);
                // 1番目のカラムは交差点の識別番号
                id = tokens[0];
                // 2番目のカラムは交差点のタイプ
                type = tokens[1];

                // デバッグ用フラグが立っていたら交差点タイプを上書き
                if (GVManager::getFlag
                    ("DEBUG_FLAG_ALL_SECTION_SINGLE_LANE_EACH_SIDE"))
                {
                    string dummy;
                    for (unsigned int i=0; i<type.length(); i++)
                    {
                        dummy += "1";
                    }
                    type = dummy;
                }

                if (type.length()==2)
                {
                    ptInter
                        = new ODNode
                        (AmuConverter::formatId
                         (id, NUM_FIGURE_FOR_INTERSECTION),
                         type,
                         _currentRoadMap);
                }
                else
                {
                    ptInter
                        = new Intersection
                        (AmuConverter::formatId
                         (id, NUM_FIGURE_FOR_INTERSECTION),
                         type,
                         _currentRoadMap);
                }
                _currentRoadMap->addIntersection(ptInter);
            }
        }
        inNetworkFile.close();
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::setConnection(const string& fNetwork)
{
    if (!_isConnectionSet)
    {
        assert(_currentRoadMap);
        assert(_isIntersectionCreated);

        // ファイルを読み込む
        ifstream inNetworkFile(fNetwork.c_str(), ios::in);
        if (!inNetworkFile && _parallel->isMaster())
        {
            cerr << "Error: cannot open file" << fNetwork << "." << endl;
            exit(EXIT_FAILURE);
        }

        // 交差点の接続情報をセット
        string str;
        while (inNetworkFile.good())
        {
            getline(inNetworkFile, str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                vector<string> tokens;
                string id;
                string type;

                AmuStringOperator::getTokens(&tokens, str, ',');
                assert(tokens.size()>2);
                id = tokens[0];
                type = tokens[1];

                Intersection* ptInter = _currentRoadMap
                    ->intersection
                    (AmuConverter::formatId
                     (id, NUM_FIGURE_FOR_INTERSECTION));

                // 隣接交差点のIDをセットする
                for (unsigned int i=2; i< tokens.size(); i++)
                {
                    ptInter->setNext
                        (_currentRoadMap->intersection
                         (AmuConverter::formatId
                          (tokens[i],NUM_FIGURE_FOR_INTERSECTION)));
                }
            }
        }
        inNetworkFile.close();
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::setPosition(const string& fMapPosition)
{
    if (!_isPositionSet)
    {
        assert(_currentRoadMap);
        assert(_isIntersectionCreated);

        //ファイルを読み込む
        ifstream inPositionFile(fMapPosition.c_str(), ios::in);
        if (!inPositionFile && _parallel->isMaster())
        {
            cerr << "Error: cannot open file " << fMapPosition << "."
                 << endl;
            exit(EXIT_FAILURE);
        }

        //交差点の座標をセット
        string str;
        while (inPositionFile.good())
        {
            getline(inPositionFile, str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                vector<string> tokens;
                string id;
                double x = 0, y = 0, z = 0;

                AmuStringOperator::getTokens(&tokens, str, ',');
                assert(tokens.size()>=3);
                id = tokens[0];
                x = static_cast<double>(atof(tokens[1].c_str()));
                y = static_cast<double>(atof(tokens[2].c_str()));
                //z座標はオプション
                if (tokens.size()==4)
                {
                    z = static_cast<double>(atof(tokens[3].c_str()));
                }
                else
                {
                    z = 0;
                }

                //値をセットする
                if (_currentRoadMap
                    ->intersection
                    (AmuConverter::formatId
                     (id, NUM_FIGURE_FOR_INTERSECTION)) != NULL)
                {
                    _currentRoadMap
                        ->intersection
                        (AmuConverter::formatId
                         (id, NUM_FIGURE_FOR_INTERSECTION))
                        ->addCenter(AmuPoint(x,y,z));
                }
            }
        }
        inPositionFile.close();
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::setIntersectionStructInfo(bool readFile)
{
    if (!_isIntersectionStructureCreated)
    {
        assert(_currentRoadMap);
        assert(_isIntersectionCreated);

        string fStruct;
        double sidewalkWidth, crosswalkWidth;
        GVManager::getVariable("INTERSECTION_STRUCT_FILE", &fStruct);

        // ファイルを読み込む
        ifstream inStructFile;
        if (readFile)
        {
            inStructFile.open(fStruct.c_str(), ios::in);
            if (!inStructFile && _parallel->isMaster())
            {
                cout << "no intersection struct file:" << endl
                     << "  " << fStruct << endl << endl;
            }
        }
        if (readFile && inStructFile)
        {
            // 交差点の構造情報を設定する
            string str;
            while (inStructFile.good())
            {
                getline(inStructFile, str);
                AmuStringOperator::getAdjustString(&str);
                if (str.empty())
                {
                    continue;
                }

                vector<string> tokens;
                string id;

                AmuStringOperator::getTokens(&tokens, str, ',');
                if (tokens.size() != 3)
                {
                    cerr << "Error: intersection struct"
                         << " invalid token number "
                         << tokens.size() << endl;
                }
                else
                {
                    // 1番目のカラムは交差点の識別番号
                    id = AmuConverter::formatId
                        (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
                    // 2番目のカラムは歩道幅
                    sidewalkWidth  = atof
                        (tokens[1].c_str());
                    // 3番目のカラムは横断歩道幅
                    crosswalkWidth = atof(tokens[2].c_str());
                    Intersection* intersection
                        = _currentRoadMap->intersection(id);
                    if (!intersection && _parallel->isMaster())
                    {
                        cerr << "Error: intersection struct"
                             << " invalid input -"
                             << " intersection " << id
                             << " does not exist." << endl;
                    }
                    else
                    {
                        if (!intersection->setStructInfo(sidewalkWidth,
                                                         crosswalkWidth))
                        {
                            cerr << "Error: intersection struct"
                                 << " invalid input of intersection "
                                 << id << endl;
                        }
                    }
                }
            }
            inStructFile.close();
        }

        // 単路の構造情報に既定値を設定する、設定済みなら内部で無視する
        // 車道頂点を先に作るために基礎構造を生成する
        CITRMAPI iti = _currentRoadMap->intersections()->begin();
        sidewalkWidth  = GVManager::getNumeric("DEFAULT_SIDEWALK_WIDTH");
        crosswalkWidth = GVManager::getNumeric("DEFAULT_CROSSWALK_WIDTH");
        while (iti != _currentRoadMap->intersections()->end())
        {
            if (!(*iti).second->setStructInfo(sidewalkWidth,
                                              crosswalkWidth))
            {
                cerr << "intersection " << (*iti).second->id()
                     << ": default struct info error." << endl;
            }
            if (!(*iti).second->createBaseStructure(readFile))
            {
                cerr << "intersection " << (*iti).second->id()
                     << ": create base structure error." << endl;
            }
            iti++;
        }
    }
    cout << endl;
    return true;
}

//======================================================================
bool RoadMapBuilder::createIntersectionStructure()
{
    if (!_isIntersectionStructureCreated)
    {
        assert(_currentRoadMap);
        assert(_isIntersectionCreated);
        assert(_isIntersectionStructInfoSet);
        assert(_isConnectionSet);

        bool check = false;

        CITRMAPI iti = _currentRoadMap->intersections()->begin();
        while (iti != _currentRoadMap->intersections()->end())
        {
            // 各Intersectionのcreate関数を呼び出す
            check = (*iti).second->create();
            if (!check)
            {
                cerr << "intersection " << (*iti).second->id()
                     << ": internal structure error." << endl;
                return false;
            }
            iti++;
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildSections()
{
    // 単路の生成
    _isSectionCreated = createSection();
    if (!_isSectionCreated && _parallel->isMaster())
    {
        cerr << "Error: Section not created." << endl;
        return false;
    }

    return true;
}

//======================================================================
bool RoadMapBuilder::createSection()
{
    if (!_isSectionCreated)
    {
        // 接続関係を持つ交差点間を結ぶ単路を作成する
        CITRMAPI iti = _currentRoadMap->intersections()->begin();
        while (iti != _currentRoadMap->intersections()->end())
        {
            for (int i=0;
                 i<static_cast<signed int>((*iti).second->numNext());
                 i++)
            {
                // IDを決定する
                Intersection* begin = NULL;
                Intersection* end   = NULL;
                string id = "";

                // 該当の交差点と接続先の交差点のIDを比べ、
                // 該当交差点のIDが小さい場合のみ単路を作成
                if ((*iti).second->id() < (*iti).second->next(i)->id())
                {
                    id = (*iti).second->id()
                        + (*iti).second->next(i)->id();
                    begin
                        = const_cast<Intersection*>
                        ((*iti).second);
                    end
                        = const_cast<Intersection*>
                        ((*iti).second->next(i));
                    assert(begin!=NULL && end!=NULL);

                    // 既にSectionオブジェクトが作られていないか
                    // チェックする
                    // 重複する単路を認めない
                    if (_currentRoadMap->section(id)==NULL)
                    {
                        Section* ptSection
                            = new Section(id, begin, end,
                                          _currentRoadMap);
                        _currentRoadMap->addSection(ptSection);
                    }
                    else
                    {
                        cerr << "intersection " << (*iti).second->id()
                             << " to intersection "
                             << (*iti).second->next(i)->id()
                             << " section error." << endl;
                        return false;
                    }
                }
            }
            iti++;
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::setSectionStructInfo(bool readFile)
{
    if (!_isSectionStructureCreated)
    {
        assert(_currentRoadMap);
        assert(_isSectionCreated);

        string fStruct;
        double laneWidth, roadsideWidth;
        double sidewalkWidthRight, sidewalkWidthLeft;
        GVManager::getVariable("SECTION_STRUCT_FILE", &fStruct);
        int autoSidewalkLane =
            (int)GVManager::getNumeric("AUTO_SIDEWALK_SECTION_LANE");

        // ファイルを読み込む
        ifstream inStructFile;
        if (readFile)
        {
            inStructFile.open(fStruct.c_str(), ios::in);
            if (!inStructFile && _parallel->isMaster())
            {
                cout <<"no section struct file:" << endl
                     << "  " << fStruct << endl << endl;;
            }
        }
        if (readFile && inStructFile)
        {
            // 単路の構造情報を設定する
            string str;
            while (inStructFile.good())
            {
                getline(inStructFile, str);
                AmuStringOperator::getAdjustString(&str);
                if (str.empty())
                {
                    continue;
                }

                vector<string> tokens;
                string beginInterId, endInterId, sectionId;
                int streetTrafficWalkerTypeNum;
                vector<int> streetTrafficWalkerType;

                AmuStringOperator::getTokens(&tokens, str, ',');
                if (tokens.size() < 7
                    || static_cast<signed int>(tokens.size())
                    != 7 + atoi(tokens[6].c_str()))
                {
                    cerr << "Error: section struct invalid token number "
                         << tokens.size() << endl;
                }
                else
                {
                    // 1/2番目のカラムは交差点の識別番号
                    beginInterId
                        = AmuConverter::formatId
                        (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
                    endInterId
                        = AmuConverter::formatId
                        (tokens[1], NUM_FIGURE_FOR_INTERSECTION);
                    if (atoi(beginInterId.c_str())
                        < atoi(endInterId.c_str()))
                    {
                        sectionId = beginInterId + endInterId;
                    }
                    else
                    {
                        sectionId = endInterId + beginInterId;
                    }
                    // 3番目のカラムはレーン幅
                    laneWidth          = atof(tokens[2].c_str());
                    // 4番目のカラムは路側幅
                    roadsideWidth      = atof(tokens[3].c_str());
                    // 5/6番目のカラムは横断歩道幅
                    sidewalkWidthRight = atof(tokens[4].c_str());
                    sidewalkWidthLeft  = atof(tokens[5].c_str());
                    // 7番目のカラムは車道通行歩行者種別数
                    streetTrafficWalkerTypeNum
                        = atoi(tokens[6].c_str());
                    // 8番目以降のカラムは車道通行歩行者種別
                    for (int i = 0; i < streetTrafficWalkerTypeNum; i++)
                    {
                        if (tokens[7 + i] == "*")
                        {
                            streetTrafficWalkerType.push_back
                                (TRAFFIC_WALKER_TYPE_ANY);
                        }
                        else
                        {
                            streetTrafficWalkerType.push_back
                                (atoi(tokens[7 + i].c_str()));
                        }
                    }
                    Section* section
                        = _currentRoadMap->section(sectionId);
                    if (!section && _parallel->isMaster())
                    {
                        cerr << "Error: section struct invalid input - "
                             << "section " << sectionId
                             << " does not exist." << endl;
                    }
                    else
                    {
                        if (!section->setStructInfo
                            (laneWidth, roadsideWidth,
                             sidewalkWidthRight, sidewalkWidthLeft,
                             streetTrafficWalkerType, false,
                             autoSidewalkLane))
                        {
                            cerr << "Error: section struct invalid input of section "
                                 << sectionId << endl;
                        }
                    }
                }

            }
            inStructFile.close();
        }

        // 単路の構造情報に既定値を設定する、設定済みなら内部で無視する
        laneWidth     = GVManager::getNumeric("DEFAULT_LANE_WIDTH");
        roadsideWidth = GVManager::getNumeric("DEFAULT_ROADSIDE_WIDTH");
        sidewalkWidthRight = sidewalkWidthLeft
            = GVManager::getNumeric("DEFAULT_SIDEWALK_WIDTH");
        vector<int> streetTrafficWalkerTypeNone;
        CITRMAPS its = _currentRoadMap->sections()->begin();
        while (its != _currentRoadMap->sections()->end())
        {
            if (!(*its).second->setStructInfo
                (laneWidth, roadsideWidth,
                 sidewalkWidthRight, sidewalkWidthLeft,
                 streetTrafficWalkerTypeNone, true,
                 autoSidewalkLane))
            {
                cerr << "section " << (*its).second->id()
                     << ": default struct info error." << endl;
            }
            its++;
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::createSectionStructure()
{
    if (!_isSectionStructureCreated)
    {
        assert(_isSectionCreated);
        assert(_isSectionStructInfoSet);

        bool check = false;

        CITRMAPS its = _currentRoadMap->sections()->begin();
        while (its != _currentRoadMap->sections()->end())
        {
            // 各Sectionのcreate関数を呼び出す
            check = (*its).second->create();
            if (!check)
            {
                cerr << "section " << (*its).second->id()
                     << ": internal structure error." << endl;
            }
            its++;
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildNetwork()
{
    assert(_isIntersectionCreated);
    assert(_isSectionCreated);

    // 具体的には交差点と単路のそれぞれに接続関係を伝える
    // Sectionのconnect()を呼び出す
    CITRMAPS its = _currentRoadMap->sections()->begin();
    while (its!=_currentRoadMap->sections()->end())
    {
        (*its).second->connect();
        its++;
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildSubnetwork()
{
    assert(_isIntersectionStructureCreated);
    assert(_isSectionStructureCreated);

    bool check = false;

    CITRMAPS its = _currentRoadMap->sections()->begin();
    while (its != _currentRoadMap->sections()->end())
    {
        check = (*its).second->createSubnetwork();
        if (!check)
        {
            cerr << "section " << (*its).second->id()
                 << ": create subnetwork error." << endl;
        }
        its++;
    }

    CITRMAPI iti = _currentRoadMap->intersections()->begin();
    while (iti != _currentRoadMap->intersections()->end())
    {
        check = (*iti).second->createSubnetwork();
        if (!check)
        {
            cerr << "intersection " << (*iti).second->id()
                 << ": create subnetwork error." << endl;
        }
        iti++;
    }

    return true;
}

//======================================================================
bool RoadMapBuilder::buildLaneConnection()
{
    assert(_isIntersectionStructureCreated);
    assert(_isSectionStructureCreated);

    const vector<LaneBundle*>* bundles = _currentRoadMap->laneBundles();
    for (unsigned int i=0; i<bundles->size(); i++)
    {
        const RMAPLAN* lanes = ((*bundles)[i])->lanes();
        for (CITRMAPLAN itl = lanes->begin();
             itl != lanes->end();
             itl++)
        {
            if (!((*itl).second->setConnection()))
            {
                cerr << "RoadMapBuilder::setLaneConnection(lane bundle:"
                     << (*bundles)[i]->id() << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildLaneCollision()
{
    assert(_isIntersectionStructureCreated);
    assert(_isSectionStructureCreated);

    const vector<LaneBundle*>* bundles = _currentRoadMap->laneBundles();
    for (unsigned int i=0; i<bundles->size(); i++)
    {
        const RMAPLAN* lanes = ((*bundles)[i])->lanes();
        for (CITRMAPLAN itl = lanes->begin();
             itl != lanes->end();
             itl++)
        {
            if (!((*itl).second->setCollision()))
            {
                cerr << "RoadMapBuilder::setLaneCollision(lane bundle:"
                     << (*bundles)[i]->id() << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildSignals()
{
    assert(_currentRoadMap);
    CITRMAPI iti = _currentRoadMap->intersections()->begin();

    SignalIO& io = SignalIO::instance();
    vector<SignalAspect> aspect;
    SignalControlDataSet dataset;

    while (iti!=_currentRoadMap->intersections()->end())
    {
        if ((*iti).second->numNext()!=1)
        {
            // 接続数1の交差点（ODノード）には信号を設置しない
            Signal* signal = new Signal();

            // 信号の設定ファイルを読み込む
            int sideNum = (*iti).second->numNext();
            aspect = io.aspect((*iti).first, sideNum);
            dataset = io.signalControlDataSet((*iti).first);

            // 信号に情報をセットする
            signal->setId((*iti).first);
            signal->setStateSet(aspect);
            signal->setSignalControlDataSet(dataset);

            // RoadMapと該当する交差点に登録する
            _currentRoadMap->addSignal(signal);
            (*iti).second->attachSignal(signal);

            // サブセクションに信号を登録する
            const RMAPENT* entities = (*iti).second->entities();
            CITRMAPENT ite;
            for (ite=entities->begin(); ite!=entities->end(); ite++)
            {
                if ((*ite).second->type()==SUBROAD_TYPE_CROSSWALK)
                {
                    int dir = (*iti).second->direction((*ite).second);
                    if (dir!=-1)
                    {
                        (*ite).second->attachSignal(signal, dir);
                    }
                }
            }
        }
        iti++;
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::buildSignalsAllBlue()
{
    assert(_currentRoadMap);
    CITRMAPI iti = _currentRoadMap->intersections()->begin();

    SignalIO& io = SignalIO::instance();
    vector<SignalAspect> aspect;
    SignalControlDataSet dataset;

    while (iti!=_currentRoadMap->intersections()->end())
    {
        if ((*iti).second->numNext()!=1)
        {
            // 接続数1の交差点（ODノード）には信号を設置しない
            Signal* signal = new Signal();

            // 信号現示パターンとスプリットを取得する
            int sideNum = (*iti).second->numNext();
            aspect = io.aspectBlue((*iti).first, sideNum);
            dataset = io.signalControlDataSetBlue((*iti).first);

            // 信号に情報をセットする
            signal->setId((*iti).first);
            signal->setStateSet(aspect);
            signal->setSignalControlDataSet(dataset);

            // RoadMapと該当する交差点に登録する
            _currentRoadMap->addSignal(signal);
            (*iti).second->attachSignal(signal);

            // サブセクションに信号を登録する
            const RMAPENT* entities = (*iti).second->entities();
            CITRMAPENT ite;
            for (ite=entities->begin(); ite!=entities->end(); ite++)
            {
                if ((*ite).second->type()==SUBROAD_TYPE_CROSSWALK)
                {
                    int dir = (*iti).second->direction((*ite).second);
                    if (dir!=-1)
                    {
                        (*ite).second->attachSignal(signal, dir);
                    }
                }
            }
        }
        iti++;
    }
    return true;
}

//======================================================================
bool RoadMapBuilder::setSpeedLimit()
{
    assert(_isSectionCreated);

    string fSpeedLimit;
    GVManager::getVariable("SPEED_LIMIT_FILE", &fSpeedLimit);

    ifstream ifs(fSpeedLimit.c_str(), ios::in);
    if (!ifs && _parallel->isMaster())
    {
        cout << "no speed limit file:" << endl
             << "  " << fSpeedLimit << endl << endl;
        return false;
    }

    string str;
    while (ifs.good())
    {
        getline(ifs, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            ulint beginTime;
            ulint endTime;
            string beginInterId;
            string endInterId;
            double limit;

            AmuStringOperator::getTokens(&tokens, str, ',');

            // 後方互換性の確保
            /*
             * 旧フォーマットでは第1カラム(開始時刻)および
             * 第2カラム(終了時刻)が不足するので補完する．
             */
            if (tokens.size()==3)
            {
                tokens.resize(5);
                for (int i=4; i>=2; i--)
                {
                    tokens[i] = tokens[i-2];
                }
                tokens[0] = "0";
                tokens[1] = "86400000";
            }

            assert(tokens.size()==5);

            beginTime = atoi(tokens[0].c_str());
            endTime   = atoi(tokens[1].c_str());

            beginInterId
                = AmuConverter::formatId
                (tokens[2], NUM_FIGURE_FOR_INTERSECTION);
            endInterId
                = AmuConverter::formatId
                (tokens[3], NUM_FIGURE_FOR_INTERSECTION);

            limit = atof(tokens[4].c_str());

            // 指定された単路が本当に存在するかチェックする
            string sectionId;
            bool isUpDirection;
            if (beginInterId < endInterId)
            {
                sectionId
                    = AmuConverter::formatId
                    (beginInterId, NUM_FIGURE_FOR_INTERSECTION)
                    + AmuConverter::formatId
                    (endInterId,   NUM_FIGURE_FOR_INTERSECTION);
                isUpDirection = true;
            }
            else
            {
                sectionId
                    = AmuConverter::formatId
                    (endInterId,   NUM_FIGURE_FOR_INTERSECTION)
                    + AmuConverter::formatId
                    (beginInterId, NUM_FIGURE_FOR_INTERSECTION);
                isUpDirection = false;
            }
            Section* section = _currentRoadMap->section(sectionId);
            if (!section && _parallel->isMaster())
            {
                cerr << "speed limit: invalid input - "
                     << "section " << sectionId << " does not exist."
                     << endl;
            }
            else
            {
                SpeedLimitCell* cell
                    = new SpeedLimitCell(beginTime, section,
                                         isUpDirection, limit);
                ScheduleManager::addSpeedLimitCell(beginTime, cell);

                // 終了時には元に戻す
                ulint nextBeginTime
                    = endTime + TimeManager::unit();
                double nextLimit
                    = GVManager::getNumeric("SPEED_LIMIT_SECTION");
                SpeedLimitCell* nextCell
                    = new SpeedLimitCell(nextBeginTime, section,
                                         isUpDirection, nextLimit);
                ScheduleManager::addSpeedLimitCell(nextBeginTime, nextCell);

                // 値をセットする
                /*
                  const RMAPLAN* lanes = section->lanes();
                  CITRMAPLAN itl;

                  for (itl=lanes->begin(); itl!=lanes->end(); itl++)
                  {
                  if (section->isUp((*itl).second)==isUpDirection)
                  {
                  (*itl).second->setSpeedLimit(limit);
                  }
                  }
                */
            }
        }
    }
    // ScheduleManager::print();
    ifs.close();
    return true;
}

//======================================================================
bool RoadMapBuilder::setTrafficControlSection()
{
    assert(_isSectionCreated);

    string fTrafficControl;
    GVManager::getVariable("TRAFFIC_CONTROL_SECTION_FILE",
                           &fTrafficControl);
    ifstream ifs(fTrafficControl.c_str(), ios::in);
    if (!ifs && _parallel->isMaster())
    {
        cout << "no section prohibition file:" << endl
             << "  " << fTrafficControl << endl << endl;
        return false;
    }

    string str;
    while (ifs.good())
    {
        getline(ifs, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            string beginInterId;
            string endInterId;
            bool isWhiteListMode = true;

            AmuStringOperator::getTokens(&tokens, str, ',');
            assert(tokens.size()>=4);

            // 1番目のカラムは始点交差点ID
            beginInterId
                = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            // 2番目のカラムは終点交差点ID
            endInterId
                = AmuConverter::formatId
                (tokens[1], NUM_FIGURE_FOR_INTERSECTION);
            /*
             * 3番目のカラムが+であればホワイトリスト，
             * -であればブラックリストに登録
             */
            if (tokens[2] == "+")
            {
                isWhiteListMode = true;
            }
            else if (tokens[2] == "-")
            {
                isWhiteListMode = false;
            }
            else
            {
                cerr
                    << "3rd column in file: "
                    << fTrafficControl
                    << " must be \"+\" of \"-\"."
                    << endl
                    << "this line is ignored: "
                    << str
                    << endl;
                continue;
            }

            // 指定された単路が本当に存在するかチェックする
            string sectionId;
            bool isUpDirection;
            if (beginInterId < endInterId)
            {
                sectionId = beginInterId + endInterId;
                isUpDirection = true;
            }
            else
            {
                sectionId = endInterId + beginInterId;
                isUpDirection = false;
            }
            Section* section = _currentRoadMap->section(sectionId);
            if (!section && _parallel->isMaster())
            {
                cerr << "section prohibition: invalid input - "
                     << "section " << sectionId << " does not exist."
                     << endl;
            }
            else
            {
                if (isWhiteListMode)
                {
                    // ホワイトリストに登録
                    for (unsigned int i=3; i<tokens.size(); i++)
                    {
                        section->addVehicleTypeToWhiteList(isUpDirection,
                                                           tokens[i]);
                    }
                }
                else
                {
                    // ブラックリストに登録
                    for (unsigned int i=3; i<tokens.size(); i++)
                    {
                        section->addVehicleTypeToBlackList(isUpDirection,
                                                           tokens[i]);
                    }
                }
            }
        }
    }
    ifs.close();
    return true;
}

//======================================================================
bool RoadMapBuilder::setRoutingProbability()
{
    assert(_isSectionCreated);

    string fRoutingProbability;
    GVManager::getVariable("ROUTING_PROBABILITY_FILE",
                           &fRoutingProbability);
    ifstream ifs(fRoutingProbability.c_str(), ios::in);
    if (!ifs && _parallel->isMaster())
    {
        cout << "no routing probability file:" << endl
             << "  " << fRoutingProbability << endl << endl;
        return false;
    }

    string str;
    while (ifs.good())
    {
        getline(ifs, str);
        AmuStringOperator::getAdjustString(&str);
        if (!str.empty())
        {
            vector<string> tokens;
            string beginInterId;
            string endInterId;
            double probability;
            VehicleType type;

            AmuStringOperator::getTokens(&tokens, str, ',');
            if (tokens.size()<4)
            {
                cout << fRoutingProbability << " is invalid." << endl;
                cout << str << endl;
                abort();
            }

            // 1番目のカラムは始点交差点ID
            beginInterId
                = AmuConverter::formatId
                (tokens[0], NUM_FIGURE_FOR_INTERSECTION);
            // 2番目のカラムは終点交差点ID
            endInterId
                = AmuConverter::formatId
                (tokens[1], NUM_FIGURE_FOR_INTERSECTION);

            // 指定された単路が本当に存在するかチェックする
            string sectionId;
            bool isUpDirection;
            if (beginInterId < endInterId)
            {
                sectionId = beginInterId + endInterId;
                isUpDirection = true;
            }
            else
            {
                sectionId = endInterId + beginInterId;
                isUpDirection = false;
            }
            Section* section = _currentRoadMap->section(sectionId);
            if (!section && _parallel->isMaster())
            {
                cerr << "ERROR/ routing probability: invalid input - "
                     << "section " << sectionId << " does not exist."
                     << endl;
            }
            else
            {
                // 3番目のカラムは選択確率
                probability = atof(tokens[2].c_str());
                // 4番目以降のカラムは車種ID
                for (unsigned int i=3; i<tokens.size(); i++)
                {
                    type = atoi(tokens[i].c_str());
                    section->addRoutingProbability(isUpDirection,
                                                   type, probability);
                }
            }
        }
    }
    return true;
}

//======================================================================
RoadMap* RoadMapBuilder::roadMap()
{
    RoadMap* tmp = _currentRoadMap;
    _currentRoadMap = 0;
    return tmp;
}

//======================================================================
void RoadMapBuilder::setParallel(Parallel* parallel)
{
  _parallel = parallel;
}

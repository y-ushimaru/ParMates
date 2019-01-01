/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <cassert>
#include "GVManager.h"
#include "SignalIO.h"
#include "Conf.h"
#include "RoadMap.h"
#include "Intersection.h"
#include "Section.h"
#include "AmuConverter.h"
#include "AmuStringOperator.h"
#include <cerrno>
#ifdef USE_MINGW
#include <io.h>
#else // USE_MINGW
#include <sys/types.h>
#include <sys/stat.h>
#endif // USE_MINGW
#include <map>
#include <string>
#include <cstdlib>
#include <sstream>

#define NUM_FIGURE_FOR_TIMELINE_FILENAME 10

#ifdef USE_ZLIB
#include <zlib.h>
#define fzopen   gzopen
#define fzclose  gzclose 
#define fzprintf gzprintf
#define fz_ok    Z_OK
#else // USE_ZLIB
#define fzopen   fopen
#define fzclose  fclose
#define fzprintf fprintf
#define fz_ok    0
#endif // USE_ZLIB

using namespace std;

const string SignalIO::_timePrefix = "#Time=";

//======================================================================
SignalIO& SignalIO::instance()
{
    static SignalIO instance;
    return instance;
}

//======================================================================
vector<SignalAspect> SignalIO::aspect(const string& id,
				      const int sideNum) const
{
    vector<SignalAspect> stateSet;

    // 各面にメイン，サブ，歩行者の合計3つの信号表示があるので
    // sideNum*3が.msaファイルの列数となる。
    int colNum = sideNum*3;
    /*
     * colNum ==  6: 二分岐交差点
     * colNum ==  9: 三叉路
     * colNum == 12: 十字路
     * colNum == 15: 五叉路
     * colNum == 18: 六叉路
     */

    assert(colNum%3==0 && colNum/3>=2 && colNum/3<=10);
    string defaultPath, aspectExtension;
    string defaultAspectPrefix;

    bool result = true;
  
    result
        = (GVManager::getVariable("SIGNAL_CONTROL_DIRECTORY", &defaultPath));
    assert(result);
    result
        = (GVManager::getVariable("ASPECT_FILE_EXTENSION", &aspectExtension));
    assert(result);

    string path = defaultPath + id + aspectExtension;
    string type = "";
    int aspectNum;

    ifstream aspectIn;
    aspectIn.open(path.c_str(), ios::in);
    if (aspectIn)
    {
        // idで指定された交差点に専用の現示ファイルが見つかった場合
        aspectIn >> aspectNum;
    } 
    else
    {
        // idで指定された交差点に専用の現示ファイルが見つからなかった場合
        // デフォルトのテーブルを開く
        result = (GVManager::getVariable("SIGNAL_ASPECT_FILE_DEFAULT_PREFIX",
                                  &defaultAspectPrefix));
        assert(result);
        ostringstream oss;
        oss << defaultPath << defaultAspectPrefix
            << colNum/3 << aspectExtension;
        defaultPath = oss.str();

        aspectIn.clear();
        aspectIn.open(defaultPath.c_str(), ios::in);
        if(aspectIn.good())
        {
            aspectIn >> aspectNum;
        }
        else
        {
            cerr << "Default aspect info file not found." << endl;
            cerr << "file name: " << defaultPath << endl;
            assert(0);
        }
    }

    vector<int> buffers;
    buffers.resize(colNum);
    vector<Triplet> aState;
    for (int j=0; j < aspectNum; j++)
    {
        for (int i=0; i < colNum && aspectIn.good(); i++)
        {
            aspectIn >> buffers[i];
        }
        for (int k=0; k < colNum; k+=3)
        {
            Triplet aTriplet;
            aTriplet.main   = buffers[k];
            aTriplet.sub    = buffers[k+1];
            aTriplet.walker = buffers[k+2]; 
            aState.push_back(aTriplet);
        }
        SignalAspect aAspect(aState);
        stateSet.push_back(aAspect);
        aState.clear();
    }
    aspectIn.close();
    aState.clear();

    // ダミーデータの挿入(99番と言うColorはない)
    for(int k=0; k < colNum; k+=3)
    {
        Triplet aTriplet;
        aTriplet.main   = 99;
        aTriplet.sub    = 99;
        aTriplet.walker = 99; 
        aState.push_back(aTriplet);
    }
    SignalAspect aAspectDummy(aState);
    // あとの処理の関係で、Signal::_stateSetには
    // 必ずNUM_MAX_SPLIT_INT個の要素が必要。
    // この関数で用意するstateSetは
    // のちにSignal::_stateSetを設定するときに使われる
    // （内容がコピーされる）ので
    // aspectNumがNUM_MAX_SPLIT_INT個に満たない場合は
    // 上で作ったダミーのデータを足しておく。
    for(int i=aspectNum; i < NUM_MAX_SPLIT_INT; i++)
    {
        stateSet.push_back(aAspectDummy);
    }
    return stateSet;
}

//======================================================================
vector<SignalAspect> SignalIO::aspectBlue(const string& id,
					  const int sideNum) const
{
    int colNum = sideNum*3;

    vector<SignalAspect> stateSet;
    vector<Triplet> aState;
    // 青データ
    for (int k=0; k < colNum; k+=3)
    {
        Triplet aTriplet;
        aTriplet.main   = SignalColor::blue();
        aTriplet.sub    = SignalColor::none();
        aTriplet.walker = SignalColor::blue(); 
        aState.push_back(aTriplet);
    }
    SignalAspect aAspect(aState);
    stateSet.push_back(aAspect);
    aState.clear();
    // ダミーデータ
    for(int k=0; k<colNum; k+=3)
    {
        Triplet aTriplet;
        aTriplet.main   = 99;
        aTriplet.sub    = 99;
        aTriplet.walker = 99; 
        aState.push_back(aTriplet);
    }
    SignalAspect aAspectDummy(aState);
    for (int i=1; i<NUM_MAX_SPLIT_INT; i++)
    {
        stateSet.push_back(aAspectDummy);
    }

    return stateSet;
}

//======================================================================
SignalControlDataSet SignalIO::signalControlDataSet(const string& id) const
{
    vector<SignalControlData> dataSet;
    string extension, defaultPath, defaultFilename;

    bool result;
    result
        = (GVManager::getVariable("SIGNAL_CONTROL_DIRECTORY", &defaultPath));
    assert(result);
    result
        = (GVManager::getVariable("CONTROL_FILE_EXTENSION", &extension));
    assert(result);
    result
        = (GVManager::getVariable("SIGNAL_CONTROL_FILE_DEFAULT",
                           &defaultFilename));
    assert(result);
    string path = defaultPath + id + extension;

    ifstream controlDataIn(path.c_str(), ios::in);
    if (!controlDataIn)
    {
        defaultPath = defaultPath + defaultFilename + extension;
        controlDataIn.clear();
        controlDataIn.open(defaultPath.c_str(), ios::in);
        if (!controlDataIn)
        {
            cerr << "Cannot find default signal controll file." << endl;
            cerr << "filename : " << defaultPath << endl;
            assert(0);
        }
    }
    ulint begin=0, end=0, cycle=0;
    vector<ulint>  split;
    string str;

    while (controlDataIn.good())
    {
        getline(controlDataIn, str);
        if (!str.empty())
        {
            vector<string> tokens;
            int splitNum = 0;
            AmuStringOperator::getTokens(&tokens, str, " ");

            begin = (ulint)atol(tokens[0].c_str());
            end   = (ulint)atol(tokens[1].c_str());
            cycle = (ulint)atol(tokens[2].c_str());
       
            for (int i=3; i<static_cast<int>(tokens.size()); i++)
            {
                split.push_back(atol(tokens[i].c_str()));
                splitNum++;
            }

            //ダミーデータの挿入
            for (int i=splitNum; i<NUM_MAX_SPLIT; i++)
            {
                split.push_back(0);
            }

            SignalControlData controlData(begin, end, cycle, split);
            dataSet.push_back(controlData);
            split.clear();
            begin = end = cycle = 0;
        }
    }
    if (controlDataIn.eof())
    {
        controlDataIn.clear();
    }

    controlDataIn.close();
    SignalControlDataSet controlDataSet(dataSet);
    return controlDataSet;
}

//======================================================================
SignalControlDataSet SignalIO::signalControlDataSetBlue
(const string& id) const
{
    vector<SignalControlData> dataSet;
  
    ulint begin = 0;
    ulint end   = 86400000;
    ulint cycle = 120000;
    vector<ulint> split;
    split.push_back(120000);
    for (int i=1; i<NUM_MAX_SPLIT; i++)
    {
        split.push_back(0);
    }
    SignalControlData controlData(begin, end, cycle, split);
    dataSet.push_back(controlData);

    SignalControlDataSet controlDataSet(dataSet);
    return controlDataSet;
}

//======================================================================
bool SignalIO::writeSignalsDynamicData
(const ulint time, const RMAPSI* signals)
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 詳細データを出力する場合
    if (GVManager::getFlag("FLAG_OUTPUT_TIMELINE_D"))
    {
        //--------------------------------------------------------------
        // ファイル名を決定する
        vector<string> paths;
        string resultDir;

        GVManager::getVariable("RESULT_TIMELINE_DIRECTORY", &resultDir);
        paths.push_back(resultDir+"signal/");

        string strTime
            = AmuConverter::itos(time, NUM_FIGURE_FOR_TIMELINE_FILENAME);
        paths.push_back(strTime.substr(0,3)+"/");
        paths.push_back(strTime.substr(3,3)+"/");
#ifdef USE_ZLIB
        paths.push_back(strTime.substr(6,4)+".txt.gz");
#else // USE_ZLIB
        paths.push_back(strTime.substr(6,4)+".txt");
#endif // USE_ZLIB
        string dynamicPath = "";
        for (unsigned int i=0; i<paths.size(); i++)
        {
            dynamicPath += paths[i];
        }

        //--------------------------------------------------------------
        // ファイルを開く
#ifdef USE_ZLIB
        _dynamicOut = fzopen(dynamicPath.c_str(), "wb6f");
#else // USE_ZLIB
        _dynamicOut = fzopen(dynamicPath.c_str(), "w");
#endif // USE_ZLIB
        if (!_dynamicOut)
        {
            // ディレクトリが準備されていない場合には作成を試みる
            paths.pop_back();
            if(_makeDirectories(paths))
            {
#ifdef USE_ZLIB
                _dynamicOut = fzopen(dynamicPath.c_str(), "wb6f");
#else // USE_ZLIB
                _dynamicOut = fzopen(dynamicPath.c_str(), "w");
#endif // USE_ZLIB
            }
        }
        // ディレクトリを作ろうとしてもファイルが開けない場合には
        // 諦めて落ちる。
        if (!_dynamicOut)
        {
            cerr << "cannot open file:" << dynamicPath << endl;
            assert(0);
            exit(EXIT_FAILURE);
        }
       
        //--------------------------------------------------------------
        // ファイルの出力
        if (GVManager::getNumeric("OUTPUT_COMMENT_IN_FILE")!=0)
        {
            fzprintf(_dynamicOut, "%s\n", _timePrefix.c_str());
        }
        CITRMAPSI its;
        for (its=signals->begin(); its!=signals->end(); its++)
        {
            // 信号現示の出力
            _writeSignalDynamicData( (*its).second );
        }

        //--------------------------------------------------------------
        // ファイルを閉じる
        int fzcl = fzclose(_dynamicOut);
        if (fzcl != fz_ok)
        {
            cerr << "cannot close file:" << dynamicPath << endl;
            exit(EXIT_FAILURE);
        }
    }
    /*
    else
    {
    
    }
    */
    return true;
}

//======================================================================
bool SignalIO::_writeSignalDynamicData(Signal* signal)
{
    assert(_dynamicOut);

    for (int i=0;
         i <= _roadMap->intersection(signal->id())->numNext()-1;
         i++) 
    {
        // signalクラスのid()とは違う。出力用の信号機IDである。
        // dst(ID) src(ID) の順番
        // 例えば交差点2(src) ->  交差点1(dst) へ向かう
        // 自動車への信号機IDならば、
        // 000001000002となる。
        // 反対に交差点1(src) ->  交差点2(dst) へ向かう
        // 自動車への信号機IDならば、
        // 000002000001となる。
        string signalID = signal->id()
            + _roadMap->intersection(signal->id())->next(i)->id();

        // 方路iの信号機の現示を取得する
        SignalMainState mainState = signal->mainColor(i);
        SignalSubState subState = signal->subColor(i);

        int signalColor = 0;

        // 現示によってsignalColorの値を決定する
        // 2進数->10進数
        // Tram及び歩行者用信号は全く出力せず．
        if (mainState == SignalColor::blue()
            || mainState == SignalColor::redblink()
            || mainState == SignalColor::yellowblink())
            // 点滅(blink)は青とする．
        {
            signalColor += 32;
        }
        if (mainState == SignalColor::yellow())
        {
            signalColor += 16;
        }
        if (mainState == SignalColor::red())
        {
            signalColor += 8; 
        }
        if (subState == SignalColor::all()
            || subState == SignalColor::left()
            || subState == SignalColor::straightLeft()
            || subState == SignalColor::leftRight())
        {
            signalColor += 4;
        }
        if (subState == SignalColor::all()
            || subState == SignalColor::straight()
            || subState == SignalColor::straightLeft()
            || subState == SignalColor::straightRight())
        {
            signalColor += 2;
        }
        if (subState == SignalColor::all()
            || subState == SignalColor::right()
            || subState == SignalColor::straightRight()
            || subState == SignalColor::leftRight())
        {
            signalColor += 1;
        }

        stringstream ss;
        ss.str("");
        ss << signalID << "," //信号機ID
           << signalColor;    // 信号現示(10進数)
        fzprintf(_dynamicOut, "%s\n", ss.str().c_str());
    }
    
    return true;
}

//======================================================================
bool SignalIO::_makeDirectories(vector<string> paths) const
{
    bool result = false;

    string tmpPath = "";
    for (unsigned int i=0; i<paths.size(); i++)
    {
        tmpPath += paths[i];
    }

#ifndef USE_MINGW
    // パーミッション設定
    mode_t mode = S_IRUSR | S_IRGRP | S_IXUSR
        | S_IXGRP | S_IWUSR | S_IWGRP;
#endif //USE_MINGW

    // ディレクトリ作成
#ifdef USE_MINGW
    if (/*_*/mkdir(tmpPath.c_str())!=0)
#else
    if (mkdir(tmpPath.c_str(), mode)!=0)
#endif
    {
        /*
         * mkdirは成功したときに0，失敗したときに-1を返し，
         * 失敗したときはエラーコードが変数errno(cerrnoで宣言)に
         * 格納される．
         *
         * man 2 mkdir を参照．
         */
        if (errno==ENOENT && paths.size()>=2)
        {
            // tmpPathの中のどれかのディレクトリが存在しない場合には
            // 成功するまで(可能な限り)上の階層にもどる
            paths.pop_back();
#ifdef USE_MINGW
            if(_makeDirectories(paths)
               && /*_*/mkdir(tmpPath.c_str())==0)
#else
            if(_makeDirectories(paths)
               && mkdir(tmpPath.c_str(), mode)==0)
#endif
            {
                result = true;
            }
        }
        else
        {
            cerr << "cannot make directory: " << tmpPath << endl;
            assert(0);
        }
    }
    else
    {
        result = true;
    }
    return result;
}
//======================================================================
void SignalIO::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

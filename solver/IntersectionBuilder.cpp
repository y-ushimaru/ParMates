/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "LaneBundle.h"
#include "Intersection.h"
#include "ODNode.h"
#include "Conf.h"
#include "AmuVector.h"
#include "AmuConverter.h"
#include "AmuPoint.h"
#include "Lane.h"
#include "LaneInIntersection.h"
#include "RoadEntity.h"
#include "AmuStringOperator.h"
#include "SubIntersection.h"
#include "Section.h"
#include "ObjManager.h"
#include "GVManager.h"
#include "RelativeDirection.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <vector>
#include <map>
#include <fstream>
#ifdef INCLUDE_TRAMS
#include "tram/TramLineManager.h"
#include "tram/TramLine.h"
#include "tram/IntersectionTramExt.h"
#include "tram/BorderTram.h"
#endif //INCLUDE_TRAMS
#ifdef INCLUDE_PEDESTRIANS
#include "pedestrian/Zebra.h"
#endif //INCLUDE_PEDESTRIANS

using namespace std;

//#define IB_DEBUG

//======================================================================
bool Intersection::setStructInfo(double sidewalkWidth,
                                 double crosswalkWidth)
{
    // 設定済みなら無視する
    if (_hasStructInfo)
    {
        return true;
    }

    if (sidewalkWidth <= 0 || crosswalkWidth <= 0)
    {
        return false;
    }

    _sidewalkWidth  = sidewalkWidth;
    _crosswalkWidth = crosswalkWidth;

    _hasStructInfo = true;
    return true;
}

//======================================================================
bool Intersection::createBaseStructure(bool readFile)
{
    // 基礎構造を生成する、相対方向テーブル、車道頂点を作る
    // 交差点ファイルを読み込み、レーン情報は保管しておく
    // 全交差点の車道頂点がないと横断歩道を作れないので先に作る
    // あわせて交差点ファイルの読み込みをまとめた

    // 相対方向テーブルとレーン情報はファイルチェックだけして後で読み直す
    // ファイル内は相対方向テーブル、レーン情報、頂点の順番に書く
    // これらは入れ替えられない
    bool isRDTable = false;
    bool isStreetVertex = false;
    bool isLane = false;

    ifstream isfile;
    if (readFile)
    {
        string defaultPath;
        GVManager::getVariable("INTERSECTION_ATTRIBUTE_DIRECTORY",&defaultPath);
        string path = defaultPath + _id + ".txt";
        isfile.open(path.c_str());

        // ODNode ならファイルを無視する
        if (isfile && dynamic_cast<ODNode*>(this) != NULL)
        {
            // cout << "Ignore ODNode " << _id
            //      << " attribute file." << endl;
            readFile = false;
        }
    }
    if (readFile && isfile)
    {
        //属性指定ファイルを利用
        // cout << "Intersection ID:" << _id
        //      << " has attribute file." << endl;

        //ファイルからの指定方法の場合分け
        string str;
        vector<string> tokens;
        while (isfile.good())
        {
            getline(isfile,str);
            AmuStringOperator::getAdjustString(&str);
            if (!str.empty())
            {
                isRDTable = true;
                break;
            }
            else if (isfile.eof())
            {
                break;
            }
        }
        if (isRDTable)
        {
            AmuStringOperator::getTokens(&tokens,str,',');
            if (tokens[0]=="vertex"
                || tokens[0]=="Vertex"
                || tokens[0]=="VERTEX")
            {
                isRDTable = false;
                isStreetVertex = true;
            }
            else if (tokens[0]!="s"
                     && tokens[0]!="t"
                     && tokens[0]!="l"
                     && tokens[0]!="r")
            {
                isRDTable = false;
                isLane = true;
            }
        }
        if (isRDTable)
        {
            for (unsigned int i=1; i<_next.size(); i++)
            {
                getline(isfile,str);
                string::size_type t;
                t=str.find("#");
                if (t!=string::npos
                    || str.empty())
                {
                    --i;
                }
                AmuStringOperator::getAdjustString(&str);
            }
            str = "";
            while (str.empty())
            {
                getline(isfile,str);
                AmuStringOperator::getAdjustString(&str);
                if (!str.empty())
                {
                    AmuStringOperator::getTokens(&tokens,str,',');
                    if (tokens[0]=="vertex"
                        || tokens[0]=="Vertex"
                        || tokens[0]=="VERTEX")
                    {
                        isStreetVertex = true;
                        break;
                    }
                    else
                    {
                        isLane = true;
                        break;
                    }
                }
                if (isfile.eof())
                {
                    break;
                }
            }
        }
        if (isLane == true)
        {
            while (isfile.good())
            {
                getline(isfile,str);
                AmuStringOperator::getAdjustString(&str);
                AmuStringOperator::getTokens(&tokens,str,',');
                if (tokens[0]=="vertex"
                    || tokens[0]=="Vertex"
                    || tokens[0]=="VERTEX")
                {
                    isStreetVertex = true;
                    break;
                }
            }
        }
        // ファイルから車道頂点読み込み
        if(isStreetVertex == true)
        {
            isStreetVertex = createStreetVerticesFromFile(&isfile);
        }
        isfile.clear();
        isfile.seekg(0, ios::beg);
    }

    // 相対方向テーブルの生成
    if (isRDTable)
    {
        isRDTable = createRDTableFromFile(&isfile);
    }
    else
    {
        isRDTable = createDefaultRDTable();
    }
    if (!isRDTable)
    {
        cerr << "intersection:" << _id
             << ": relative direction table has not been successfully created." << endl;
        return false;
    }

#ifdef INCLUDE_TRAMS
    // 路面電車軌道を持つ交差点の設定
    /*
     * 車道頂点よりも前に設定
     */
    _tramExt->setNumTramConnectors();
#endif //INCLUDE_TRAMS

    // 車道頂点既定値の生成、相対方向テーブルを使っているのでここで処理する
    if (!isStreetVertex)
    {
        isStreetVertex = createDefaultStreetVertices();
    }
    if (!isStreetVertex)
    {
        cerr << "intersection:" << _id
             << ": street vertices has not been successfully created."
             << endl;
        return false;
    }

    // レーン情報を読み込んで保存、レーン生成は後で行う
    if (isLane)
    {
        isLane = readLanesFromFile(&isfile);
    }

    isfile.close();

    return true;
}

//======================================================================
void Intersection::setIncidentSection(int direction, Section* section)
{
    assert(0<=direction
           && direction<static_cast<signed int>(_incSections.size()));
    assert(!_incSections[direction]);
    _incSections[direction] = section;
}

//======================================================================
bool Intersection::create()
{
    // _passTimeのリサイズと初期化
    _passTime.resize(_next.size()*_next.size());
    _passTimeTotal.resize(_next.size()*_next.size());
    _numPassedVehicleTotal.resize(_next.size()*_next.size());
    _previousPassTimeTotal.resize(_next.size()*_next.size());
    _previousNumPassedVehicleTotal.resize(_next.size()*_next.size());
    for (unsigned int i=0; i<_next.size()*_next.size(); i++)
    {
        _passTimeTotal[i] = 0.0;
        _numPassedVehicleTotal[i] = 0;
        _previousPassTimeTotal[i] = 0.0;
        _previousNumPassedVehicleTotal[i] = 0;
    }

    // 頂点配列の生成、車道頂点をもとに作る
    bool isVertexCreated = createVertices();
    if (!isVertexCreated)
    {
        cerr << "intersection:" << _id
             << ": vertices have not been successfully created." << endl;
        return false;
    }

    // 境界の生成
    bool isBorderCreated = createDefaultBorders();
    if (!isBorderCreated)
    {
        cerr << "intersection:" << _id
             << ": borders have not been successfully created." << endl;
        return false;
    }

    // サブセクションの生成
    bool isSubsectionCreated = createDefaultSubsections();
    if (!isSubsectionCreated)
    {
        cerr << "intersection:" << _id
             << ": subsections have not been successfully created." << endl;
        return false;
    }

    // コネクタの生成
    bool isConnectorCreated = createDefaultConnectors();
    if (!isConnectorCreated)
    {
        cerr << "intersection:" << _id
             << ": connectors have not been successfully created." << endl;
        return false;
    }

    //レーンの生成
    bool isLaneCreated;
    if(!_laneBegEnd.empty())
    {
        isLaneCreated = createLanesFromFile();
    }
    else
    {
        isLaneCreated = createDefaultLanes();
#ifdef IB_DEBUG
        cout << "Intersection ID:" << _id
             << ", default Lanes were created." <<endl;
#endif
    }
    if (!isLaneCreated)
    {
        cerr << "intersection:" << _id
             << ": lanes have not been successfully created." << endl;
        return false;
    }

    return (isVertexCreated
            && isBorderCreated
            && isSubsectionCreated
            && isConnectorCreated
            && isLaneCreated);
}

//======================================================================
// Intersection, ODNodeで(今のところ)共通
bool Intersection::createDefaultRDTable()
{
    RelativeDirectionTable* tmpRDTable;

    if (_next.size() == 1)
    {
        tmpRDTable = new RelativeDirectionTableTerminal();
    }
    else if (_next.size() == 2)
    {
        tmpRDTable = new RelativeDirectionTable2Way();
    }
    else
    {
        // 単路数によらず角度のみで直進、左右チェック
        // 両方向が直進の場合のみ直進判定
        RelativeDirectionTableCustom* tmpRDTableCustom;
        tmpRDTableCustom = new RelativeDirectionTableCustom(_next.size());

        AmuVector dv[100];
        AmuVector rdv[100];
        int straight[100];
        assert(_next.size() <= 100);
        for (unsigned int i=0; i<_next.size(); i++)
        {
            dv[i].setPoints(_center, _next[i]->center());
            dv[i].setZ(0);
            dv[i].normalize();
            rdv[i].setPoints(_next[i]->center(), _center);
            rdv[i].setZ(0);
            rdv[i].normalize();
            straight[i] = -1;
        }
        for (unsigned int i=0; i<_next.size(); i++)
        {
            double thetaMin = 10000;
            for (unsigned int j=0; j<_next.size(); j++)
            {
                double theta = dv[i].calcAngle(rdv[j]);
                if (i == j)
                {
                    tmpRDTableCustom->setItem(i,j,RD_BACK);
                }
                else if (theta > 0)
                {
                    tmpRDTableCustom->setItem(i,j,RD_RIGHT);
                }
                else
                {
                    tmpRDTableCustom->setItem(i,j,RD_LEFT);
                }

                if (i != j && thetaMin > abs(theta))
                {
                    thetaMin = abs(theta);
                    straight[i] = j;
                }
            }
        }
        for (unsigned int i=0; i<_next.size(); i++)
        {
            int j = straight[i];
            if (j != -1 && static_cast<signed int>(i) == straight[j])
            {
                tmpRDTableCustom->setItem(i,j,RD_STRAIGHT);
                tmpRDTableCustom->setItem(j,i,RD_STRAIGHT);
            }
        }

        tmpRDTable = tmpRDTableCustom;
    }

    _rdTable = tmpRDTable;
    return true;
}

//======================================================================
bool Intersection::createRDTableFromFile(ifstream* isf)
{
    string str;
    vector<vector<string> > vecRD;

    for (unsigned int i=0; i<_next.size(); i++)
    {
        getline((*isf),str);

        string::size_type t;
        t=str.find("#");
        if (t!=string::npos || str.empty())
        {
            --i;
        }
        AmuStringOperator::getAdjustString(&str);

        if (!str.empty())
        {
            vector<string> tokens;
            vector<string> tmpvec;

            AmuStringOperator::getTokens(&tokens,str,',');
            if (_next.size()!=tokens.size())
            {
                cerr << "File:" << _id << ".txt , " << i+1
                     << "th border; You must put " << _next.size()
                     << " values for relative directions." << endl;
                exit(1);
            }
            for (unsigned int j=0; j<tokens.size(); j++)
            {
                assert( tokens[j]=="s" || tokens[j]=="t"
                        || tokens[j]=="l" || tokens[j]=="r");
                if (i==j)
                {
                    assert(tokens[j]=="t");
                }
                tmpvec.push_back(tokens[j]);
            }
            vecRD.push_back(tmpvec);
        }
    }

#ifdef IB_DEBUG
    cout << "Intersection ID:" << _id
         << ", unique RD table below." << endl;

    for(int i=0; i<_next.size(); i++)
    {
        for(int j=0;j<_next.size(); j++)
        {
            cout << vecRD[i][j] << "  ";
        }
        cout << endl;
    }
#endif

    RelativeDirectionTableCustom* tmpRDTable;

    tmpRDTable = new RelativeDirectionTableCustom(_next.size());
    for(unsigned int i=0; i<_next.size(); i++)
    {
        for(unsigned int j=0; j<_next.size(); j++)
        {
            tmpRDTable->setItem(i,j,atoRD(&vecRD[i][j]));
        }
    }

    _rdTable = tmpRDTable;
    return true;
}

//=====================================================================
RelativeDirection Intersection::atoRD(string* str)
{
    if((*str)=="s")
    {
        return RD_STRAIGHT;
    }
    else if((*str)=="t")
    {
        return RD_BACK;
    }
    else if((*str)=="l")
    {
        return RD_LEFT;
    }
    else if((*str)=="r")
    {
        return RD_RIGHT;
    }
    else
    {
        return RD_NONE;
    }
}

//======================================================================
// ODノードの場合はODNode::createDefaultStreetVertices()が呼び出される
bool Intersection::createDefaultStreetVertices()
{
    /*
     * 交差点はxy平面に平行で、頂点配列は左回りであることを前提とする
     * 接続数が4以下の場合，接続交差点にむかう2本のベクトルを用意して
     * 形状を規定する
     *
     * 実際には接続交差点の数だけ方向ベクトルを用意するが，
     * 基本となる2本を反転したもの
     *
     * - 2: 1->0,0->1
     * - 3: 2->0,自分->1 (0->2がRD_STRAIGHTの場合)
     * - 4: 0->2,1->3
     */

#ifdef IB_DEBUG
    cout << "Intersection ID:" << _id << endl;
#endif

    // 接続する単路の交点を求める
    vector<AmuPoint> crossPoint(_next.size());
    for (unsigned int i=0; i<_next.size(); i++)
    {
        AmuVector thisDV(_center, _next[i]->center());
        thisDV.setZ(0);
        thisDV.normalize();
        AmuVector prevDV(_center,
                         _next[(i+_next.size()-1)%_next.size()]->center());
        prevDV.setZ(0);
        prevDV.normalize();
        double theta = thisDV.calcAngle(prevDV);

#ifdef IB_DEBUG
        cout << " "<< i << ":" << prevDV.x() << "," << prevDV.y() << " - "
             << thisDV.x() << "," << thisDV.y() << ":"
             << theta*180*M_1_PI << endl;
#endif

        if (sin(theta)>1.0e-5)
        {
#ifdef INCLUDE_TRAMS
           crossPoint[i] = _center
                +(prevDV
                  *((_numIn[i]
                     + _numOut[i]
                     + _tramExt
                     ->numTotalTramLanes(i))
                    * 0.5*_incSections[i]->laneWidth()
                    + _incSections[i]->roadsideWidth())/sin(theta))
                +(thisDV
                  *((_numIn[(i+_next.size()-1)%_next.size()]
                     + _numOut[(i+_next.size()-1)%_next.size()]
                     + _tramExt
                     -> numTotalTramLanes((i+_next.size()-1)%_next.size()))
                    * 0.5*_incSections[i]->laneWidth()
                    + _incSections[i]->roadsideWidth())/sin(theta));
#else //INCLUDE_TRAMS not defined
           crossPoint[i] = _center
                +(prevDV
                  *((_numIn[i]+_numOut[i])*0.5*_incSections[i]->laneWidth()
                    + _incSections[i]->roadsideWidth())/sin(theta))
                +(thisDV
                  *((_numIn[(i+_next.size()-1)%_next.size()]
                     +_numOut[(i+_next.size()-1)%_next.size()])
                    * 0.5*_incSections[i]->laneWidth()
                    + _incSections[i]->roadsideWidth())/sin(theta));
#endif //INCLUDE_TRAMS
        }
        else
        {
            /*
             * sin(theta)が0の場合
             * （なす角が0[rad]またはpi[rad]では交点は不定）
             * _next.size()が2でなければ_insideDirectionVector(i+1)が
             * 交差する単路であるはずなのでそれを利用する
             * _next.size()が2の場合には法線ベクトルを利用する
             */
            if ((*_rdTable)(i,(i+1)%_next.size())==RD_RIGHT)
            {
#ifdef INCLUDE_TRAMS
                double ratio
                    = ((_numIn[i]
                        + _tramExt
                        ->numTotalTramLanes(i)
                        /2.0)
                       * (_numOut[(i+1)%_next.size()]
                          + _tramExt
                          ->numTotalTramLanes((i+1)%_next.size())
                          /2.0)
                       + (_numOut[(i+_next.size()-1)%_next.size()]
                          + _tramExt
                          -> numTotalTramLanes((i+_next.size()-1)%_next.size())
                          /2.0)
                       * (_numIn[(i+1)%_next.size()]
                          + _tramExt
                          -> numTotalTramLanes((i+1)%_next.size())
                          /2.0))
                    / (_numIn[(i+1)%_next.size()]
                       + _numOut[(i+1)%_next.size()]
                       + _tramExt
                       ->numTotalTramLanes((i+1)%_next.size()));
                crossPoint[i]
                    = _center
                    -(_insideDirectionVector((i+1)%_next.size())
                      *(_incSections[i]->laneWidth()*ratio
                        +_incSections[i]->roadsideWidth()));
#else //INCLUDE_TRAMS not defined
                double ratio
                    = (_numIn[i]*_numOut[(i+1)%_next.size()]
                       + _numOut[(i+_next.size()-1)%_next.size()]
                       *_numIn[(i+1)%_next.size()])
                    / (_numIn[(i+1)%_next.size()]
                       +_numOut[(i+1)%_next.size()]);
                crossPoint[i]
                    = _center
                    -(_insideDirectionVector((i+1)%_next.size())
                      *(_incSections[i]->laneWidth()*ratio
                        +_incSections[i]->roadsideWidth()));
#endif //INCLUDE_TRAMS
            }
            else
            {
                AmuVector v = _insideDirectionVector(i);
                v.revoltXY(M_PI_2);
#ifdef INCLUDE_TRAMS
                crossPoint[i]
                    = _center
                    - v*(_incSections[i]->laneWidth()
                         *(_numIn[i]
                           + _tramExt
                           -> numTotalTramLanes(i)
                           /2.0
                           + _numOut[(i+_next.size()-1)%_next.size()]
                           + _tramExt
                           -> numTotalTramLanes
                           ((i+_next.size()-1)%_next.size())
                           /2.0)
                         *0.5
                         +_incSections[i]->roadsideWidth());
#else //INCLUDE_TRAMS not defined
                crossPoint[i]
                    = _center
                    - v*(_incSections[i]->laneWidth()
                         *(_numIn[i]+_numOut[(i+_next.size()-1)
                                             %_next.size()])*0.5
                         +_incSections[i]->roadsideWidth());
#endif //INCLUDE_TRAMS
            }
        }
    }

#ifdef IB_DEBUG
    cout << " CrossPoint:" << endl;
    for (int i=0; i<crossPoint.size(); i++)
    {
        cout << " " << i << ":"
             << crossPoint[i].x() << ","
             << crossPoint[i].y() << ","
             << crossPoint[i].z() << endl;
    }
#endif

    //多角形の頂点を決定する
    for (int i=0; i<static_cast<signed int>(_next.size()); i++)
    {
        double margin = 2.0;

        AmuPoint tmpVertexL,tmpVertexR;
        tmpVertexL = crossPoint[i]
            + _insideDirectionVector(i)*margin;
        tmpVertexR = crossPoint[(i+1)%_next.size()]
            + _insideDirectionVector(i)*margin;

        // 境界の方向ベクトル
        AmuVector bv;
        if ( _next.size()<=4
             && (*_rdTable)(i, (i+1)%_next.size())==RD_RIGHT
             && (*_rdTable)(i, (i+_next.size()-1)%_next.size())==RD_LEFT)
        {
            bv = _insideDirectionVector((i+1)%_next.size());
        }
        else
        {
            bv = _insideDirectionVector(i);
            bv.revoltXY(M_PI_2);
        }

        // 道路の中心線（境界の中点を通る）
        AmuLineSegment centerLine(_center,
                                  AmuPoint(_next[i]->center().x(),
                                           _next[i]->center().y(),
                                           _center.z()));
        // 境界と中心線の交点を求めるための線分．長さは適当で構わない．
        AmuLineSegment borderLineL(tmpVertexL, tmpVertexL+bv*100);
        AmuLineSegment borderLineR(tmpVertexR, tmpVertexR-bv*100);

#ifdef IB_DEBUG
        cout << "Center of Border:" << endl;
        cout << " borderVector:"
             << bv.x() << ","
             << bv.y() << endl;
        cout << " centerLine:"
             << centerLine.pointBegin().x() << ","
             << centerLine.pointBegin().y() << ","
             << centerLine.pointBegin().z() << "-"
             << centerLine.pointEnd().x() << ","
             << centerLine.pointEnd().y() << ","
             << centerLine.pointEnd().z() << endl;
        cout << " borderLineL:"
             << borderLineL.pointBegin().x() << ","
             << borderLineL.pointBegin().y() << ","
             << borderLineL.pointBegin().z() << "-"
             << borderLineL.pointEnd().x() << ","
             << borderLineL.pointEnd().y() << ","
             << borderLineL.pointEnd().z() << endl;
        cout << " borderLineR:"
             << borderLineR.pointBegin().x() << ","
             << borderLineR.pointBegin().y() << ","
             << borderLineR.pointBegin().z() << "-"
             << borderLineR.pointEnd().x() << ","
             << borderLineR.pointEnd().y() << ","
             << borderLineR.pointEnd().z() << endl;
#endif

        AmuPoint midPoint, midPointL, midPointR;
        if (!centerLine.createIntersectionPoint(borderLineL, &midPointL))
        {
            midPointL = _center;
        }

        if (!centerLine.createIntersectionPoint(borderLineR, &midPointR))
        {
            midPointR = _center;
        }

        // 中点の遠い方を採用
        if (_center.distance(midPointL)>=_center.distance(midPointR))
        {
            midPoint = midPointL;
        }
        else
        {
            midPoint = midPointR;
        }

        // 交差点サイズ制限、レーン数が入出力で違う三叉路だと
        // 長大になる可能性
        AmuVector mv(_center, midPoint);
        mv.setZ(0);
        double maxMidVec;
        GVManager::getVariable("INTERSECTION_SIZE_LIMIT",
                               &maxMidVec);
        if (mv.size() > maxMidVec)
        {
            mv.normalize();
            mv = mv * maxMidVec;
            midPoint = _center;
            midPoint += mv;
        }

#ifdef IB_DEBUG
        cout << " midL:"
             << midPointL.x() << ","
             << midPointL.y() << ","
             << midPointL.z() << endl;
        cout << " midR:"
             << midPointR.x() << ","
             << midPointR.y() << ","
             << midPointR.z() << endl;
        cout << " mid :"
             << midPoint.x() << ","
             << midPoint.y() << ","
             << midPoint.z() << endl;
#endif

        AmuVector dv(_center, _next[i]->center());
        dv.setZ(0);
        dv.revoltXY(M_PI_2);
        double theta = dv.calcAngle(bv);

#ifdef INCLUDE_TRAMS
        tmpVertexL = midPoint - bv
            * ((_numIn[i]+_numOut[i]+_tramExt->numTotalTramLanes(i))
               *0.5*_incSections[i]->laneWidth()
               +_incSections[i]->roadsideWidth()) / abs(cos(theta));
        tmpVertexR = midPoint + bv
            * ((_numIn[i]+_numOut[i]+_tramExt->numTotalTramLanes(i))
               *0.5*_incSections[i]->laneWidth()
               +_incSections[i]->roadsideWidth()) / abs(cos(theta));
#else //INCLUDE_TRAMS not defined
        tmpVertexL = midPoint - bv
            * ((_numIn[i]+_numOut[i])*0.5*_incSections[i]->laneWidth()
               +_incSections[i]->roadsideWidth()) / abs(cos(theta));
        tmpVertexR = midPoint + bv
            * ((_numIn[i]+_numOut[i])*0.5*_incSections[i]->laneWidth()
               +_incSections[i]->roadsideWidth()) / abs(cos(theta));
#endif //INCLUDE_TRAMS

#ifdef IB_DEBUG
        cout << "Vertex:" << endl;
        cout << " " << i << "th L:"
             << tmpVertexL.x() << ","
             << tmpVertexL.y() << ","
             << tmpVertexL.z() << endl;
        cout << " " << i << "th R:"
             << tmpVertexR.x() << ","
             << tmpVertexR.y() << ","
             << tmpVertexR.z() << endl;
#endif

        _streetVertices.push_back(tmpVertexL);
        _streetVertices.push_back(tmpVertexR);
    }
    return true;
}

//======================================================================
bool Intersection::createStreetVerticesFromFile(ifstream* isf)
{
    string str;
    int numofVertices=0;
    while((*isf).good())
    {
        AmuPoint tmpVertex;
        getline(*isf,str);
        AmuStringOperator::getAdjustString(&str);
        if(!str.empty())
        {
            vector<string> tokens;
            AmuStringOperator::getTokens(&tokens,str,',');
            if(tokens.size()!=3)
            {
                cerr << "File:" << _id << ".txt , " << numofVertices
                     << "th vertex; You must put 3 values for x-y-z coordinates." << endl;
                exit(1);
            }
            tmpVertex.setX(_center.x() + atof(tokens[0].c_str()));
            tmpVertex.setY(_center.y() + atof(tokens[1].c_str()));
            tmpVertex.setZ(_center.z() + atof(tokens[2].c_str()));
            _streetVertices.push_back(tmpVertex);
            ++numofVertices;
        }
    }
    if(_streetVertices.size()!=_next.size()*2)
    {
        cerr << "File:" << _id << ".txt , you must put " << _next.size()*2
             << " values for " << _next.size() << "vertices." << endl;
        exit(1);
    }
    return true;
}

//======================================================================
// ODノードの場合はODNode::createVertices()が呼び出される
bool Intersection::createVertices()
{

    // 交差点幅と歩道幅の分だけ外側に広げる、途中で車道境界の点も計算する
    // 0 幅で処理を分けないと微小にずれる
    for (unsigned int i = 0; i < _next.size(); i++)
    {
        int connectDir = _next[i]->direction(this);
        AmuLineSegment streetLine1(_streetVertices[i * 2],
                                   _next[i]
                                   ->_streetVertices[connectDir * 2 + 1]);
        AmuLineSegment streetLine2(_streetVertices[i * 2 + 1],
                                   _next[i]
                                   ->_streetVertices[connectDir * 2]);
        AmuPoint p1, p2;
        double curCrosswalkWidth = crosswalkWidth(i);
        if (curCrosswalkWidth == 0.0)
        {
            p1 = _streetVertices[i * 2];
            p2 = _streetVertices[i * 2 + 1];
        }
        else
        {
            p1 = streetLine1.createInteriorPoint(crosswalkWidth(i),
                                                 streetLine1.length()
                                                 - curCrosswalkWidth);
            p2 = streetLine2.createInteriorPoint(crosswalkWidth(i),
                                                 streetLine2.length()
                                                 - curCrosswalkWidth);
        }

        _borderPoints.push_back(p1);
        _borderPoints.push_back(p2);
        double sidewalkWidth1 = _incSections[i]->sidewalkWidth(this, false);
        double sidewalkWidth2 = _incSections[i]->sidewalkWidth(this, this);

        if (sidewalkWidth1 != 0.0 || sidewalkWidth2 != 0.0)
        {
            AmuLineSegment crosswalkLine(p1, p2);
            AmuVector crosswalkVector = crosswalkLine.directionVector();
            crosswalkVector.normalize();
            p1 -= crosswalkVector * sidewalkWidth1;
            p2 += crosswalkVector * sidewalkWidth2;
        }
        addVertex(p1);
        addVertex(p2);
    }

    /*
     * 以下不都合な図形の補正処理、見た目が悪いだけでなく歩行ができない
     * 車道頂点と違う場合のみ補正する
     * ほぼ補正できると思われるが、あまりにひどい場合は
     * 補正できないかもしれない
     * z = 0 平面上で交差計算する、z を 0 にして交点を求めて戻す
     */

    // 角の補正、左回りで接続単路の次の角、outerIndex1 - 2 が対象の角
    bool revise = false;
    vector<bool> reviseCorner;
    AmuPoint revisePoint;
    for (unsigned int i = 0; i < _next.size(); i++)
    {
        reviseCorner.push_back(false);
    }
    for (unsigned int i = 0; i < _next.size(); i++)
    {
        int outerIndex0 = i * 2;
        int outerIndex1 = (i * 2 + 1) % _vertices.size();
        int outerIndex2 = (i * 2 + 2) % _vertices.size();
        int outerIndex3 = (i * 2 + 3) % _vertices.size();
        if (!(_vertices[outerIndex1] == _streetVertices[outerIndex1])
            && !(_vertices[outerIndex2] == _streetVertices[outerIndex2]) )
        {
            AmuLineSegment outerLine01(_vertices[outerIndex0],
                                       _vertices[outerIndex1]);
            AmuLineSegment outerLine12(_vertices[outerIndex1],
                                       _vertices[outerIndex2]);
            AmuLineSegment outerLine23(_vertices[outerIndex2],
                                       _vertices[outerIndex3]);

            // 交差の補正、頂点の中点を使う
            AmuPoint tmpPoint;
            if (outerLine01.checkIntersectionZ0(outerLine23, &tmpPoint))
            {
                revisePoint = outerLine12.createInteriorPoint(1.0, 1.0);
                reviseCorner[i] = true;
            }
            // 凹角の補正、外側の頂点を使う
            else if (!outerLine01.isLeftSide(_vertices[outerIndex2]))
            {
                revisePoint = _vertices[outerIndex2];
                reviseCorner[i] = true;
            }
            else if (!outerLine23.isLeftSide(_vertices[outerIndex1]))
            {
                revisePoint = _vertices[outerIndex1];
                reviseCorner[i] = true;
            }
            // 頂点の設定
            if (reviseCorner[i])
            {
                revise = true;
                _vertices[outerIndex1]
                    = _vertices[outerIndex2]
                    = revisePoint;
            }
        }
    }

    // 単路車道に関する補正、角の補正が行われた場所のみ行う、
    // outerIndex0 - 1 が対象の単路
    for (unsigned int i = 0; i < _next.size(); i++)
    {
        if (reviseCorner[i]
            || reviseCorner[(i + _next.size() - 1) % _next.size()])
        {
            int outerIndexM2 = (i * 2 + _vertices.size() - 2)
                % _vertices.size();
            int outerIndexM1 = (i * 2 + _vertices.size() - 1)
                % _vertices.size();
            int outerIndex0  = i * 2;
            int outerIndex1  = (i * 2 + 1) % _vertices.size();
            int outerIndex2  = (i * 2 + 2) % _vertices.size();
            int outerIndex3  = (i * 2 + 3) % _vertices.size();
            int connectDir = _next[i]->direction(this);
            AmuLineSegment outerLine01(_vertices[outerIndex0],
                                       _vertices[outerIndex1]);
            AmuLineSegment streetLine0
                (_streetVertices[outerIndex0],
                 _next[i]->_streetVertices[connectDir * 2 + 1]);
            AmuLineSegment streetLine1
                (_streetVertices[outerIndex1],
                 _next[i]->_streetVertices[connectDir * 2]);

            /*
             * 車道へ入り込む頂点の補正と、車道境界点の補正
             * 車道と外部頂点による線の交点から車道境界点を作る
             * 交点がない場合は車道に入り込んでいるとみなし、
             * 別の外部頂点による線との交点から車道境界点を作る
             * 車道境界点が作れない場合は外部頂点を車道境界点とする
             *（ちょうど車道の線上）
             * 車道には入り込んでいる場合と単路歩道がない場合は
             * 車道境界点に合わせて外部頂点を直す
             */
            if (!streetLine0.checkIntersectionZ0
                (outerLine01, &_borderPoints[outerIndex0]))
            {
                AmuLineSegment outerLineM2M1
                    (_vertices[outerIndexM2], _vertices[outerIndexM1]);
                if (!streetLine0.checkIntersectionZ0
                    (outerLineM2M1, &_borderPoints[outerIndex0]))
                {
                    _borderPoints[outerIndex0] = _vertices[outerIndex0];
                }
                _vertices[outerIndex0]
                    = _vertices[outerIndexM1]
                    = _borderPoints[outerIndex0];
            }
            if (_incSections[i]->sidewalkWidth(this, false) == 0.0)
            {
                _vertices[outerIndex0] = _borderPoints[outerIndex0];
            }
            if (!streetLine1.checkIntersectionZ0
                (outerLine01, &_borderPoints[outerIndex1]))
            {
                AmuLineSegment outerLine23
                    (_vertices[outerIndex2], _vertices[outerIndex3]);
                if (!streetLine1.checkIntersectionZ0
                    (outerLine23, &_borderPoints[outerIndex1]))
                {
                    _borderPoints[outerIndex1] = _vertices[outerIndex1];
                }
                _vertices[outerIndex1]
                    = _vertices[outerIndex2]
                    = _borderPoints[outerIndex1];
            }
            if (_incSections[i]->sidewalkWidth(this, true)  == 0.0)
            {
                _vertices[outerIndex1] = _borderPoints[outerIndex1];
            }
        }
    }

    if (revise)
    {
        cout << "warning: revise intersection structure " << _id << endl;
    }
    return true;
}

//======================================================================
bool ODNode::createDefaultStreetVertices()
{
    //単路の単位方向ベクトル（隣接交差点へ向かう方向が正）を作成
    AmuVector dv;
    dv.setX(_next[0]->center().x()-_center.x());
    dv.setY(_next[0]->center().y()-_center.y());
    dv.setZ(0);
    dv.normalize();

    //単位法線ベクトルを作成
    AmuVector nv;
    nv = _insideDirectionVector(0);
    nv.revoltXY(M_PI_2);
    AmuPoint tmpVertex;

    double tmpWidth = 2.0; //仮の道路幅(実際には接続単路のない境界)
    double tmpX,tmpY;
    tmpVertex.setZ(_center.z());

    int numConnectors = _numIn[0] + _numOut[0];

#ifdef INCLUDE_TRAMS
    numConnectors += _tramExt->numTotalTramLanes(0);
#endif //INCLUDE_TRAMS

    tmpX = _center.x()
        -nv.x()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())+dv.x()*tmpWidth;
    tmpY = _center.y()
        -nv.y()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())+dv.y()*tmpWidth;
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    _streetVertices.push_back(tmpVertex);

    tmpX = _center.x()
        +nv.x()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())+dv.x()*tmpWidth;
    tmpY = _center.y()
        +nv.y()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())+dv.y()*tmpWidth;
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    _streetVertices.push_back(tmpVertex);

    tmpX = _center.x()
        +nv.x()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())-dv.x()*tmpWidth;
    tmpY = _center.y()
        +nv.y()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())-dv.y()*tmpWidth;
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    _streetVertices.push_back(tmpVertex);

    tmpX = _center.x()
        -nv.x()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())-dv.x()*tmpWidth;
    tmpY = _center.y()
        -nv.y()*(numConnectors*0.5*_incSections[0]->laneWidth()
                 +_incSections[0]->roadsideWidth())-dv.y()*tmpWidth;
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    _streetVertices.push_back(tmpVertex);

    return true;
}

//======================================================================
bool ODNode::createVertices()
{
    // ODNode では車道境界の点＝車道多角形の頂点
    for (unsigned int i = 0; i < _streetVertices.size(); i++)
    {
        _borderPoints.push_back(_streetVertices[i]);
    }

    //単位法線ベクトルを作成
    AmuVector nv;
    nv = _insideDirectionVector(0);
    nv.revoltXY(M_PI_2);
    AmuPoint tmpVertex;

    double tmpX,tmpY;
    tmpVertex.setZ(_center.z());

    tmpX
        = _streetVertices[0].x()
        -nv.x()*_incSections[0]->sidewalkWidth(this, false);
    tmpY
        = _streetVertices[0].y()
        -nv.y()*_incSections[0]->sidewalkWidth(this, false);
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    addVertex(tmpVertex);

    tmpX
        = _streetVertices[1].x()
        +nv.x()*_incSections[0]->sidewalkWidth(this, true);
    tmpY
        = _streetVertices[1].y()
        +nv.y()*_incSections[0]->sidewalkWidth(this, true);
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    addVertex(tmpVertex);

    tmpX
        = _streetVertices[2].x()
        +nv.x()*_incSections[0]->sidewalkWidth(this, true);
    tmpY
        = _streetVertices[2].y()
        +nv.y()*_incSections[0]->sidewalkWidth(this, true);
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    addVertex(tmpVertex);

    tmpX
        = _streetVertices[3].x()
        -nv.x()*_incSections[0]->sidewalkWidth(this, false);
    tmpY
        = _streetVertices[3].y()
        -nv.y()*_incSections[0]->sidewalkWidth(this, false);
    tmpVertex.setX(tmpX);
    tmpVertex.setY(tmpY);
    addVertex(tmpVertex);

    return true;
}

//======================================================================
// ODノードの場合はODNode::createDefaultBorders()が呼び出される
bool Intersection::createDefaultBorders()
{
    // デフォルトでは_border[i]は_vertex[2i]と_vertex[2i+1]を結ぶ辺
    for (unsigned int i=0; i<_next.size(); i++)
    {
        Border* border;
#ifdef INCLUDE_TRAMS
        if (_tramExt->numTotalTramLanes(i)>0)
        {

            border = new BorderTram(_borderPoints[2 * i],
                                    _borderPoints[2 * i + 1],
                                    _numIn[i], _numOut[i],
                                    _tramExt, i,
                                    _incSections[i]->roadsideWidth(),
                                    false);

        }
        else
        {
            border = new Border(_borderPoints[2 * i],
                                _borderPoints[2 * i + 1],
                                _numIn[i], _numOut[i],
                                _incSections[i]->roadsideWidth());
        }
#else //INCLUDE_TRAMS not defined
        border = new Border(_borderPoints[2 * i],
                            _borderPoints[2 * i + 1],
                            _numIn[i], _numOut[i],
                            _incSections[i]->roadsideWidth());
#endif //INCLUDE_TRAMS
        assert(border);
        addBorder(border, i*2);
    }
    return true;
}

//======================================================================
bool ODNode::createDefaultBorders()
{
    AmuLineSegment line;
    AmuVector v;

    // _edge[0]に_border[0]を作成する
    Border* border0 = NULL;
    Border* border1 = NULL;
#ifdef INCLUDE_TRAMS
    if (_tramExt->numTotalTramLanes(0)>0)
    {
        border0 = new BorderTram(_borderPoints[0],
                                 _borderPoints[1],
                                 _numIn[0], _numOut[0],
                                 _tramExt, 0,
                                 _incSections[0]->roadsideWidth(),
                                 false);
        // _edge[2]に_border[1]を作成する(_border[0]と逆向き)
        border1 = new BorderTram(_borderPoints[2],
                                 _borderPoints[3],
                                 _numOut[0], _numIn[0],
                                 _tramExt, 0,
                                 _incSections[0]->roadsideWidth(),
                                 true);
    }
    else
    {
        border0 = new Border(_borderPoints[0],
                             _borderPoints[1],
                             _numIn[0], _numOut[0],
                             _incSections[0]->roadsideWidth());
        // _edge[2]に_border[1]を作成する(_border[0]と逆向き)
        border1 = new Border(_borderPoints[2], _borderPoints[3],
                         _numOut[0], _numIn[0],
                         _incSections[0]->roadsideWidth());
    }
#else //INCLUDE_TRAMS not defined
    border0 = new Border(_borderPoints[0], _borderPoints[1],
                         _numIn[0],_numOut[0],
                         _incSections[0]->roadsideWidth());
    // _edge[2]に_border[1]を作成する(_border[0]と逆向き)
    border1 = new Border(_borderPoints[2], _borderPoints[3],
                         _numOut[0], _numIn[0],
                         _incSections[0]->roadsideWidth());
#endif //INCLUDE_TRAMS

    assert(border0);
    addBorder(border0, 0);

    assert(border1);
    addBorder(border1, 2);

    return true;
}

//======================================================================
// ODノードの場合はODNode::createDefaultSubsections()が呼び出される
bool Intersection::createDefaultSubsections()
{
    // 極座標系でr方向に内側から0,1,2,…、
    // theta方向に辺0から左回りに0,1,2,…と番号づけ
    // (r,theta)の組でIDを表す

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 中央サブセクション(ID:00)を作成する
    SubIntersection* ptSubI
        = new SubIntersection("00", SUBROAD_TYPE_STREET);
    ptSubI->setLaneBundle(this);

    //通行権の設定
    ptSubI->setPermission(TRAFFIC_VEHICLE_ANY);
    bool initWalkerType = true;
    for (int i=0; i<static_cast<signed int>(_next.size()); i++)
    {
        vector<int> walkerTypes;
        if (crosswalkWidth(i)==0 &&
            _incSections[i]->streetTrafficWalkerTypes(walkerTypes))
        {
            // 接続単路の車道を歩行者が通行でき，
            // かつ境界に横断歩道が設置されていない場合は
            // 歩行者が中央サブセクションを通行できる
            // 歩行者種別は最初だけ初期化する
            ptSubI->setPermission(TRAFFIC_ANY, initWalkerType);
            initWalkerType = false;
            ptSubI->addWalkerTypes(walkerTypes);
        }
    }

    for (unsigned int i = 0; i < _streetVertices.size(); i++)
    {
        ptSubI->addVertex(_streetVertices[i]);
    }
    ptSubI->createCenter();
    _entities["00"] = ptSubI;
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 周回サブセクション(ID:10〜)を作成する

    int r = 10;
    for (unsigned int i=0; i<_vertices.size(); i++)
    {
        if (i%2==0)
        {
            if (crosswalkWidth(edge2dir(i))!=0)
            {
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 横断歩道
                string id
                    = AmuConverter::itos(r+i, NUM_FIGURE_FOR_SUBSECTION);

#ifdef INCLUDE_PEDESTRIANS
                Zebra* zebra = new Zebra(id);
                // ZebraクラスはSubIntersectionのサブクラス
                SubIntersection* ptSubI = zebra;
                _pedExt->setZebra(zebra);
#else //INCLUDE_PEDESTRIANS not defined
                SubIntersection* ptSubI
                    = new SubIntersection(id, SUBROAD_TYPE_CROSSWALK);
#endif //INCLUDE_PEDESTRIANS

                ptSubI->setLaneBundle(this);
                ptSubI->setPermission(TRAFFIC_ANY);

                //辺番号から境界番号を探す
                int e2d = edge2dir(i);
                assert(e2d!=-1);

                // 接続単路歩行可能なら赤信号でも歩行可能
                // 本来は全員歩行可能なら信号ごと削りたいが難しい
                vector<int> walkerTypes;
                if (_incSections[e2d]
                    ->streetTrafficWalkerTypes(walkerTypes))
                {
                    ptSubI->setWalkerTypes(walkerTypes,
                                           TRAFFIC_SIGNAL_RED);
                }

                // _border[e2d]の始点から左回りの四角形
                //+++++++++++++++++++++++++++++++++++
                // edge0:単路
                // edge1:横断歩道終点
                // edge2:中心サブセクション
                // edge3:横断歩道始点
                //+++++++++++++++++++++++++++++++++++
                ptSubI->addVertex(_borderPoints[i]);
                ptSubI->addVertex(_borderPoints[i + 1]);
                ptSubI->addVertex(_streetVertices[i + 1]);
                ptSubI->addVertex(_streetVertices[i]);
                ptSubI->createCenter();
                _entities[id] = ptSubI;

#ifdef INCLUDE_PEDESTRIANS
                zebra->initialize(e2d);
#endif //INCLUDE_PEDESTRIANS
            }
        }
        else
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // サイドウォーク
            int e2d[2];
            e2d[0] = edge2dir((i+_vertices.size()-1)%_vertices.size());
            e2d[1] = edge2dir((i+1)%_vertices.size());
            assert(e2d[0]!=-1 && e2d[1]!=-1);

            if (_incSections[e2d[0]]->sidewalkWidth(this, true)!=0
                || _incSections[e2d[1]]->sidewalkWidth(this, false)!=0)
            {
                string id
                    = AmuConverter::itos(r+i, NUM_FIGURE_FOR_SUBSECTION);
                SubIntersection* ptSubI
                    = new SubIntersection(id, SUBROAD_TYPE_SIDEWALK);
                ptSubI->setLaneBundle(this);
                ptSubI->setPermission(TRAFFIC_WALKER);

                // _border[e2d[0]]の終点から左回りの六角形
                // T字路の場合には見た目は四角形になるが、
                // 内部的には六つの辺を持つ
                //+++++++++++++++++++++++++++++++++++
                // edge0:始点側単路の歩道
                // edge1:接続なし
                // edge2:終点側単路の歩道
                // edge3:終点側横断歩道
                // edge4:交差点内中央サブセクション
                // edge5:始点側横断歩道
                //+++++++++++++++++++++++++++++++++++
                int iNext = (i + 1) % _vertices.size();
                if (_incSections[e2d[0]]->sidewalkWidth(this, true)!=0)
                {
                    ptSubI->addVertex(_borderPoints[i]);
                }
                ptSubI->addVertex(_vertices[i]);
                ptSubI->addVertex(_vertices[iNext]);
                if (_incSections[e2d[1]]->sidewalkWidth(this, false)!=0)
                {
                    ptSubI->addVertex(_borderPoints[iNext]);
                }
                if (crosswalkWidth(e2d[1])!=0)
                {
                    ptSubI->addVertex(_streetVertices[iNext]);
                }
                if (crosswalkWidth(e2d[0])!=0)
                {
                    ptSubI->addVertex(_streetVertices[i]);
                }
                ptSubI->createCenter();
                _entities[id] = ptSubI;
            }
        }
    }
    return true;
}

//======================================================================
bool ODNode::createDefaultSubsections()
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 車道サブセクション

    SubIntersection* ptSubI
        = new SubIntersection("00", SUBROAD_TYPE_STREET);
    ptSubI->setLaneBundle(this);

    ptSubI->setPermission(TRAFFIC_VEHICLE_ANY);
    vector<int> walkerTypes;
    if (crosswalkWidth(0)==0 &&
        _incSections[0]->streetTrafficWalkerTypes(walkerTypes))
    {
        // 接続単路の車道を歩行者が通行でき、
        // かつ境界に横断歩道が設置されていない場合は
        // 歩行者が中央サブセクションを通行できる
        ptSubI->setPermission(TRAFFIC_ANY);
        ptSubI->setWalkerTypes(walkerTypes);
    }

    for (unsigned int i = 0; i < _streetVertices.size(); i++)
    {
        ptSubI->addVertex(_streetVertices[i]);
    }
    ptSubI->createCenter();
    _entities["00"] = ptSubI;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // サイドウォーク

    if (_incSections[0]->sidewalkWidth(this, true)!=0)
    {
        SubIntersection* ptSubI1
            = new SubIntersection("11", SUBROAD_TYPE_SIDEWALK);
        ptSubI1->setLaneBundle(this);
        ptSubI1->setPermission(TRAFFIC_WALKER);
        ptSubI1->addVertex(_borderPoints[1]);
        ptSubI1->addVertex(_vertices[1]);
        ptSubI1->addVertex(_vertices[2]);
        ptSubI1->addVertex(_borderPoints[2]);
        ptSubI1->createCenter();
        _entities["11"] = ptSubI1;
    }

    if (_incSections[0]->sidewalkWidth(this, false)!=0)
    {
        SubIntersection* ptSubI3
            = new SubIntersection("13", SUBROAD_TYPE_SIDEWALK);
        ptSubI3->setLaneBundle(this);
        ptSubI3->setPermission(TRAFFIC_WALKER);
        ptSubI3->addVertex(_borderPoints[3]);
        ptSubI3->addVertex(_vertices[3]);
        ptSubI3->addVertex(_vertices[0]);
        ptSubI3->addVertex(_borderPoints[0]);
        ptSubI3->createCenter();
        _entities["13"] = ptSubI3;
    }

    return true;
}

//======================================================================
// Intersection, ODNodeで(今のところ)共通
bool Intersection::createDefaultConnectors()
{
    if(_next.size()!=1)
    {
        ITRMAPENT ite = _entities.begin();
        while (ite!=_entities.end())
        {
            int idInt = atoi((*ite).second->id().c_str());

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // 中心サブセクション
            if (idInt==0)
            {
            }

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // 周回サブセクション
            else
            {

                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 横断歩道の場合
                // 辺0のコネクタ
                //（Sectionに接するのでcreateBorderで作成済）と
                // 辺2のコネクタを結ぶレーンを設置する
                // border上のコネクタは内部コネクタではないので
                // ここでは作らない
                if ((*ite).second->type()==SUBROAD_TYPE_CROSSWALK)
                {
                    int dir = edge2dir(idInt - 10);
                    AmuLineSegment line
                        = (*ite).second->edge(2).reversal();
#ifdef INCLUDE_TRAMS
                    if (_tramExt->numTotalTramLanes(dir)>0)
                    {
                        _tramExt->createConnectorsOnLine
                            (line, _numIn[dir], _numOut[dir], dir);
                    }
                    else
                    {
                        _createConnectorsOnLine(line,
                                                _numIn[dir], _numOut[dir],
                                                dir);
                    }
#else //INCLUDE_TRAMS not defined
                    _createConnectorsOnLine(line,
                                            _numIn[dir], _numOut[dir],
                                            dir);
#endif //INCLUDE_TRAMS
                }
            }
            ite++;
        }
    }
    return true;
}

//======================================================================
// ODノードの場合はODNode::createDefaultLanes()が呼び出される
bool Intersection::createDefaultLanes()
{
    int idInt;
    int idIntBegin, idIntEnd;

    for (unsigned int i=0; i<_borders.size(); i++)
    {
        if (_numIn[i]>0)
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            // 中心サブセクション
            for (unsigned int j=0; j<_borders.size(); j++)
            {
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向なしの場合
                if ((*_rdTable)(i,j) == RD_NONE)
                {
                    continue;
                }
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向が転回の場合
                if ((*_rdTable)(i,j) == RD_BACK)
                {
                    continue;
                }
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向が右折の場合
                // 境界iのコネクタ_numIn[i]-1（最内の流入点）から
                // 境界jの各流出コネクタへ
                if ((*_rdTable)(i,j) == RD_RIGHT)
                {
                    if (_numOut[j]==0)
                    {
                        continue;
                    }
                    idIntBegin = i*100+_numIn[i]-1;
                    for (int k=_numIn[j]; k<_numIn[j]+_numOut[j]; k++)
                    {
                        idIntEnd = j*100+k;
                        idInt = idIntBegin*10000+idIntEnd;

                        const Connector* pointBegin
                            = edgeConnector(idIntBegin);
                        const Connector* pointEnd
                            = edgeConnector(idIntEnd);
                        assert(pointBegin && pointEnd);

                        createLane(idInt, pointBegin, pointEnd, true);
                    }
                }

                //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向が直進の場合
                // 境界iの流入点と境界jの流出点を外から順に結ぶ
                if ((*_rdTable)(i,j) == RD_STRAIGHT)
                {
                    if (_numOut[j]==0)
                    {
                        continue;
                    }

                    // 流入側が余るのを抑止するため、
                    // 次の場合は左２番目から接続
                    // - （流入数-流出数 = 2）の場合
                    // - （流入数-流出数 = 1）で右折がない場合
                    // 流出側が余ったら最後の流入につなげる
                    // 車は一台ずつなのでこれで動く
                    // 接続は反時計回り
                    int kAdd = 0;
                    if (_numIn[i] - _numOut[j] >= 2
                        || ( _numIn[i] - _numOut[j] == 1
                             && (i + _borders.size() + 1)
                             % _borders.size() == j))
                    {
                        kAdd++;
                    }
                    for (int k=0; k<_numOut[j]; k++)
                    {
                        if (k < _numIn[i])
                        {
                            idIntBegin = i*100 + k + kAdd;
                        }
                        else
                        {
                            idIntBegin = i*100 + _numIn[i] - 1;
                        }
                        idIntEnd = j*100+(_numIn[j]+_numOut[j]-1-k);
                        idInt = idIntBegin*10000+idIntEnd;

                        const Connector* pointBegin
                            = edgeConnector(idIntBegin);
                        const Connector* pointEnd
                            = edgeConnector(idIntEnd);
                        assert(pointBegin && pointEnd);

                        createLane(idInt, pointBegin, pointEnd, true);
                    }
                }

                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向が左折の場合
                // 境界iのコネクタ0(最外の流入点)から
                // 境界jの各流出コネクタへ
                if ((*_rdTable)(i,j) == RD_LEFT)
                {
                    if (_numOut[j]==0)
                    {
                        continue;
                    }
                    for (int k=_numIn[j]; k<_numIn[j]+_numOut[j]; k++)
                    {
                        idIntBegin = i*100;
                        idIntEnd = j*100+k;
                        idInt = idIntBegin*10000+idIntEnd;

                        const Connector* pointBegin
                            = edgeConnector(idIntBegin);
                        const Connector* pointEnd
                            = edgeConnector(idIntEnd);
                        assert(pointBegin && pointEnd);

                        createLane(idInt, pointBegin, pointEnd, true);
                    }
                }
            }
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 横断歩道
        if (crosswalkWidth(i)!=0)
        {
            for (int k=0; k<_numIn[i]; k++)
            {
                idIntBegin = 1*1000+i*100+k;
                idIntEnd   = i*100+k;
                idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = _borders[i]->connector(k);
                const Connector* pointEnd
                    = connector
                    (AmuConverter::itos(idIntEnd,
                                        NUM_FIGURE_FOR_CONNECTOR_LOCAL));
                if (pointBegin && pointEnd)
                {
                    createLane(idInt, pointBegin, pointEnd, false);
                }
            }
            for (int k=_numIn[i]; k<_numIn[i]+_numOut[i]; k++)
            {
                idIntBegin = i*100+k;
                idIntEnd   = 1*1000+i*100+k;
                idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = connector
                    (AmuConverter::itos(idIntBegin,
                                        NUM_FIGURE_FOR_CONNECTOR_LOCAL));
                const Connector* pointEnd  = _borders[i]->connector(k);
                if (pointBegin && pointEnd)
                {
                    createLane(idInt, pointBegin, pointEnd, false);
                }
            }
        }
    }

#ifdef INCLUDE_TRAMS
    _tramExt->createTramLanesInIntersection();
#endif //INCLUDE_TRAMS

    return true;
}

//======================================================================
bool Intersection::readLanesFromFile(ifstream* isf)
{
    string str;
    int numofLanes=0;

#ifdef IB_DEBUG
    cout << "Intersection ID:" << _id
         << ", unique Lanes below." << endl;
#endif

    while ((*isf).good())
    {
        getline((*isf),str);
        AmuStringOperator::getAdjustString(&str);
        if (str=="vertex"
            || str=="Vertex"
            || str=="VERTEX")
        {
            break;
        }
        else if(!str.empty())
        {

#ifdef IB_DEBUG
            cout << numofLanes+1 << " : " << str << endl;
#endif
            vector<string> tokens;
            AmuStringOperator::getTokens(&tokens,str,',');
            if (tokens.size()!=2)
            {
                cerr << "File:" << _id << ".txt , " << numofLanes+1
                     << "th lane; You must select 2 connectors." << endl;
                exit(1);
            }
            _laneBegEnd.push_back(tokens[0]);
            _laneBegEnd.push_back(tokens[1]);
            ++numofLanes;
        }
    }
    return true;
}

//======================================================================
// ODノードの場合はODNode::createLanesFromFile()が呼び出される
bool Intersection::createLanesFromFile()
{
    int numofLanes = (int)_laneBegEnd.size() / 2;

    int idInt;
    int idIntBegin, idIntEnd;

    for(int i=0; i<numofLanes; i++)
    {
        idIntBegin = atoi(_laneBegEnd[i*2].c_str());
        idIntEnd = atoi(_laneBegEnd[i*2+1].c_str());
        idInt = idIntBegin*10000 + idIntEnd;

        const Connector* pointBegin = edgeConnector(idIntBegin);
        const Connector* pointEnd   = edgeConnector(idIntEnd);
        assert(pointBegin && pointEnd);

        createLane(idInt, pointBegin, pointEnd, false);
    }

    for (unsigned int i=0; i<_borders.size(); i++)
    {
        if (_numIn[i]>0)
        {

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //中心サブセクション
            for (unsigned int j=0; j<_borders.size(); j++)
            {
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向なしの場合
                if ((*_rdTable)(i,j) == RD_NONE) continue;

                //++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // 相対方向が転回の場合
                if ((*_rdTable)(i,j) == RD_BACK) continue;

            }
        }

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // 横断歩道
        if (crosswalkWidth(i)!=0)
        {
            for (int k=0; k<_numIn[i]; k++)
            {
                idIntBegin = 1*1000+i*100+k;
                idIntEnd   = i*100+k;
                idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = _borders[i]->connector(k);
                const Connector* pointEnd
                    = connector
                    (AmuConverter::itos(idIntEnd,
                                        NUM_FIGURE_FOR_CONNECTOR_LOCAL));
                if (pointBegin && pointEnd)
                {
                    createLane(idInt, pointBegin, pointEnd, false);
                }
            }
            for (int k=_numIn[i]; k<_numIn[i]+_numOut[i]; k++)
            {
                idIntBegin = i*100+k;
                idIntEnd   = 1*1000+i*100+k;
                idInt = idIntBegin*10000+idIntEnd;

                const Connector* pointBegin
                    = connector
                    (AmuConverter::itos(idIntBegin,
                                        NUM_FIGURE_FOR_CONNECTOR_LOCAL));
                const Connector* pointEnd
                    = _borders[i]->connector(k);
                if (pointBegin && pointEnd)
                {
                    createLane(idInt, pointBegin, pointEnd, false);
                }
            }
        }
    }
    return true;
}

//======================================================================
void Intersection::createLane
(int idInt, const Connector* pointBegin,
 const Connector* pointEnd, bool setLaneBegEnd)
{
    string id;

    id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
    _lanes[id] = new LaneInIntersection(id, pointBegin, pointEnd, this);
    _lanes[id]
        ->setSpeedLimit(GVManager::getNumeric("SPEED_LIMIT_INTERSECTION"));
    if (setLaneBegEnd)
    {
        char buff1[16], buff2[16];
        sprintf(buff1, "%04d", idInt / 10000);
        sprintf(buff2, "%04d", idInt % 10000);
        _laneBegEnd.push_back(buff1);
        _laneBegEnd.push_back(buff2);
    }
}

//======================================================================
void Intersection::addLane(Lane* lane, bool setLaneBegEnd)
{
    _lanes[lane->id()] = lane;

    if (setLaneBegEnd)
    {
        int idInt = atoi(lane->id().c_str());
        char buff1[16], buff2[16];
        sprintf(buff1, "%04d", idInt / 10000);
        sprintf(buff2, "%04d", idInt % 10000);
        _laneBegEnd.push_back(buff1);
        _laneBegEnd.push_back(buff2);
    }
}

//======================================================================
bool ODNode::createDefaultLanes()
{
    int idInt;
    int idIntBegin, idIntEnd;
    string id;

    // 境界0のコネクタから境界1のコネクタへ
    for (int j=0; j<_numIn[0]; j++)
    {
        idIntBegin = j;
        idIntEnd   = 1*100+(_numOut[0]+_numIn[0]-1-j);
        idInt = idIntBegin*10000+idIntEnd;

        const Connector* pointBegin
            = _borders[0]->connector(j);
        const Connector* pointEnd
            = _borders[1]->connector(_numOut[0]+_numIn[0]-1-j);
        if (pointBegin && pointEnd)
        {
            id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
            _lanes[id] = new Lane(id, pointBegin, pointEnd, this);
        }
    }
    // 境界1のコネクタから境界0のコネクタへ
    for (int j=0; j<_numOut[0]; j++)
    {
        idIntBegin = 1*100+j;
        idIntEnd = _numOut[0]+_numIn[0]-1-j;
        idInt = idIntBegin*10000+idIntEnd;
        id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);

        const Connector* pointBegin
            = _borders[1]->connector(j);
        const Connector* pointEnd
            = _borders[0]->connector(_numOut[0]+_numIn[0]-1-j);
        if (pointBegin && pointEnd) {
            id = AmuConverter::itos(idInt, NUM_FIGURE_FOR_LANE);
            _lanes[id] = new Lane(id, pointBegin, pointEnd, this);
        }
    }

#ifdef INCLUDE_TRAMS
    _tramExt->createTramLanesInODNode();
#endif //INCLUDE_TRAMS

    return true;
}

//======================================================================
bool ODNode::createLanesFromFile()
{
    // 既定値と同じ処理だったので既定値を呼ぶよう修正した、ファイルは無視する
    // 実際には修正前も修正後も呼ばれていない
    return createDefaultLanes();
}

//======================================================================
const AmuVector Intersection::_insideDirectionVector(int i)
{
    assert(0<=i && i<static_cast<signed int>(_next.size()));

    // 単路の単位方向ベクトル（隣接交差点へ向かう方向が正）を作成
    AmuVector tmpVBegin, tmpVEnd, result;
    if (_next.size()==2)
    {
        tmpVBegin.setPoints(_center, _next[(i+1)%2]->center());
        tmpVBegin.normalize();
        tmpVEnd.setPoints(_center, _next[i]->center());
        tmpVEnd.normalize();
        result = tmpVEnd-tmpVBegin;
    }
    else if (_next.size()==3)
    {
        if ((*_rdTable)(i,(i+1)%3)==RD_STRAIGHT)
        {
            tmpVBegin.setPoints(_center, _next[(i+1)%3]->center());
            tmpVBegin.normalize();
            tmpVEnd.setPoints(_center, _next[i]->center());
            tmpVEnd.normalize();
            result = tmpVEnd-tmpVBegin;
        }
        else if ((*_rdTable)(i,(i+2)%3)==RD_STRAIGHT)
        {
            tmpVBegin.setPoints(_center, _next[(i+2)%3]->center());
            tmpVBegin.normalize();
            tmpVEnd.setPoints(_center, _next[i]->center());
            tmpVEnd.normalize();
            result = tmpVEnd-tmpVBegin;
        }
        else
        {
            result.setPoints(_center, _next[i]->center());
        }
    }
    else if (_next.size()==4)
    {
        tmpVBegin.setPoints(_center, _next[(i+2)%4]->center());
        tmpVBegin.normalize();
        tmpVEnd.setPoints(_center, _next[i]->center());
        tmpVEnd.normalize();
        result = tmpVEnd-tmpVBegin;
    }
    else
    {
        result.setPoints(_center, _next[i]->center());
    }
    result.setZ(0);
    result.normalize();

    return result;
}

//======================================================================
void Intersection::_createConnectorsOnLine(AmuLineSegment line,
					   int numIn, int numOut,
					   int borderId)
{
    string id;
    int sum = numIn+numOut;

    AmuVector vec = line.directionVector();
    vec.normalize();
    AmuLineSegment newLine(line.pointBegin()
                           +vec*_incSections[borderId]->roadsideWidth(),
                           line.pointEnd()
                           -vec*_incSections[borderId]->roadsideWidth());

    for (int i=0; i<sum; i++)
    {
        id = AmuConverter::itos(borderId*100+i,
                                NUM_FIGURE_FOR_CONNECTOR_LOCAL);
        const AmuPoint tmpPoint
            = newLine.createInteriorPoint(i * 2 + 1,
                                          sum * 2 - i * 2 - 1);
        if (!tmpPoint.flag())
        {
            cerr << "WARNING: Intersection::_createConnectorsOnLine(): bad Point."
                 << endl;
        }
        _innerConnectors[id] = ObjManager::createConnector(tmpPoint.x(),
                                                           tmpPoint.y(),
                                                           tmpPoint.z());
    }
}

//======================================================================
double Intersection::crosswalkWidth(int dir)
{
    // 以下デフォルト値
    // ODノード，接続数2の交差点には横断歩道なし
    assert(_hasStructInfo);
    if (_next.size()<=2)
    {
        return 0.0;
    }
    // 対応する単路と交差する単路の両方にsidewalkがなければ，横断歩道なし
    else if (_incSections[dir]->sidewalkWidth(this, true)==0
             && _incSections[dir]->sidewalkWidth(this, false)==0
             && _incSections[(dir+_next.size()-1)%_next.size()]
             ->sidewalkWidth(this, true)==0
             && _incSections[(dir+1)%_next.size()]
             ->sidewalkWidth(this, false)==0)
    {
        return 0.0;
    }
    // レーンなしなら横断歩道不要
    // 中央を通ってしまうが仕方ないか、あまりないと思われる
    else if (_numIn[dir]+_numOut[dir]<1)
    {
        return 0.0;
    }
    else
    {
        return _crosswalkWidth;
    }
}

//======================================================================
bool Intersection::createSubnetwork()
{
    ITRMAPENT ite;
    for (ite=_entities.begin(); ite!=_entities.end(); ite++)
    {
        for (int i=0;
             i<static_cast<signed int>((*ite).second->numVertices());
             i++)
        {
            RoadEntity* adjEntity = pairedEntity((*ite).second, i);
            if (adjEntity!=NULL)
            {
                (*ite).second->addAdjEntity(adjEntity, i);
            }
        }
    }
    return true;
}

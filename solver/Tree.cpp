/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <algorithm>
#include <string>
#include <fstream>
#include "Tree.h"
#include "Intersection.h"
#include "NodeAStar.h"

using namespace std;

//======================================================================
Tree::Tree(){}

//======================================================================
Tree::~Tree()
{
    //コンテナの中身を消去する。
    vector<NodeAStar*>::iterator itv;
  
    //メインコンテナについては参照しているオブジェクトも消去する。
    for (itv = _nodes.begin();itv != _nodes.end();itv++)
    {
        delete *itv;
    }
}

//======================================================================
//この関数は高速化をはかるべきである可能性が非常に高い。
//
bool Tree::_isExist(NodeAStar* node) const
{
    bool flag = false;
    vector<NodeAStar*>::const_iterator in;
    in = find(_nodes.begin(),_nodes.end(),node);
    if (in != _nodes.end())
    {
        flag = true;
    }
    return flag;
}

//======================================================================
NodeAStar* Tree::_findSame(NodeAStar* node) const
{
    NodeAStar* rnode = NULL;
    vector<NodeAStar*>::const_iterator in;
    for (in = _nodes.begin(); in != _nodes.end(); ++in)
    {
        if (node->compare(*(*in)))
        {
            rnode = (*in);
        }
    }
    return rnode;
}

//======================================================================
NodeAStar* Tree::_findReverse(NodeAStar* node) const
{
  
    NodeAStar* rnode = NULL;
    NodeAStar reverseNode(NULL,NULL);
    reverseNode.setTop(node->bottom());
    reverseNode.setBottom(node->top());

    vector<NodeAStar*>::const_iterator in;
    for (in = _nodes.begin(); in != _nodes.end(); ++in)
    {
        if (reverseNode.compare(*(*in)))
        {
            rnode = (*in);
        }
    }
    return rnode;
}


//======================================================================
void Tree::_rmParentFromLeaves(NodeAStar* node)
{
    // 祖先のノードが_leavesにあれば外す
    NodeAStar* anc = node->parent();
    multimap<double, NodeAStar*, less<double> >::iterator it;

    while (anc != NULL)
    {
        for (it = _leaves.begin(); it != _leaves.end(); ++it)
        {
            if((*it).second == anc)
            {
                _leaves.erase(it);
                break;
            }
        }
        anc = anc->parent();
    }
} 

//======================================================================
bool Tree::push(NodeAStar* node)
{
    bool rflag = false;//return value;

    double f = node->gv() + node->hv();
  
    bool shouldPush = true;

    NodeAStar* same = _findSame(node);
    if(same != NULL)
    {
        // 同じノードが見つかった場合
        double otherF = same->gv() + same->hv();
        if(otherF > f)
        {
            // 自分の評価値の方が小さい場合はsameノードを展開不能に
            same->disable();
        }
        else
        {
            // 自分の評価値の方が大きい場合は_nodesに追加しない
            shouldPush = false;
        }
    }

    if(shouldPush)
    {
        rflag = true;
        // _nodesに追加する
        _nodes.push_back(node);
        // _leavesに追加する
        _leaves.insert(pair<double,NodeAStar*>(f,node));
        _rmParentFromLeaves(node);
    }
    else
    {
        if(!_isExist(node))
        {
            delete node;
        }
        rflag = false;
    }
    
    return rflag;
}

//======================================================================
NodeAStar* Tree::pop()
{
    multimap<double,NodeAStar*,less<double> >::iterator itmu; 
    itmu = _leaves.begin();

    NodeAStar* node = NULL;
    if(itmu != _leaves.end())
    {
        node = (*itmu).second;
        _leaves.erase(itmu);
    }
    return node;
}

//======================================================================
NodeAStar* Tree::popAble()
{
    multimap<double,NodeAStar*,less<double> >::iterator itmu; 
    itmu = _leaves.begin();

    NodeAStar* node = NULL;
    bool isFound = false;
    while (itmu != _leaves.end() && !isFound)
    {
        if ((*itmu).second->isAble())
        {
            node = (*itmu).second;
            _leaves.erase(itmu);
            isFound = true;
        }
        else
        {
            ++itmu;
        } 
    }
    return node;
}

//======================================================================
NodeAStar* Tree::popUnable()
{
    multimap<double,NodeAStar*,less<double> >::iterator itmu; 
    itmu = _leaves.begin();

    NodeAStar* node = NULL;
    bool isFound = false;
    while( itmu != _leaves.end() && !isFound)
    {
        if (!(*itmu).second->isAble())
        {
            node = (*itmu).second;
            _leaves.erase(itmu);
            isFound = true;
        }
        else
        {
            ++itmu;
        }
    }
    return node;
}

//======================================================================
NodeAStar* Tree::leaf(int position) const
{
    NodeAStar* node = NULL;

    multimap<double,NodeAStar*,less<double> >::const_iterator itmu; 
    itmu = _leaves.begin();
  
    for (int i = 0; i<=position && itmu!=_leaves.end(); ++i,++itmu)
    {
        // _leavesはfに関して昇順にソートされている
        node = (*itmu).second;
    }
    return node;
}

//======================================================================
NodeAStar* Tree::leafAble(int position) const
{
    NodeAStar* node = NULL;

    multimap<double, NodeAStar*, less<double> >::const_iterator itmu; 
    itmu = _leaves.begin();
  
    for (int i=0; i<=position && itmu!=_leaves.end(); ++itmu)
    {
        if((*itmu).second->isAble())
        {
            ++i;
            node = (*itmu).second;
        }
    }
    return node;
}

//======================================================================
NodeAStar* Tree::leafUnable(int position) const
{
    NodeAStar* node = NULL;

    multimap<double,NodeAStar*,less<double> >::const_iterator itmu; 
    itmu = _leaves.begin();
  
    for (int i=0; i<=position && itmu!=_leaves.end(); ++itmu)
    {
        if (!(*itmu).second->isAble())
        {
            ++i;
            node = (*itmu).second;
        }
    }
    return node;
}

//======================================================================
void Tree::print(std::ostream& rout)
{
    multimap<double,NodeAStar*,less<double> >::const_iterator itm;
    for(itm = _leaves.begin();itm != _leaves.end();++itm)
    {
        if((*itm).second->isAble())
        {
            rout << "A";
        }
        else
        {
            rout << "D";
        }
        Intersection* tmp_m  = (*itm).second->top();
        NodeAStar* tmp_p = (*itm).second->parent();
        rout << tmp_m->id()<<",";
        while(tmp_p != NULL)
        {
            if(tmp_p->isAble())
            {
                rout << "A";
            }
            else
            {
                rout << "D";
            }
            tmp_m = tmp_p->top();
            tmp_p = tmp_p->parent();
            rout << tmp_m->id()<<",";
        }
        rout<<endl;
    }
}

//======================================================================
void Tree::print(std::ostream& rout, int counter)
{
    rout << "step:" << counter << endl;
    print(rout);
}
//======================================================================
int Tree::size()
{
    return _nodes.size();
}

//======================================================================
/*
  void Tree::draw() const{
//  bool flagStopped = false;
bool flagTop = false;
multimap<double,NodeAStar*,less<double> >::const_iterator it;
for(it = _leaves.begin(); it != _leaves.end();it++){
double z = 0.0;//,評価値が最小:13.0,展開可能:12.0,展開不可能:11.0
if(!flagTop){
if((*it).second->isAble()){
flagTop = true;
AutoGL_SetColor(1,0,0);
z = 13.0;
}else{
AutoGL_SetColor(0,1,0);
z = 11.0;
}
}else{
if((*it).second->isAble()){
AutoGL_SetColor(0,0,1);
z = 12.0;
}else{
AutoGL_SetColor(0,1,0);
z = 11.0;
}
}

//末端
if((*it).second->bottom() != NULL){
const Point t = (*it).second->top()->center();
const Point b = (*it).second->bottom()->center();
t.draw();
b.draw();
AutoGL_DrawBoldArrow2D(10.0,
b.x(),b.y(),z,
t.x(),t.y(),z);
}
    
NodeAStar* tmp_p = (*it).second->parent();
while(tmp_p != NULL){
if(tmp_p->bottom() != NULL){
const Point t = tmp_p->top()->center();
const Point b = tmp_p->bottom()->center();
t.draw();
b.draw();
AutoGL_DrawBoldArrow2D(10.0,
b.x(),b.y(),z,
t.x(),t.y(),z);
}
      
tmp_p = tmp_p->parent();
}
}
}
*/

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "../ObjManager.h"
#include "Pedestrian.h"
#include "../AmuConverter.h"
#include "../Conf.h"
#include <cassert>

using namespace std;

int ObjManager::numPedestrian = 0;
vector<Pedestrian*> ObjManager::_pedestrians;

//======================================================================
std::vector<Pedestrian*>* ObjManager::pedestrians()
{
    return &_pedestrians;
}

//======================================================================
Pedestrian* ObjManager::pedestrian(const std::string& id)
{
    for (unsigned int i=0; i<_pedestrians.size(); i++)
    {
        if (_pedestrians[i]->id().compare(id)==0)
        {
            return _pedestrians[i];
        }
    }
    return NULL;
}

//======================================================================
Pedestrian* ObjManager::createPedestrian()
{
    Pedestrian *pds = new Pedestrian();
    return pds;
}

//======================================================================
bool ObjManager::addPedestrianToReal(Pedestrian *pds)
{
    assert(pds);
    bool flag = false;

#ifdef PDS_DEBUG
    cerr << "addPedestrianToReal(): pds$(" << pds
         << ") : id()=[" << pds->id() << "]" << endl;
    cerr.flush();
#endif
    assert(pds->id().empty());

    pds->setId(AmuConverter::itos(numPedestrian,
                                  NUM_FIGURE_FOR_PEDESTRIAN));
    numPedestrian++;

#ifdef PDS_DEBUG
    cerr << "                          -> [" << pds->id()
         << "]$(" << pds << ")"
         << (pedestrian(pds->id())?"found!":"notfound") << endl;
    cerr.flush();
#endif

    // 既に同じ歩行者が生成されていないかチェックする
    if (pedestrian(pds->id())==NULL)
    {
        flag = true;
    }
    else
    {
#ifdef PDS_DEBUG
        cerr << "pds[" << pds->id()
             << "] has already generated... why?" << endl;
#endif
    }
    if (flag)
    {
        _pedestrians.push_back(pds);
    }

    return flag;
}

//======================================================================
void ObjManager::deleteFinishedPedestrians()
{
    vector<Pedestrian*>::iterator itp;

#ifdef DEBUG
    cerr << "pedestrians.size=" << _pedestrians.size() << endl;
    int i = 0;
#endif

    itp = _pedestrians.begin();
    while (itp != _pedestrians.end())
    {
    	if (!(*itp)->isOnZebra())
        {
            assert((*itp)->zebra() == NULL);

#ifdef DEBUG
            cerr << "   delete pedestrian(" << i <<
                ")[" << (*itp)->id() << "]$(" << (*itp) << ")" << endl;
            cerr.flush();
#endif
            /* 
             * 20150827 ここでdeleteすると、次のステップの
             * Zebra#renewPedestrianLine()内にあるインスタンスが
             *不正になって、誤評価してしまう(はず)
             * delete *itp;
             */
            itp = _pedestrians.erase(itp);
    	}
        else
        {
#ifdef DEBUG
            cerr << "          pedestrian(" << i
                 << ")[" << (*itp)->id() << "]$(" << (*itp) << ")" << endl;
            cerr.flush();
#endif
            itp++;
    	}
    }
#ifdef DEBUG
    cerr << "pedestrians.size=" << _pedestrians.size() << endl;
#endif
}

//======================================================================
void ObjManager::deleteAllPedestrians()
{
    for (unsigned int i=0; i<_pedestrians.size(); i++)
    {
        delete _pedestrians[i];
    }
    _pedestrians.clear();
    numPedestrian = 0;
}

//======================================================================
void ObjManager::deletePedestrian(Pedestrian* pedestrian)
{
    vector<Pedestrian*>::iterator itp = find(_pedestrians.begin(),
                                             _pedestrians.end(),
                                             pedestrian);
    if (itp!=_pedestrians.end())
    {
    	delete *itp;
        _pedestrians.erase(itp);
    }
}

#endif //INCLUDE_PEDESTRIANS

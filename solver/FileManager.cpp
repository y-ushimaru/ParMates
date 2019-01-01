/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include <cassert>
#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>

using namespace std;

unordered_map<string, ofstream*> FileManager::_ofstreams;

//======================================================================
void FileManager::deleteAllOFStreams()
{
    unordered_map<string, ofstream*>::iterator where;
    for (where=_ofstreams.begin();
         where!=_ofstreams.end();
         where++)
    {
        ofstream* ofs = (*where).second;
        if(ofs->good())
        {
            ofs->close();
        }
        delete ofs;
    }
}

//======================================================================
ofstream& FileManager::getOFStream(const string& filename)
{
    ofstream *result = NULL;
    unordered_map<string, ofstream*>::const_iterator where;

    where = _ofstreams.find(filename);
    if(where != _ofstreams.end())
    {
        // 既にファイルが開かれていたらそれを使う。
        result = (*where).second;
    }
    else
    {
        // まだ開かれていないファイルだったら開く。
        ofstream *ofs = new ofstream(filename.c_str(), ios::out);
        if(!ofs->good())
        {
            cerr << "Failed to open " << filename << endl;
            assert(0);
            exit(-1);
        }
        _ofstreams.insert(make_pair(filename, ofs));
        result = ofs;
    }

    return *result;
}

//======================================================================
void FileManager::closeOFStream(const string& filename)
{
    unordered_map<string, ofstream*>::iterator where;
    where = _ofstreams.find(filename);
    if(where != _ofstreams.end())
    {
        ofstream *ofs = (*where).second;
        ofs->close();
        delete ofs;
        _ofstreams.erase(where);
    }
}

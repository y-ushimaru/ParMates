/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "AmuConverter.h"
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

//======================================================================
string AmuConverter::itos(int num, int size)
{
    string str;
    stringstream strStream("");
    strStream.setf(ios::right, ios::adjustfield);

    //sizeより小さい数の場合は上位の桁を0で埋める
    strStream << setw(size) << setfill('0') << num;
    strStream >> str;
    return str;
}

//======================================================================
double AmuConverter::strtod(const string& src)
{
    double val;
    stringstream str;
    str << src;
    str >> val;
    return val;
}

//======================================================================
unsigned long AmuConverter::strtoul(const string& src)
{
    return ::std::strtoul(src.c_str(), 0, 10);
}

//======================================================================
const string AmuConverter::formatId(const string& src,
                                    const string::size_type size)
{
    // 変換元が10進数値であるかチェック
    for (string::size_type i=0; i<src.size(); i++)
    {
        assert(isdigit(src[i]));
    }

    if (src.size() < size)
    {
        string dst(size - src.size(), '0');
        dst.append(src);
        return dst;
    }
    else if (src.size() == size)
    {
        return src;
    }
    else
    {
        return src.substr(src.size() - size, size);
    }
}

//======================================================================
const string AmuConverter::formatIdExt(const string& src,
                                       const string::size_type size)
{
    if (isdigit(src[0]))
    {
        return formatId(src, size);
    }
    else if (src[0]=='*')
    {
        if (src.size() < size)
        {
            string dst(size - src.size(), '*');
            dst.append(src);
            return dst;
        }
        else if (src.size()==size)
        {
            return src;
        }
        else
        {
            return src.substr(src.size() - size, size);
        }
    }
    else
    {
        cerr << "formatIdExt - invalid input string: " << src << endl;
        assert(0);
    }
}

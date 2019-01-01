/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#include "RelativeDirectionTable.h"
#include <cassert>

using namespace std;

//######################################################################
RelativeDirectionTable::~RelativeDirectionTable(){}

//######################################################################
RelativeDirection RelativeDirectionTableTerminal::operator()(int i, int j) const
{
    if (i==0 && j==0)
    {
        return RD_BACK;
    }
    else
    {
        assert(false);
        return RD_NONE;
    }
}

//######################################################################
RelativeDirection RelativeDirectionTable2Way::operator()(int i, int j) const
{
    if (0<=i && i<2 && 0<=j && j<2)
    {
        if (i==j)
        {
            return RD_BACK;
        }
        else
        {
            return RD_STRAIGHT;
        }
    }
    else
    {
        assert(false);
        return RD_NONE;
    }
}

//######################################################################
const RelativeDirection RelativeDirectionTableTJunction::_table[3][3] =
{
    {RD_BACK    ,RD_RIGHT   ,RD_STRAIGHT},
    {RD_LEFT    ,RD_BACK    ,RD_RIGHT},
    {RD_STRAIGHT,RD_LEFT    ,RD_BACK}
};

RelativeDirectionTableTJunction::RelativeDirectionTableTJunction(int start)
    : _start(start)
{
    assert(0<=start && start<3);
}

RelativeDirection RelativeDirectionTableTJunction::operator()(int i, int j) const
{
    assert(0<=i && i<3 && 0<=j && j<3);
    i = (i + 3 - _start) % 3;
    j = (j + 3 - _start) % 3;
    return _table[i][j];
}

//######################################################################
RelativeDirection RelativeDirectionTableCross::operator()(int i, int j) const
{
    assert(0<=i && i<4 && 0<=j && j<4);  
    switch ((j+4-i)%4)
    {
    case 0:
        return RD_BACK;
    case 1:
        return RD_RIGHT;
    case 2:
        return RD_STRAIGHT;
    case 3:
        return RD_LEFT;
    default:
        assert(false);
        return RD_NONE;
    }
}

//######################################################################
RelativeDirectionTableCustom::RelativeDirectionTableCustom(int size) : _size(size)
{
    _table.resize(size*size, RD_NONE);
}

RelativeDirection RelativeDirectionTableCustom::operator()(int i, int j) const
{
    assert(0<=i && i<_size && 0<=j && j<_size);    
    return (_table[i*_size+j]);
}

void RelativeDirectionTableCustom::setItem(int i, int j, RelativeDirection rd)
{
    assert(0<=i && i<_size && 0<=j && j<_size);    
    _table[i*_size+j] = rd;
}

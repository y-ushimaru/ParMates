/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef SHMEM_H
#define SHMEM_H

#include <qsharedmemory.h>

//共有メモリ名
#define SH_MEM_NAME      "MATES_EDITOR"

//共有メモリデータ、IDが負ならなし
typedef struct {
    int         intsecId1;
    int         intsecId2;
} ShMemData;

//共有メモリ
class ShMem
{
    //共有メモリ
    QSharedMemory _shMem;

public:
    ShMem(): _shMem(SH_MEM_NAME) {}

    //共有メモリアクセス、IDが負ならなし
    void set(int intsecId1, int intsecId2 = -1);
    void get(int* intsecId1, int* intsecId2 = NULL);
};

#endif // SH_MEM_H



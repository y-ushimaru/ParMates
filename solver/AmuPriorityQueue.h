/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __AMU_PRIORITY_QUEUE_H__
#define __AMU_PRIORITY_QUEUE_H__

#include <vector>
#include <algorithm>
#include <cmath>

/// 簡易優先キューを表すクラス
/**
 * バイナリヒープによる実装
 *
 * 要素の追加と根（優先度の最も高い要素）の取り出し，
 * 木の上下方向に向かった並べ替えをサポートしている．
 *
 * @ingroup utility
 */
template <typename T>
class AmuPriorityQueue
{
public:
    AmuPriorityQueue(bool (*func)(T, T))
        : _isPrior(func){};
    ~AmuPriorityQueue(){};

    /// キューをクリアする
    void clear()
    {
        _queue.clear();
    };

    /// 要素を挿入する
    void insert(T t)
    {
        _queue.push_back(t);

        // 下から再構成
        makeHeapUp(_queue.size()-1);
    };

    /// 根（最優先の要素）を取り出す
    T extractRoot()
    {
        T result = _queue[0];
        
        // 最後の要素を根にコピーし，オリジナルは消す
        _queue[0] = _queue[_queue.size()-1];
        _queue.pop_back();
        
        // 上から再構成
        makeHeapDown(0);
   
        return result;
    };

    /// @p n番目の要素を返す
    T at(int n) const
    {
        return _queue[n];
    };

    /// @p n番目の要素から順に上へヒーププロパティを整える
    void makeHeapUp(int n)
    {
        for (int i=n; i>0;)
        {
            int parent = floor((i-1)/2);
            if ((*_isPrior)(_queue[i], _queue[parent]))
            {
                // 下位の要素の優先度が高い場合はスワップ
                std::swap(_queue[parent], _queue[i]);
            }
            i = parent;
        }
    };

    /// @p n番目の要素から順に下へヒーププロパティを整える
    void makeHeapDown(int n)
    {
        for (unsigned int i=n; i<_queue.size();)
        {
            unsigned int childLeft = i*2+1;
            unsigned int childRight = i*2+2;
            
            if (childLeft>=_queue.size())
            {
                return;
            }
            
            unsigned int childPrior;
            if (childRight<_queue.size()
                && (*_isPrior)(_queue[childRight], _queue[childLeft]))
            {
                childPrior = childRight;
            }
            else
            {
                childPrior = childLeft;
            }
            
            if ((*_isPrior)(_queue[childPrior], _queue[i]))
            {
                // 下位の要素の優先度が高い場合はスワップ
                std::swap(_queue[childPrior], _queue[i]);
            }
            i = childPrior;
        }
    };

    /// キューが空かどうか
    bool empty() const
    {
        return _queue.empty();
    };

    /// サイズを返す
    unsigned int size() const
    {
        return _queue.size();
    };

private:
    /// キュー本体
    /**
     * 最大要素数を定めれば配列による実装は容易
     */
    std::vector<T> _queue;

    /// 優先度を比較するための関数ポインタ
    bool (*_isPrior)(T, T);
};

#endif //__AMU_PRIORITY_QUEUE_H__

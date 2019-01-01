/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __PF_LOGGER_H__
#define __PF_LOGGER_H__

#include <map>
#include <string>
#include <vector>

class RoutingNode;
struct PFSearchNodeBase;

/// 経路探索器による探索のプロセスを記録するクラス
class PFLogger
{
public:
    PFLogger();
    ~PFLogger();

    /// SearchNodeのIDとRoutingNodeを紐付ける
    void addSnodeId2RoutingNode(const std::string& snodeId,
                                const RoutingNode* rnode);

    /// SearchNodeのIDとRoutingNodeの変換テーブルを解除する
    void clearSnodeId2RoutingNode();

    /// ログがとられている状態か
    bool isEmpty() const;

    /// ログのサイズを返す
    unsigned int logSize() const;
    
    /// ログをリセットする
    void reset();

    /// ログを取る
    void addLog(PFSearchNodeBase* snode);

    /// ログを返す
    const std::vector<PFSearchNodeBase*>* log() const;

    /// SearchNodeのIDとRoutingNodeの変換テーブルを返す
    const std::map<const std::string,
                   const RoutingNode*>* snodeId2RoutingNode() const;

    /// ログを標準出力に表示する
    void printLog() const;
    
private:
    /// SearchNodeのIDとRoutingNodeの変換テーブル
    std::map<const std::string,
             const RoutingNode*> _snodeId2RoutingNode;
    
    /// 探索過程を記録するコンテナ
    std::vector<PFSearchNodeBase*> _log;
};

#endif //__PF_LOGGER_H__

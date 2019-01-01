/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __OD_H__
#define __OD_H__

#include <string>
#include <vector>

/// OD(起点Origin-終点Destination)と経路情報を格納するクラス
/**
 * 情報は全てstring型の交差点IDで持つ．Intersectionのポインタではない．
 *
 * @ingroup Routing
 */
class OD
{
public:
    OD();
    ~OD();

    /// 起点，終点を設定する．
    /**
     * 経由地を指定しない場合に用いる
     */
    bool setValue(const std::string &start, const std::string &goal);
  
    /// start=origin, goal=destination, stopPoints=経由する交差点を設定する
    bool setValue(const std::string &start, const std::string &goal,
                  const std::vector<std::string>& stopPoints);

    /// 最後に通過した経由地を指定する。
    /**
     * 最後に通過した経由地を指定する。
     * 指定された交差点が経由地リストになければ何もしない。
     */
    void setLastPassedStopPoint(const std::string passedInter);

    /// 出発地の交差点の識別番号を返す
    const std::string& start() const;

    /// 目的地の交差点の識別番号を返す
    const std::string& goal() const;

    /// 経由地の交差点の識別番号を返す
    const std::vector<std::string>* stopPoints() const;

    /// 最後に通過した経由地のindexを返す
    int lastPassedStopPoint() const;

    /// 初期化する
    void clear();

protected:
    std::string _start; //< 出発地
    std::string _goal;  //< 目的地
    std::vector<std::string> _stopPoints; //< 経由地
    int _lastPassedStopPoint; //< 最後に通過した経由地のindex
};

#endif //__OD_H__

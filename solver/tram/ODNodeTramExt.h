/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __OD_NODE_TRAM_EXT_H__
#define __OD_NODE_TRAM_EXT_H__

#include <vector>
#include <deque>

class RoadMap;
class ODNode;
class Vehicle;

/// ODノードの路面電車機能拡張クラス
/**
 * @ingroup Tram
 */
class ODNodeTramExt
{
public:
    ODNodeTramExt(){};
    ODNodeTramExt(ODNode* odNode);
    ~ODNodeTramExt();

    /// 路面電車を発生させる
    void putTram(RoadMap* roadMap,
                 std::deque<Vehicle*>* waitingVehicles,
                 std::vector<Vehicle*>* skipVehicles);

private:
    /// 対応するODノード
    ODNode* _odNode;

};

#endif //__OD_NODE_TRAM_EXT_H__

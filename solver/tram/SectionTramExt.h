/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __SECTION_TRAM_EXT_H__
#define __SECTION_TRAM_EXT_H__

#include <vector>

class Section;
class Intersection;
class Lane;

/// 単路の路面電車機能拡張クラス
/**
 * @ingroup Tram
 */
class SectionTramExt
{
public:
    SectionTramExt(){};
    SectionTramExt(Section* section);
    ~SectionTramExt(){};

    /// 交差点 @p inter から単路に流入する路面電車レーンを返す
    void getTramLanesFrom(std::vector<Lane*>* result_lanes,
                          const Intersection* inter);

    /// 単路から交差点 @p inter に流出する路面電車レーンを返す
    void getTramLanesTo(std::vector<Lane*>* result_lanes,
                        const Intersection* inter);

    /// 単路内の路面電車レーンを生成する
    bool createTramLanesInSection();

private:
    /// 対応する単路
    Section* _section;
};

#endif //__INTERSECTION_TRAM_EXT_H__

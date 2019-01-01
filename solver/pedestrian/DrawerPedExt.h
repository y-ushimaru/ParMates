/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifndef __DRAWER_PED_EXT_H__
#define __DRAWER_PED_EXT_H__

class Pedestrian;

/// 歩行者を描画するクラス
/**
 * @ingroup Drawing
 */
class PedestrianDrawer
{
public:
    static PedestrianDrawer& instance ();
    virtual void draw (const Pedestrian& pds) const;
    // virtual void drawSimple (const Pedestrian& pds, double size) const;

protected:
    PedestrianDrawer(){};
    PedestrianDrawer(const PedestrianDrawer&){};
    virtual ~PedestrianDrawer(){};
};

#endif //__DRAWER_PED_EXT_H__

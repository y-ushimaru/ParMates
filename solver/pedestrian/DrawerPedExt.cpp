/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "DrawerPedExt.h"
#include "Pedestrian.h"
#include "../GVManager.h"
#include "../GLColor.h"
#include <autogl.h>
#include <string>

using namespace std;

//######################################################################
PedestrianDrawer& PedestrianDrawer::instance ()
{
    static PedestrianDrawer instance;
    return instance;
}
//----------------------------------------------------------------------
void PedestrianDrawer::draw (const Pedestrian& pds) const
{
    double x = pds.x();
    double y = pds.y();
    double z = pds.z();
    double radius = pds.radius ();
    double zoffset = 0.1;
    //double psRadius = pds.psRadius();

    const AmuVector* velocity;
    double vx, vy;
    double vzoffset;

    velocity = pds.velocity();
    vx = velocity->x();
    vy = velocity->y();
    //vz = velocity->z();
    vzoffset = zoffset*2.0;

    if (radius < 0.1)
    {
    	radius = 0.1;
    }

    // position -> circle
    GLColor::setPedestrian();
    AutoGL_DrawCircle3D(x, y, z + zoffset, 0, 0, 1, radius, 8);

    // velocity -> arrow
    if (pds.recognitionStatus() == RecogStatus_NoChange)
    {
        GLColor::setPedestrianArrow();
    }
    else
    {
        GLColor::setShiftedPedestrianArrow();
    }
    AutoGL_DrawLine (x, y, z + vzoffset, x+vx, y+vy, z+vzoffset);

    if (GVManager::getFlag("VIS_PEDESTRIAN_ID"))
    {
        string id;
        id = pds.id();
        if (id != "")
        {
            GLColor::setPedestrianId();
            AutoGL_DrawString(x, y, z+5, id.c_str());
        }
    }
}

#endif //INCLUDE_PEDESTRIANS

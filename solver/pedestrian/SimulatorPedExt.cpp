/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "SimulatorPedExt.h"
#include "GeneratePedestrianController.h"
#include "../Simulator.h"
#include <cassert>

using namespace std;

//======================================================================
SimulatorPedExt::SimulatorPedExt(Simulator* simulator)
{
    _simulator = simulator;
    _genPedestrianController
        = &GeneratePedestrianController::instance();
}

//======================================================================
bool SimulatorPedExt::getReadyPedestrians()
{
    assert(_simulator->_roadMap);

    _genPedestrianController->setRoadMap(_simulator->_roadMap);
    _genPedestrianController->getReadyGeneration();
    return true;
}

//======================================================================
void SimulatorPedExt::generatePedestrian()
{
    _genPedestrianController->generatePedestrian();
}

#endif //INCLUDE_PEDESTRIANS

/* **************************************************
 * Copyright (C) 2014 ADVENTURE Project
 * All Rights Reserved
 **************************************************** */
#ifdef INCLUDE_PEDESTRIANS

#include "GeneratePedestrianController.h"
#include "IntersectionPedExt.h"
#include "Zebra.h"
#include "Pedestrian.h"
#include "GeneratePedestrianIO.h"
#include "../RoadMap.h"
#include "../Intersection.h"
#include "../ObjManager.h"
#include "../GVManager.h"
#include "../Random.h"

// #define PDS_DEBUG

using namespace std;

//======================================================================
GeneratePedestrianController::GeneratePedestrianController()
{
    _rnd = Random::randomGenerator();
}

//======================================================================
GeneratePedestrianController::~GeneratePedestrianController()
{
    Random::releaseRandomGenerator(_rnd);
}

//======================================================================
GeneratePedestrianController& GeneratePedestrianController::instance()
{
    static GeneratePedestrianController instance;
    return instance;
}

//======================================================================
void GeneratePedestrianController::setRoadMap(RoadMap* roadMap)
{
    _roadMap = roadMap;
}

//======================================================================
bool GeneratePedestrianController::getReadyGeneration()
{
    // TODO : implement me!
    /*string fGeneratePedestrian;
      GVManager::getVariable("GENERATE_PEDESTRIAN",
      &fGeneratePedestrian);
      fstream fin;
      vector<string> tokens;
      string str;*/

    GeneratePedestrianIO* genPedIO
        = &(GeneratePedestrianIO::instance());
    genPedIO->setRoadMap(_roadMap);
    genPedIO->readPedGenVolume();

    return true;
}

//======================================================================
void GeneratePedestrianController::setOutputGeneratePedestrianDataFlag
(bool flag)
{
    _isOutputGeneratePedestrianData = flag;
}

//======================================================================
bool GeneratePedestrianController::canGeneratePedestrian(Zebra* zebra,
                                                         int dir)
{
    double rnd = Random::uniform(_rnd);
    if (dir==0)
    {
        return rnd < zebra->pedGenProbability(false);
    }
    else
    {
        return rnd < zebra->pedGenProbability(true);
    }
}

//======================================================================
void GeneratePedestrianController::generatePedestrian()
{
    /*
     * 現段階では歩行者は横断歩道上にしか存在させない．
     * 車両と同様、実際の生成はObjManager::createPedestrian
     * によって行われる．
     */

    //交差点群
    const RMAPI* intersections = _roadMap->intersections();
    for (CITRMAPI iti = intersections->begin();
         iti != intersections->end();
         iti++)
    {
        Intersection* intersection = iti->second;

        /*
         * intersection->zebras()で交差点のみ処理するので、
         * 端点は無視できる
         */
        const RMAPZBR* zebras = intersection->pedExt()->zebras();
        if (!zebras)
        {
            continue;
        }

        for (CITRMAPZBR ite = zebras->begin();
             ite != zebras->end();
             ite++)
        {
            Zebra* zebra = const_cast<Zebra*>(ite->second);
            if (zebra == NULL)
            {
                continue;
            }

            /// 発生歩行者データ
            std::vector<GeneratedPedestrianData> _nodeGvd;

            // 交差点内での横断歩道のID
            int intersectionDir = intersection->direction(zebra);

            // 交差点の信号群
            Signal* signal = (*iti).second->signal();

            // 歩行者信号の状態
            SignalWalkerState state
                = signal->walkerColor(intersectionDir);

#ifdef _OPENMP
#pragma omp critical (addWatchedPedestrian)
            {
#endif //_OPENMP

                //歩行者の発生
                for (int dir = 0; dir < 2; dir++)
                {
                    if (canGeneratePedestrian(zebra, dir))
                    {
                        // 生成
                        Pedestrian* candPds
                            = ObjManager::createPedestrian();
                        candPds->setDirection(dir);

                        // _waitingPdsに入れる
                        zebra->putWaitingPds(candPds, dir);
#ifdef PDS_DEBUG
                        cerr << "z[" << zebra->id()
                             << "][" << dir << "] put new pds$"
                             << candPds << endl;
#endif
                    }

                    if (state == SignalColor::blue())
                    {
                        Pedestrian* tmpPds
                            = zebra->popPdsFromWaiting(dir);

#ifdef PDS_DEBUG
                        cerr << "z[" << zebra->id()
                             << "][" << dir << "] pop new pds$"
                             << tmpPds << endl;
#endif
                        // シミュレータ上に登場させる
                        if (tmpPds != NULL)
                        {
#if 0
                            // get abs. start position on zebra.
                            double baselinePosition = Random::uniform();
                            AmuPoint initialPosition = zebra->initialPositionOf (dir, baselinePosition);
                            tmpPds->setPoint(&initialPosition);
                            // get direction vector
                            double speedRatio = Random::uniform();
                            AmuVector initialVelocity = zebra->initialVelocityOf (dir, speedRatio);
                            tmpPds->setVelocity(initialVelocity);
#else
                            // start position??
                                                  
                            // get abs. start position on zebra.
                            // get direction vector
                            double speedRatio
                                = Random::uniform(_rnd);
                            AmuVector initialVelocity
                                = zebra->initialVelocityOf
                                (dir, speedRatio);
                            tmpPds->setVelocity(initialVelocity);

                            double baselinePosition
                                = Random::uniform(_rnd);
                            AmuPoint initialPosition
                                = zebra->initialPositionOf
                                (dir, baselinePosition);

                            initialPosition += initialVelocity * 0.001;
                            tmpPds->setPoint(&initialPosition);
#endif
                                                  
                            tmpPds->setStartTime(TimeManager::time());

                            /* idをObjMangerで付与して横断歩道上に置く。
                             * この時点でObjManager::pedestrians()に入る。
                             */
                            bool result
                                = ObjManager::addPedestrianToReal(tmpPds);
                            assert(result);

                            // @todo sometime error happens...
                            zebra->putPedestrian(tmpPds);

                            // set locations
                            tmpPds->setIntersection (intersection);
                            tmpPds->setZebra (zebra);
                            tmpPds->setOnZebra();

                            if (GVManager::getFlag
                                ("FLAG_OUTPUT_GEN_COUNTER"))
                            {
                                GeneratedPedestrianData gvd;
                                gvd.pedestrian = tmpPds;
                                gvd.intersection = intersection;
                                gvd.zebra = zebra;
                                gvd.startTime = tmpPds->startTime();
                                _nodeGvd.push_back(gvd);
                            }
                        } // if(tmpPds)
                    } // if (state)
                } // for (dir)

                // 横断歩道にすべての歩行者をのせる
                zebra->renewPedestrianLine();
#ifdef _OPENMP
            }
#endif //_OPENMP

            // 歩行者発生情報の出力
            /* !!StaticDataとGenerateDataの混同!!
            if (GVManager::getFlag("FLAG_OUTPUT_GEN_COUNTER")
                && _isOutputGeneratePedestrianData
                && !_nodeGvd.empty())
            {
                PedestrianIO::instance()
                    .writePedestrianStaticData(zebra, &_nodeGvd);
                _nodeGvd.clear();
            }
            */

        } // while(ite)

        //iti++;
    } // while(iti)
}

#endif //INCLUDE_PEDESTRIANS

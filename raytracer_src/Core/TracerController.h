/*
 * Copyright 2015 VTT Technical Research Center of Finland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRACERCONTROLLER_H_
#define TRACERCONTROLLER_H_

#include "Common.h"
#include "AbstractGeometry.h"
#include "Source.h"
#include "Ray.h"
#include "Surface.h"
#include "UtilityFunctions.h"
#include "LostRaySaver.h"
#include <vector>
#include <fstream>
#include <string>
#include "json.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

using namespace std;

struct RayPathData {
    vector<double> pathpoints;
    vector<double> colors;
    vector<long long> offsets;
    long long pointNumber;
    double lastWavelength;
    long long rayNumber;
};

class TracerController {
  public:
    TracerController();
    TracerController(Json::Value jsonData);
    virtual ~TracerController();

    /*
     * This adds a source to be traced.
     */
    void addSource(Source* newSource);

    /**
     * Will start tracing from sources one by one.
     */
    void startTracing();

    /*
     * Maximum number of raytracing iterations before moving
     * on to next ray.
     */
    double maxIterations;
  private:

    void _traceRay(Source* s);

    vector<Source*> _sources;
    void _saveRayPathPoint(Ray& ray, RayPathData& data);
    void _saveNewRayPath(Ray& ray, RayPathData& data);
    void _saveRayEnd(RayPathData& data);
    void _saveRayPathData(RayPathData& data);
    void _saveVTPRayfile();
    string _rayPathsFilename;
    string _rayTmpFilename;
    string _rayOffsetsFilename;
    string _rayColorFilename;
    ofstream _rayTmpFile;
    ofstream _rayOffsetsFile;
    ofstream _rayColorFile;
    long long _totalPointNumber;
    long long _totalRayNumber;

    atomic<unsigned long long> _maxIterRays;
    unsigned long long _raysLeftToTrace;

    unsigned int maxThreads;

    mutex _rayWriteMutex;
    mutex _powerSavingMutex;
    mutex _rayStartMutex;
    //LostRaySaver* _lostrays;
    // Power lost by terminating rays due to too many iterations
    double _lostPower;
    // Fluorescent power lost by terminating rays due to too many iterations
    double _lostPowerFluor;

};

#endif /* TRACERCONTROLLER_H_ */

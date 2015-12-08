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

#ifndef RAYTRACER_SRC_DETECTORS_ZEMAXRAYFILEDETECTOR_H_
#define RAYTRACER_SRC_DETECTORS_ZEMAXRAYFILEDETECTOR_H_

#include <fstream>
#include <string>
#include "SurfaceProperty.h"
#include "Ray.h"
#include "json.h"
#include <mutex>
#include "Surface.h"

using namespace std;

/**
 * Generates a Zemax readable ray source file for the surface
 * this detector is attached.
 */
class ZemaxRayfileDetector : public SurfaceProperty {
 public:
  ZemaxRayfileDetector(Json::Value jsonData);
  virtual ~ZemaxRayfileDetector();
  AbstractGeometry* receiveRay(Ray& ray, Surface* surface,
                               AbstractGeometry* from, AbstractGeometry* to);
  virtual void tracerDidEndTracing();
 private:
  ofstream _file;
  static unsigned int _count;
  unsigned int _rayNum;
  mutex _savingMutex;

  string _rayFilename;
  string _rayTmpFilename;
  ofstream _rayTmpFile;
  bool _stopRays;
  int _includeOnlyNormalDir;

  float _cumulativeRadiantPower;

};

#endif /* RAYTRACER_SRC_DETECTORS_ZEMAXRAYFILEDETECTOR_H_ */

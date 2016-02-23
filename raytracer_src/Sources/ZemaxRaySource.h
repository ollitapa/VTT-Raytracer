/*
 * Copyright 2016 VTT Technical Research Center of Finland
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

#ifndef ZEMAXRAYSOURCE_H_
#define ZEMAXRAYSOURCE_H_

#include "Source.h"
#include "Solid.h"
#include "Ray.h"
#include "json.h"
#include "Common.h"
#include <fstream>
#include <mutex>

#define MAX_ZEMAX_RAY_BUFFER_SIZE 100000
//#define MAX_ZEMAX_RAY_BUFFER_SIZE 10

struct RayData {
  float flux;
  float wavelength;
  float x;
  float y;
  float z;
  float i;// Direction x
  float j;// Direction y
  float k;// Direction z
};

class ZemaxRaySource : public Source {
 public:
  ZemaxRaySource(Json::Value jsonData, Solid* theEnclosingObject);

  /*
   * Starting object of the ray.
   */
  AbstractGeometry* startingObject();
  /**
   * This method will generate rays from this source.
   * The rays are pointed random direction and
   * distributed evenly.
   *
   */
  virtual Ray generateRay();

 private:
  /*
   * Buffer for rays. Refilled when empty.
   */
  vector<RayData> _rayBuffer;

  /**
   * Mutex for loading rays
   */
  mutex _loadMutex;

  /**
   * Flux of the combined set of rays. Header info.
   */
  float _sourceFlux;

  /**
   *  Total flux of the rays combined. Updated everytime a ray is loaded.
   *
   */
  double _fluxSum;
  /**
   * Fill the raybuffer from file.
   */
  void _fillRayBuffer();

  /**
   * Filestream of the ray file.
   */
  ifstream f;


  unsigned int _rayNumber;

  /**
   * Number whether a spectral ray data should be used.
   */
  bool _spectral;

  /**
   * Conversion coefficient to meters.
   */
  double _unit;


};

#endif /* ZEMAXRAYSOURCE_H_ */

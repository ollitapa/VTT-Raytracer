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

#ifndef ALLDETECTOR_H_
#define ALLDETECTOR_H_

#include <fstream>
#include <string>
#include "SurfaceProperty.h"
#include "Ray.h"
#include "json.h"
#include <mutex>

using namespace std;

class AllDetector: public SurfaceProperty {
  public:
    AllDetector();
    AllDetector(Json::Value jsonData);
    virtual ~AllDetector();
    AbstractGeometry* receiveRay(Ray& ray, Surface* surface,
                                 AbstractGeometry* from,
                                 AbstractGeometry* to);
    virtual void tracerDidEndTracing();
  private:
    ofstream _file;
    static int count;
    mutex _savingMutex;

};

#endif /* ALLDETECTOR_H_ */

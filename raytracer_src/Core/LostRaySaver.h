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

#ifndef RAYTRACER_SRC_CORE_LOSTRAYSAVER_H_
#define RAYTRACER_SRC_CORE_LOSTRAYSAVER_H_

#include <fstream>
#include <string>
#include "Ray.h"
#include <mutex>

using namespace std;

class LostRaySaver {
  LostRaySaver();
  virtual ~LostRaySaver();
  static LostRaySaver* instance;
 public:

  static LostRaySaver* getInstance() {
    if (instance == NULL) {
      instance = new LostRaySaver();
    }

    return instance;
  }

  void saveRay(Ray& ray);
  void endTracing();

 private:
  ofstream _file;
  unsigned int _count;
  mutex _savingMutex;
};

#endif /* RAYTRACER_SRC_CORE_LOSTRAYSAVER_H_ */

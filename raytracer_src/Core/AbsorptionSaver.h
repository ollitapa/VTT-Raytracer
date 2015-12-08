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

#ifndef __MMP_Raytracer__AbsorptionSaver__
#define __MMP_Raytracer__AbsorptionSaver__

#include "SimpleFunctions.h"
#include "Common.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <armadillo>
#include <stdint.h>
#include <mutex>

using namespace std;

class AbsorptionSaver {
  static AbsorptionSaver* instance;
  AbsorptionSaver();
  void initAbsorptionFile();
 public:
  static AbsorptionSaver* getInstance() {
    if (instance == NULL) {
      instance = new AbsorptionSaver();
    }

    return instance;
  }

  void saveAbsorptionData(vertex& place, double data);
  void saveVTPAbsorptionfile();
  static void tracerDidEndTracing();
  string absorptionFilename;
 private:
  uint64_t _absorptionNumber;
  string _tmpFilenamePoints;
  string _tmpFilenameData;
  ofstream _absorptionFile;
  ofstream _pointsFile;
  bool _saved;
  mutex _savingMutex;
};
#endif /* defined(__MMP_Raytracer__AbsorptionSaver__) */

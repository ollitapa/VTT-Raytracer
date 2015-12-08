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

#ifndef ABSORPTION_H_
#define ABSORPTION_H_

#include "SurfaceProperty.h"
#include "SimpleFunctions.h"
#include "json.h"
#include <fstream>
#include <string>
#include <mutex>

#define ABSORPTION_BUFFER_SIZE 1000000

class Absorption : public SurfaceProperty {
 public:
  Absorption();
  Absorption(Json::Value jsonData);
  virtual ~Absorption();
  AbstractGeometry* receiveRay(Ray& ray, Surface* surface,
                               AbstractGeometry* from, AbstractGeometry* to);
  double absorptionCoefficient;

  static void _initAbsorptionFile();
  static void _saveAbsorptionData(vertex& place, double data);

  static string _tmpFilenamePoints;
  static string _tmpFilenameData;
  static string _absorptionFilename;
  static ofstream _absorptionFile;
  static ofstream _pointsFile;
  static bool _saveAbsorption;
  static bool _configured;
  static void _saveTempAbsorptionFile(int size);
  static void _saveVTPAbsorptionfile();
  static unsigned long long _absorptionNumber;

  /**
   * Tracer calls this method when the tracing has ended.
   */
  virtual void tracerDidEndTracing();

 private:
  static mutex _savingMutex;
  static vector<double> _points;
  static vector<double> _absorption;
};

#endif /* ABSORPTION_H_ */

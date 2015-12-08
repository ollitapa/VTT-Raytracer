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

#ifndef IRRADIANCEDETECTOR_H_
#define IRRADIANCEDETECTOR_H_

#include <fstream>
#include <string>
#include "SurfaceProperty.h"
#include "Ray.h"
#include <omp.h>
#include <tbb/concurrent_vector.h>
#include "Surface.h"

using namespace std;
using namespace tbb;

class IrradianceDetector : public SurfaceProperty {

 public:

//		IrradianceDetector();
  IrradianceDetector(Surface* detectorObject, int XPixels, int YPixels,
                     int N_RAYS);
  virtual ~IrradianceDetector();
  AbstractGeometry* receiveRay(Ray& ray, Surface* surface,
                               AbstractGeometry* from, AbstractGeometry* to);

  void RecordRayData(Ray& ray);
 public:

  void SaveRays(string& RayFileName);

  void Irradiance(string& DetectorDataFileName, int NUM_THREADS);

  void ConstructIrradianceData(int NUM_THREADS);

  void IrradianceCrossSectionVertical(int ColNumber,
                                      string& CrossSectionDataFileName);
  void IrradianceCrossSectionHorizontal(int RowNumber,
                                        string& CrossSectionDataFileName);

  void Spectrum(string& SpectrumDataFileName);

 public:
  // X and Y pixel number
  int XPix;
  int YPix;

  // step-variables
  double XSTEP;
  double YSTEP;

  double RS_width;
  double RS_height;

  // Container for the detected rays
  concurrent_vector<Ray> DetectedRays;

  // Total detected flux
  double TotalFlux;

  mat FluxData;

  // Number of Rays propagated
  int TotalNumberOfRays;

  vec pos;
  vec uvec;
  vec vvec;

 private:

  static int count;

};

#endif /* IRRADIANCEDETECTOR_H_ */

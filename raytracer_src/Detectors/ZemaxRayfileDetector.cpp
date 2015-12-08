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

#include "ZemaxRayfileDetector.h"
// Init count
unsigned int ZemaxRayfileDetector::_count = 0;

ZemaxRayfileDetector::ZemaxRayfileDetector(Json::Value jsonData) {
  _count++;
  checkRequiredJSONParam(jsonData, "name", this);
  checkRequiredJSONParam(jsonData, "stopRays", this);
  checkRequiredJSONParam(jsonData, "includeOnlyNormalDirection", this);
  // Must cooperate with refraction
  _cooperation_level = COOPERATION_LEVEL_REFRACTION;
  _hierarchy = HIERARCHY_REFRACTION;
  stringstream ss;
  ss << _count;
  // Filename for detector
  _rayTmpFilename = "ZemaxRays" + ss.str() + "_" + jsonData["name"].asString()
      + ".tmp";
  _rayFilename = "ZemaxRays" + ss.str() + "_" + jsonData["name"].asString()
      + ".dat";
  _rayTmpFile.open(_rayTmpFilename.c_str(), ios::out | ios::binary);
  _stopRays = jsonData["stopRays"].asBool();
  _rayNum = 0;
  _cumulativeRadiantPower = 0;
  _includeOnlyNormalDir = jsonData["includeOnlyNormalDirection"].asInt();
}

ZemaxRayfileDetector::~ZemaxRayfileDetector() {
}

AbstractGeometry* ZemaxRayfileDetector::receiveRay(Ray& ray, Surface* surface,
                                                   AbstractGeometry* from,
                                                   AbstractGeometry* to) {
  // Check if ray is to be included.
  // _includeOnlyNormalDir < 0 : include rays that are going against
  //                              surface normal
  // _includeOnlyNormalDir > 0 : include rays that are going towards
  //                              surface normal
  // _includeOnlyNormalDir = 0  : include all rays
  double n = arma::dot(surface->normal(ray.location), ray.direction);

  if (_includeOnlyNormalDir != 0) {
    if (signum(n) != signum(_includeOnlyNormalDir))
      return (to);
  }

  _savingMutex.lock();
  // Save ray to file
  /**
   * typedef struct
   * {
   *     float x, y, z;
   *     float l, m, n;
   *     float flux, wavelength;
   * } SPECTRAL_COLOR;
   */
  float loc1 = ray.location[0];
  float loc2 = ray.location[1];
  float loc3 = ray.location[2];
  _rayTmpFile.write((char*) &(loc1), 4);
  _rayTmpFile.write((char*) &(loc2), 4);
  _rayTmpFile.write((char*) &(loc3), 4);
  float dir1 = ray.direction[0];
  float dir2 = ray.direction[1];
  float dir3 = ray.direction[2];
  _rayTmpFile.write((char*) &(dir1), 4);
  _rayTmpFile.write((char*) &(dir2), 4);
  _rayTmpFile.write((char*) &(dir3), 4);
  float power = ray.radiantPower();
  _rayTmpFile.write((char*) &(power), 4);
  float wavelen = ray.wavelength_0 / 1000.0;
  _rayTmpFile.write((char*) &(wavelen), 4);
  // Add ray number and power
  _rayNum++;
  _cumulativeRadiantPower += power;

  // Stop ray if applicable
  if (_stopRays)
    ray.flux = 0.0;

  _savingMutex.unlock();
  return (to);
}

void ZemaxRayfileDetector::tracerDidEndTracing() {
  _rayTmpFile.close();
  // Append data from tmp-file
  ifstream ifile(_rayTmpFilename.c_str(), std::ios::in | ios::binary);

  if (!ifile.is_open())
    return;

  ofstream f;
  f.open(_rayFilename.c_str(), ios::out | ios::binary);
  // Write header
  /**
   * typedef struct
   * {
   *      int Identifier; // Format version ID, current value is 1010
   *      unsigned int NbrRays; // The number of rays in the file
   *      char Description[100]; // A text description of the source
   *      float SourceFlux; // The total flux in watts of this source
   *      float RaySetFlux; // The flux in watts represented by this Ray Set
   *      float Wavelength; // The wavelength in micrometers, 0 if a composite
   *      float InclinationBeg, InclinationEnd; // Angular range for ray set (Degrees)
   *      float AzimuthBeg, AzimuthEnd; // Angular range for ray set (Degrees)
   *      long DimensionUnits; // METERS=0, IN=1, CM=2, FEET=3, MM=4
   *      float LocX, LocY,LocZ; // Coordinate Translation of the source
   *      float RotX,RotY,RotZ; // Source rotation (Radians)
   *      float ScaleX, ScaleY, ScaleZ; // Currently unused
   *      float unused1, unused2, unused3, unused4;
   *      int ray_format_type, flux_type;
   *      int reserved1, reserved2;
   * } NSC_RAY_DATA_HEADER;
   */
  // TODO: Fix zemax format
  int ident = 8675309;  // Couldnt figure out why this is this number 1010...
  // but HEY, it works...
  f.write((char*) &(ident), 4);  // Format version ID, current value is 1010
  f.write((char*) &(_rayNum), 4);  // The number of rays in the file
  char s[100] = "Zemax Ray file created by MMP Raytracer";
  f.write((char*) &(s), 100);  // A text description of the source
  f.write((char*) &(_cumulativeRadiantPower), 4);  // The total flux in watts of this source
  f.write((char*) &(_cumulativeRadiantPower), 4);  // The flux in watts represented by this Ray Set
  float w = 0;
  f.write((char*) &(w), 4);  // The wavelength in micrometers, 0 if a composite
  float a = 0;
  float aa = 180;
  f.write((char*) &(a), 4);  // Angular range for ray set (Degrees)
  f.write((char*) &(aa), 4);  // Angular range for ray set (Degrees)
  float b = 0;
  float bb = 360;
  f.write((char*) &(b), 4);  // Angular range for ray set (Degrees)
  f.write((char*) &(bb), 4);  // Angular range for ray set (Degrees)
  long unit = 0;
  f.write((char*) &(unit), 4);  // METERS=0, IN=1, CM=2, FEET=3, MM=4
  float c = 0;
  float cc = 0;
  float ccc = 0;
  f.write((char*) &(c), 4);  // Coordinate Translation of the source
  f.write((char*) &(cc), 4);  // Coordinate Translation of the source
  f.write((char*) &(ccc), 4);  // Coordinate Translation of the source
  f.write((char*) &(c), 4);  // Source rotation (Radians)
  f.write((char*) &(cc), 4);  // Source rotation (Radians)
  f.write((char*) &(ccc), 4);  // Source rotation (Radians)
  float scale = 1;
  f.write((char*) &(scale), 4);  // Currently unused ScaleX
  f.write((char*) &(scale), 4);  // Currently unused ScaleY
  f.write((char*) &(scale), 4);  // Currently unused ScaleZ
  f.write((char*) &(c), 4);  // Currently unused1
  f.write((char*) &(c), 4);  // Currently unused2
  f.write((char*) &(c), 4);  // Currently unused3
  f.write((char*) &(c), 4);  // Currently unused4
  int r_form = 2;
  f.write((char*) &(r_form), 4);  // ray_format_type // Spectral=2
  int f_type = 0;
  f.write((char*) &(f_type), 4);  // flux_type // Watts=0
  int reserved1 = 28;
  int reserved2 = 0;
  f.write((char*) &(reserved1), 4);  // Reserved1
  f.write((char*) &(reserved2), 4);  // Reserved2

  //check to see that it exists:
  if (!ifile.is_open()) {
    cout << "file not found for Zemax ray temp" << endl;

  } else {
    f << ifile.rdbuf();
  }

  ifile.close();
  // Close file
  f.close();
  // Remove temp-files
  remove(_rayTmpFilename.c_str());
}

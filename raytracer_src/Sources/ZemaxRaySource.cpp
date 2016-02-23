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

#include "ZemaxRaySource.h"

ZemaxRaySource::ZemaxRaySource(Json::Value jsonData, Solid* theEnclosingObject)
    : Source(theEnclosingObject, 1) {

  checkRequiredJSONParam(jsonData, "filepath", this);
  checkRequiredJSONParam(jsonData, "wavelengths", this);
  checkRequiredJSONParam(jsonData, "intensities", this);
  checkRequiredJSONParam(jsonData, "rays", this);

  // Number of rays, does not have effect
  cout << "Number of rays does not have any effect!" << endl;


  string fname = jsonData["filepath"].asString();

  cout << "Loading first batch of rays..." << endl;

  // Reserve buffer
  _rayBuffer.reserve(MAX_ZEMAX_RAY_BUFFER_SIZE * sizeof(RayData));

  // Open file
  f.close();
  f.open(fname.c_str(), ios::in | ios::binary);

  _fluxSum = 0;

  // Read header ////////////////////////////////////////////////////
  // Read file identifier

  int a;
  f.read(reinterpret_cast<char *>(&a), 4);
  if (a == 1010)
    cout << "Valid file" << endl;
  else {
    cout << "Invalid file, may not work" << endl;
  }

  int rays = 0;
  // Number of rays
  cout << "Number of rays: ";
  f.read(reinterpret_cast<char *>(&rays), 4);
  cout << rays << endl;
  nRays = rays;

  char* description = 0;
  description = new char[100 + 1];
  f.read(description, 100);
  description[100] = '\0';
  cout << "Description: " << description << endl;

  // Source flux W
  cout << "Source flux W: ";
  float b;
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;
  _sourceFlux = b;

  // Ray set flux W
  cout << "Rayset flux W: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  // Wavelength, 0 if composite
  cout << "Wavelength (0 if composite): ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  // InclinationBeg, InclinationEnd; // Angular range for ray set (Degrees)
  cout << "InclinationBeg: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;
  cout << "InclinationEnd: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  cout << "AzimuthBeg: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  cout << "AzimuthEnd: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  // DimensionUnits; // METERS=0, IN=1, CM=2, FEET=3, MM=4
  cout << "DimensionUnits (METERS=0, IN=1, CM=2, FEET=3, MM=4): ";
  //long c;
  f.read(reinterpret_cast<char *>(&a), 4);
  cout << a << endl;

  switch (a) {
    case 0:  // METERS
      _unit = 1.0;
      break;
    case 1:  // IN
      _unit = 0.0254;
      break;
    case 2:  // CM
      _unit = 0.01;
      break;
    case 3:  // FEET
      _unit = 0.3048;
      break;
    case 4:  //MM
      _unit = 0.001;
      break;
    default:
      _unit = 1.0;
      break;
  }

  cout << "SourceTranslation: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;
  cout << "Source rotation: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  cout << "Unused Scale: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  cout << "Unused: ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << ", ";
  f.read(reinterpret_cast<char *>(&b), 4);
  cout << b << endl;

  cout << "ray_format_type: ";
  f.read(reinterpret_cast<char *>(&a), 4);
  cout << a << endl;
  if (a == 2)
    _spectral = true;
  else
    _spectral = false;

  cout << "flux_type: ";
  f.read(reinterpret_cast<char *>(&a), 4);
  cout << a << endl;
  cout << "Reserved: ";
  f.read(reinterpret_cast<char *>(&a), 4);
  cout << a << ", ";
  f.read(reinterpret_cast<char *>(&a), 4);
  cout << a << endl;

  _fillRayBuffer();

}

AbstractGeometry* ZemaxRaySource::startingObject() {
  return (enclosingObject);
}

Ray ZemaxRaySource::generateRay() {
  Ray r;
  RayData rd;
  _initMutex.lock();

  if (!(_rayNumber < MAX_ZEMAX_RAY_BUFFER_SIZE))
    _fillRayBuffer();

  rd = _rayBuffer[_rayNumber];
  _rayNumber++;

  r.wavelength = rd.wavelength * 1000
      / enclosingObject->material->refractiveIndex;
  r.wavelength_0 = rd.wavelength * 1000;
  r._n_wavepackage = 1.0 / r.wavelength_0;
  r.flux = rd.flux;

  vertex p;
  p << rd.x * _unit << rd.y * _unit << rd.z * _unit;
  r.location = p;
  vertex dir;
  dir << rd.i << rd.j << rd.k;
  r.direction = dir;

  _initMutex.unlock();

  return (r);
}

void ZemaxRaySource::_fillRayBuffer() {



  for (int i = 0; i < MAX_ZEMAX_RAY_BUFFER_SIZE; ++i) {

    if(f.eof())
      break;

    RayData r;

    f.read(reinterpret_cast<char *>(&r.x), 4);
    f.read(reinterpret_cast<char *>(&r.y), 4);
    f.read(reinterpret_cast<char *>(&r.z), 4);

    f.read(reinterpret_cast<char *>(&r.i), 4);
    f.read(reinterpret_cast<char *>(&r.j), 4);
    f.read(reinterpret_cast<char *>(&r.k), 4);

    f.read(reinterpret_cast<char *>(&r.flux), 4);
    if (_spectral) {
      f.read(reinterpret_cast<char *>(&r.wavelength), 4);
    } else
      r.wavelength = 450.0;

    //cout << "wave: " << r.wavelength << endl;
    //cout << "flux: " << r.flux << endl;

    _rayBuffer[i] = r;
    _fluxSum += r.flux;

  }

  if(f.eof())
    cout << "flux sum: " << _fluxSum << endl;

  _rayNumber = 0;

}

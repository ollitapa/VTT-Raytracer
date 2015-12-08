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

#include "SurfaceNormalSource.h"

SurfaceNormalSource::SurfaceNormalSource(RectSurface* theEnclosingObject,
                                         uint64_t n_rays)
    : Source(theEnclosingObject, n_rays) {
  _surface = theEnclosingObject;
  _a = _surface->u();
  _b = _surface->v();
  direction = _surface->normal(_surface->p());
  _toObjectGiven = false;
}

SurfaceNormalSource::SurfaceNormalSource(Json::Value jsonData,
                                         Solid* theEnclosingObject)
    : Source(theEnclosingObject, 1) {
  checkRequiredJSONParam(jsonData, "wavelengths", this);
  checkRequiredJSONParam(jsonData, "intensities", this);
  checkRequiredJSONParam(jsonData, "rays", this);
  checkRequiredJSONParam(jsonData, "attachedTo", this);
  int i = jsonData["attachedTo"].asInt();
  _surface = dynamic_cast<RectSurface*>(theEnclosingObject->surfaces[i]);
  enclosingObject = dynamic_cast<Solid*>(theEnclosingObject->parent);
  // Number of rays
  nRays = jsonData["rays"].asUInt64();
  // Spectrum ###################
  const Json::Value wave = jsonData["wavelengths"];
  const Json::Value inte = jsonData["intensities"];
  vector<double> waves;
  vector<double> intens;

  for (unsigned int i = 0; i < wave.size(); ++i)
    waves.push_back(wave[i].asDouble());

  for (unsigned int i = 0; i < inte.size(); ++i)
    intens.push_back(inte[i].asDouble());

  setSpectrum(waves, intens);
  // ###################
  _a = _surface->u();
  _b = _surface->v();
  direction = _surface->normal(_surface->p());
  // Select direction
  //if(jsonData.isMember("reverseDirection"))
  //  direction *= -1;
  _toObjectGiven = false;
}

SurfaceNormalSource::~SurfaceNormalSource() {
  // TODO Auto-generated destructor stub
}

Ray SurfaceNormalSource::generateRay() {
  Ray r;
  r.previousObjects[_surface->parent->parent->id()] = _surface;
  r.previousObjects[_surface->parent->id()] = _surface;
  _initialiseRay(r);
  // TODO: Direction should be known somehow
  r.direction = direction;
  r.length = 1.0;
  // Generate random starting place
  double ra = rnd1();
  double rb = rnd1();
  // Set location
  r.location = _surface->p() + _a * ra + _b * rb;  // + _surface->normal(_surface->p())*0.0000001;
  return r;
}

AbstractGeometry* SurfaceNormalSource::startingObject() {
  if (_toObjectGiven) {
    _toObjectGiven = false;
    return (_surface);

  } else {
    _toObjectGiven = true;
    return (_surface->parent->parent);
  }
}

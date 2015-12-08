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

#include "IsotropicSurfaceSource.h"

IsotropicSurfaceSource::IsotropicSurfaceSource(RectSurface* theEnclosingObject,
                                               uint64_t n_rays)
    : Source(theEnclosingObject, n_rays) {
  _surface = theEnclosingObject;
  _a = _surface->u();
  _b = _surface->v();
  direction = _surface->normal(_surface->p());
  _toObjectGiven = false;
}

IsotropicSurfaceSource::IsotropicSurfaceSource(Json::Value jsonData,
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

IsotropicSurfaceSource::~IsotropicSurfaceSource() {
  // TODO Auto-generated destructor stub
}

AbstractGeometry* IsotropicSurfaceSource::startingObject() {
  if (_toObjectGiven) {
    _toObjectGiven = false;
    return (_surface);

  } else {
    _toObjectGiven = true;
    return (_surface->parent->parent);
  }
}

Ray IsotropicSurfaceSource::generateRay() {
  Ray r;
  r.previousObjects[_surface->parent->parent->id()] = _surface;
  r.previousObjects[_surface->parent->id()] = _surface;
  _initialiseRay(r);
  // Generate random starting place
  double ra = rnd1();
  double rb = rnd1();
  // Set location
  r.location = _surface->p() + _a * ra + _b * rb;
  vertex n = _surface->normal(r.location);
  // Pick the theta and phi -angle values of the
  // surface normal. We need these values when we use
  // the surface normal as a reference z-axis for the
  // scattered ray.
  double th = acos(n[2]);     // Theta
  double ph = atan2(n[1], n[0]);  // Phi
  // Calculate the theta and phi -angles for the
  // scattered ray. The theta -angle is limited to
  // hemisphere [0...PI/2]!
  // Here we assume that the new ray direction is
  // calculated with respect to the (nominal) global
  // z-axis.
  double tht = acos(rnd1());
  double phi = rnd1() * 2.0 * M_PI;
  // Set the new ray direction in global coordinates
  r.direction[0] = sin(tht) * cos(phi);
  r.direction[1] = sin(tht) * sin(phi);
  r.direction[2] = cos(tht);
  // Rotate the ray direction so that as we would
  // have used the surface normal as the reference
  // z-axis. This transforms the ray direction into
  // local coordinate system set by the surface normal.
  r.direction = RotateY(r.direction, th);
  r.direction = RotateZ(r.direction, ph);
  return r;
}

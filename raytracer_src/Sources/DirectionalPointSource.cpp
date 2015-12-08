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

#include "DirectionalPointSource.h"

DirectionalPointSource::DirectionalPointSource(Solid* theEnclosingObject,
                                               uint64_t n_rays)
    : Source(theEnclosingObject, n_rays) {
  direction << 0 << 0 << 1.0;
  location << 0 << 0 << 0;
}

DirectionalPointSource::DirectionalPointSource(Json::Value jsonData,
                                               Solid* theEnclosingObject)
    : Source(theEnclosingObject, 1) {
  checkRequiredJSONParam(jsonData, "wavelengths", this);
  checkRequiredJSONParam(jsonData, "intensities", this);
  checkRequiredJSONParam(jsonData, "rays", this);
  checkRequiredJSONParam(jsonData, "location", this);
  checkRequiredJSONParam(jsonData, "direction", this);
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
  vertex position;
  Json::Value l = jsonData["location"];
  position << l[0].asDouble() << l[1].asDouble() << l[2].asDouble();
  vertex dire;
  Json::Value d = jsonData["direction"];
  dire << d[0].asDouble() << d[1].asDouble() << d[2].asDouble();
  location = position;
  direction = arma::normalise(dire);
}

DirectionalPointSource::~DirectionalPointSource() {
}

Ray DirectionalPointSource::generateRay() {
  Ray r;
  _initialiseRay(r);
  r.direction = direction;
  r.location = location;
  return (r);
}

AbstractGeometry* DirectionalPointSource::startingObject() {
  return (enclosingObject);
}


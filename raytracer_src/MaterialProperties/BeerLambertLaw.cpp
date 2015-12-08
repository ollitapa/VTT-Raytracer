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

#include "BeerLambertLaw.h"

BeerLambertLaw::BeerLambertLaw(Material* host)
    : ContinuousInteraction(host) {
  _discretization = 0.1;
  _absSaver = AbsorptionSaver::getInstance();
}

BeerLambertLaw::BeerLambertLaw(Json::Value jsonData, Material* host)
    : ContinuousInteraction(host) {
  checkRequiredJSONParam(jsonData, "discretization", this);
  checkRequiredJSONParam(jsonData, "wavelengths", this);
  checkRequiredJSONParam(jsonData, "attenuation", this);
  _absSaver = AbsorptionSaver::getInstance();
  _discretization = jsonData["discretization"].asDouble();
  // Attenuation spectrum ###################
  const Json::Value wave = jsonData["wavelengths"];
  const Json::Value att = jsonData["attenuation"];
  vector<double> waves;
  vector<double> intens;

  for (unsigned int i = 0; i < wave.size(); ++i)
    waves.push_back(wave[i].asDouble());

  for (unsigned int i = 0; i < att.size(); ++i)
    intens.push_back(att[i].asDouble());

  _wavelengths = waves;
  _attenuationCoeffs = intens;
  // ###################
}

BeerLambertLaw::~BeerLambertLaw() {
}

void BeerLambertLaw::performContinuousInteraction(Ray& ray) {
#ifdef DEBUG_VERBOSE
  cout << "Performing continuous interaction on material..." << endl;
#endif
  // Previous ray location
  vertex preLoc = ray.location - ray.length * ray.direction;
  double step = ray.length * _discretization;

  for (double distance = 0; distance <= ray.length;
      distance = step + distance) {
    // Find matching wavelength discretization
    vector<double>::iterator low;
    low = lower_bound(_wavelengths.begin(), _wavelengths.end(), ray.wavelength);

    if (low == _wavelengths.end())
      low -= 1;

    // Attenuation coeff
    double sigma = _attenuationCoeffs[low - _wavelengths.begin()];
    // Save old energy
    double p_before = ray.radiantPower();
    // Calculate attenuation
    ray.flux *= exp(-sigma * step);
    double lostE = p_before - ray.radiantPower();
    // Save absorption
    vertex newLoc = preLoc + ray.direction * (distance + step);
    _absSaver->saveAbsorptionData(newLoc, lostE);
  }

#ifdef DEBUG_VERBOSE
  cout << "... continuous interaction done." << endl;
#endif
}

double BeerLambertLaw::discretization() const {
  return _discretization;
}

void BeerLambertLaw::setDiscretization(double discretization) {
  _discretization = discretization;
}

const vector<double>& BeerLambertLaw::attenuationCoeffs() const {
  return _attenuationCoeffs;
}

void BeerLambertLaw::setAttenuationCoeffs(
    const vector<double>& attenuationCoeffs) {
  _attenuationCoeffs = attenuationCoeffs;
}

const vector<double>& BeerLambertLaw::wavelengths() const {
  return _wavelengths;
}

void BeerLambertLaw::setWavelengths(const vector<double>& wavelengths) {
  _wavelengths = wavelengths;
}


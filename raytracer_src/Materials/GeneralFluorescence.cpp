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

#include "GeneralFluorescence.h"

GeneralFluorescence::GeneralFluorescence(Json::Value jsonData)
    : GeneralScatterer(jsonData) {
  checkRequiredJSONParam(jsonData, "absorptionSpectrumFilenames", this);
  checkRequiredJSONParam(jsonData, "excitationSpectrumFilenames", this);
  checkRequiredJSONParam(jsonData, "cumulativeEmissionSpectrumFilenames", this);
  checkRequiredJSONParam(jsonData, "saveAbsorptionDistribution", this);
  checkRequiredJSONParam(jsonData, "phosphorEfficiencies", this);
  checkRequiredJSONParam(jsonData, "numberOfFluorescentParticles", this);
  checkRequiredJSONParam(jsonData, "exAndEmDataOffsets", this);
  checkRequiredJSONParam(jsonData, "multipleFluorescence", this);

  if (jsonData["saveAbsorptionDistribution"].asBool()) {
    _saveAbsorption = true;
    _absSaver = AbsorptionSaver::getInstance();

  } else
    _saveAbsorption = false;

  Json::Value items;
  cout << "Loading fluorescent particles.." << endl;
  // Fluor particles
  numberOfFluorescentParticles =
      jsonData["numberOfFluorescentParticles"].asUInt();
  // Multi fluor switch
  _multiFluor = jsonData["multipleFluorescence"].asBool();
  cout << "Loading data offsets.." << endl;
  // Data offsets
  vector<unsigned int> offsets;
  items = jsonData["exAndEmDataOffsets"];

  for (unsigned int i = 0; i < items.size(); ++i) {
    offsets.push_back(items[i].asUInt());
  }

  unsigned int j = 0;
  Json::Value em_items = jsonData["cumulativeEmissionSpectrumFilenames"];
  Json::Value ex_items = jsonData["excitationSpectrumFilenames"];
  Json::Value abs_items = jsonData["absorptionSpectrumFilenames"];
  // Load phosphor efficiencies
  Json::Value effs = jsonData["phosphorEfficiencies"];
  cout << "Loading emission and excitation data.. " << endl;

  for (unsigned int i = 0; i < offsets.size(); ++i) {
    for (; j < offsets[i]; ++j) {
      // Load ex and em
      mat ex;
      vec em;
      mat abs;

      if (!em.load(em_items[i].asCString()))
        exit(-1);

      if (!ex.load(ex_items[i].asCString()))
        exit(-1);

      if (!abs.load(abs_items[i].asCString()))
        exit(-1);

      //cout << abs.col(1) << endl;
      // Push data to
      emissionDiscretization.push_back(em.size());
      inverseEmissionWavelengthsCDF.push_back(new vec(em));
      excitationWavelengths.push_back(new vec((ex.col(0))));
      excitationIntensities.push_back(new vec(ex.col(1)));
      _absorptionWavelengths.push_back(new vec(abs.col(0)));
      _absorptionIntensities.push_back(new vec(abs.col(1)));
      phosphorEfficiencies.push_back(effs[i].asDouble());
    }
  }

  cout << "Loading done!" << endl;
}

GeneralFluorescence::~GeneralFluorescence() {
}

void GeneralFluorescence::interactWithRay(Ray& ray) {
  double fluor_prob;
  // Now, Find out which particle type we are
  // hitting.
  unsigned int ptype = _randomParticleType(ray);
  // Spectrum of the absorption
  vec* wave;
  vec* intens;

  // Check if this is a fluorescent particle
  if (ptype < numberOfFluorescentParticles) {
    wave = _absorptionWavelengths[ptype];
    intens = _absorptionIntensities[ptype];
    unsigned int lambda_ind = findClosestIndex(wave, ray.wavelength_0);
    fluor_prob = intens->at(lambda_ind);

  } else {
    fluor_prob = -1;
  }

  if (rnd1() <= fluor_prob && (!ray._fluor || _multiFluor)) {
    ray._fluor = true;
    fluorescence(ray, ptype);

  } else {
    // No fluorescence, only bulk scattering!
    scatter(ray, ptype);
  }
}

void GeneralFluorescence::fluorescence(Ray& ray, unsigned int ptype) {
  // Fluorescence occurring!
  // Find the emission wavelength corresponding to randomly
  // picked value from the probability distribution function
  double lambda_em = 0.0;
  unsigned int random_idx = rndrangeInt(0u, emissionDiscretization[ptype] - 1);
  //cout << random_idx ;
  // Select random emission wavelength
  lambda_em = inverseEmissionWavelengthsCDF[ptype]->at(random_idx);

  //cout << ": "<<ray.wavelength_0 << ": " << ray.wavelength;
  //cout << ": "<<lambda_em ;
  // Restrict fluorescence only to longer wavelengths
  // Check if the obtained emission wavelength is acceptable
  while (lambda_em < ray.wavelength_0) {
    // Obtained wavelength was not suitable!
    // Next, find a new random number from interval [rndvalue,1.0].
    // Setting the previously roulette'd rndvalue as lower
    // limit for random number generation corresponds to setting
    // the lower limit for the corresponding wavelengths to
    // lambda_em. Repeating this procedure we obtain acceptable
    // emission wavelength quicker since with each iteration the
    // relative space of acceptable wavelenghts increases in the
    // value interval corresponding to [rndvalue,1.0]
    random_idx = rndrangeInt(random_idx, emissionDiscretization[ptype] - 1);
    lambda_em = inverseEmissionWavelengthsCDF[ptype]->at(random_idx);
  }

  //cout << ": "<<lambda_em <<endl;
  //vec* wave = excitationWavelengths[ptype];
  //vec* intens = excitationIntensities[ptype];
  //unsigned int lambda_ind = findClosestIndex(wave, ray.wavelength_0);
  //double ex_coef = intens->at(lambda_ind);
  // Calculate new ray energy.
  double old_p = ray.radiantPower();
  double new_p = old_p * phosphorEfficiencies[ptype];
  //double new_p = old_p * ex_coef;
  // flux must be formalized to new power, because the
  // Emission spectrum takes into account the wavelength shift (Stokes)
  double new_flux = new_p / (lambda_em * ray._n_wavepackage);
  ray.flux = new_flux;
  ray.wavelength_0 = lambda_em;
  ray.wavelength = lambda_em / refractiveIndex;
  double r_lost_power = old_p - new_p;
  // Determine the new ray direction due to Fluorescence:
  // Isotropic scattering!
  double tht = acos(1.0 - 2.0 * rnd1());
  double phi = rnd1() * 2.0 * M_PI;
  // Set the new direction vector for the ray
  // No need to rotate coordinate systems because the
  // new direction is anyway random.
  ray.direction(0) = sin(tht) * cos(phi);
  ray.direction(1) = sin(tht) * sin(phi);
  ray.direction(2) = cos(tht);

  // Store absorption data together with coordinates into file
  if (_saveAbsorption)
    _absSaver->saveAbsorptionData(ray.location, r_lost_power);
}

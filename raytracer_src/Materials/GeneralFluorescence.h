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

#ifndef RAYTRACER_SRC_MATERIALS_GENERALFLUORESCENCE_H_
#define RAYTRACER_SRC_MATERIALS_GENERALFLUORESCENCE_H_

#include "GeneralScatterer.h"
#include "Common.h"
#include "SimpleFunctions.h"
#include "Ray.h"
#include <armadillo>
#include <vector>
#include "json.h"
#include <hdf5.h>

using namespace arma;
using namespace std;

class GeneralFluorescence : public GeneralScatterer {
 public:
  GeneralFluorescence(Json::Value jsonData);
  virtual ~GeneralFluorescence();

  virtual void interactWithRay(Ray &ray);

  /**
   * Excitation spectrum wavelengths for each particle type.
   */
  vector<vec*> excitationWavelengths;

  /**
   * Excitation spectrum intensities for each particle type.
   */
  vector<vec*> excitationIntensities;

  /**
   * Number of emission random variables present in
   * cumulativeEmissionWavelengths. Int for each particle type.
   */
  vector<unsigned int> emissionDiscretization;

  /**
   * Inverse cumulative distribution of emission wavelengths for each
   * particle type. [nm]
   * The inverse function is used by randomly picking a number between
   * [0, emissionDiscretization].
   */
  vector<vec*> inverseEmissionWavelengthsCDF;

  /**
   * Number of fluorescent particles. The fluorescence can contain
   * non-fluorescent particles appended after the fluorescent ones.
   */
  unsigned int numberOfFluorescentParticles;

  /**
   * Efficiency of phosphors. [0,1] This is a watt -> watt efficiency and
   * should not be confused with quantum efficiency.
   */
  vector<double> phosphorEfficiencies;

 protected:
  /**
   * Absorption spectrum wavelengths for each particle type.
   */
  vector<vec*> _absorptionWavelengths;
  /**
   * Absorption spectrum intensities for each particle type.
   */
  vector<vec*> _absorptionIntensities;
  /**
   * Fluorescense routine.
   */
  void fluorescence(Ray &ray, unsigned int ptype);

  bool _saveAbsorption;
  AbsorptionSaver* _absSaver;
  bool _multiFluor;
};

#endif /* RAYTRACER_SRC_MATERIALS_GENERALFLUORESCENCE_H_ */

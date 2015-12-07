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

#ifndef RAYTRACER_SRC_MATERIALPROPERTIES_BEERLAMBERTLAW_H_
#define RAYTRACER_SRC_MATERIALPROPERTIES_BEERLAMBERTLAW_H_

#include "ContinuousInteraction.h"
#include "SimpleFunctions.h"
#include "Common.h"
#include "Material.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include "json.h"
#include "AbsorptionSaver.h"

using namespace std;


class BeerLambertLaw: public ContinuousInteraction {
  public:
    BeerLambertLaw(Material* host);
    BeerLambertLaw(Json::Value jsonData, Material* host);
    virtual ~BeerLambertLaw();

    /**
     * This method is called always after the ray has mode a
     * distance ray.lenght. Superclass should not alter any other parameter
     * than the flux and wavelength of the ray.
     */
    virtual void performContinuousInteraction(Ray& ray);


    double discretization() const;
    void setDiscretization(double discretization);
    const vector<double>& attenuationCoeffs() const;
    void setAttenuationCoeffs(const vector<double>& attenuationCoeffs);
    const vector<double>& wavelengths() const;
    void setWavelengths(const vector<double>& wavelengths);

  private:
    double _discretization;
    vector<double> _wavelengths;
    vector<double> _attenuationCoeffs;
    AbsorptionSaver* _absSaver;
};

#endif /* RAYTRACER_SRC_MATERIALPROPERTIES_BEERLAMBERTLAW_H_ */

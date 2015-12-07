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

#ifndef RAYTRACER_SRC_MATERIALS_GENERALSCATTERER_H_
#define RAYTRACER_SRC_MATERIALS_GENERALSCATTERER_H_

#include "Material.h"
#include "Common.h"
#include "SimpleFunctions.h"
#include <armadillo>
#include <cmath>
#include "json.h"
#include <hdf5.h>

using namespace arma;
using namespace std;

class GeneralScatterer: public Material {
  public:

    /*
     *
     */
    GeneralScatterer(Json::Value jsonData);
    virtual ~GeneralScatterer();

    virtual void interactWithRay(Ray &ray);
    void propagationDistance(Ray &ray);
    virtual void scatter(Ray& ray, unsigned int particleType);

  protected:

    /**
     * Initializes the scatterer
     */
    void _initializeScatterer();

    /**
     * Randomly pick particle type used in scattering event.
     * According to particle densities.
     */
    unsigned int _randomParticleType(Ray& ray);

    /*
     * Contains the inverse of cumulative distribution of P(theta), for each
     * particle type. The colum = wavelength. [deg]
     *
     * x = [0,1], y =[0, 180] degrees
     * The values should be evenly distributed, since the scattering direction
     * is picked using uniform random variable.
     *
     * This can be done by
     * theta = [0, 180]
     * mu = cos(theta) ,[-1,1]
     * P(mu) = -0.5 * cumtrapz(p(mu), mu) , [0,1]
     *
     * invMu = P(mu)^-1 , [-1, 1]
     *
     * inverse = arccos(invMu) [0, 180]
     *
     * Remember to re-sample the inverse to have uniform spacing.
     *
     */
    vector<mat*> _inverseThetaCDF;

    /*
     * Scattering cross sections for each particle type as a function of wavelength.
     *
     * Sould be in [um]
     */
    vector<vec*> scatteringCrossSections;

    // Vector containing the particle
    // densities for different particle types
    vector<double> rho;

    // Vector containing the Mean Free Paths
    // for each wavelength
    vector<double> meanFreePaths;


    /*
     * Wavelength vector of probabilities of hitting a type of particle.
     *
     */
    vector<vec*> probabilityOfHittingParticleType;

    // Number of different particle types
    // in the mixture
    int numberOfParticleTypes;

    // Number of system wavelengths
    int numberOfWavelengths;

    /**
     * Vector containing the wavelength discretization in vacuum.
     */
    vec* wavelenghts_0;

    unsigned long long _numberOfParticles;
    unsigned long long _numberOfThetaAngles;



};

#endif /* RAYTRACER_SRC_MATERIALS_GENERALSCATTERER_H_ */

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

#include "IsotropicPointSource.h"

IsotropicPointSource::IsotropicPointSource(Solid* theEnclosingObject,
        uint64_t n_rays)
    : Source(theEnclosingObject, n_rays) {
    location << 0 << 0 << 0;
}

IsotropicPointSource::IsotropicPointSource(Json::Value jsonData,
        Solid* theEnclosingObject) : Source(theEnclosingObject, 1)  {
    checkRequiredJSONParam(jsonData, "wavelengths", this);
    checkRequiredJSONParam(jsonData, "intensities", this);
    checkRequiredJSONParam(jsonData, "rays", this);
    checkRequiredJSONParam(jsonData, "location", this);
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
    location = position;
}

IsotropicPointSource::~IsotropicPointSource() {
    // TODO Auto-generated destructor stub
}

AbstractGeometry* IsotropicPointSource::startingObject() {
    return (enclosingObject);
}

Ray IsotropicPointSource::generateRay() {
    Ray ray;
    _initialiseRay(ray);
    // Calculate the theta and phi -angles for the
    // generated ray.
    double tht = acos(1.0 - 2.0 * rnd1());
    double phi = rnd1() * 2.0 * M_PI;
    // Here we assume that the new ray direction is
    // calculated with respect to the (nominal) global
    // z-axis.
    ray.direction[0] = sin(tht) * cos(phi);
    ray.direction[1] = sin(tht) * sin(phi);
    ray.direction[2] = cos(tht);
    //ray.direction = arma::normalise(ray.direction);
    // Add location and wavelengths
    ray.location = location;
    return (ray);
}



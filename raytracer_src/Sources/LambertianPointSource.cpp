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

#include "LambertianPointSource.h"

LambertianPointSource::LambertianPointSource(Solid* theEnclosingObject,
        uint64_t n_rays):
    Source(theEnclosingObject, n_rays) {
    direction << 0 << 0 << 1.0;
    location << 0 << 0 << 0;
}

LambertianPointSource::LambertianPointSource(Json::Value jsonData,
        Solid* theEnclosingObject) : Source(theEnclosingObject, 1) {
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

LambertianPointSource::~LambertianPointSource() {
    // TODO Auto-generated destructor stub
}

Ray LambertianPointSource::generateRay() {
    Ray ray;
    _initialiseRay(ray);
    // Pick the theta and phi -angle values of the
    // direction. We need these values when we use
    // the given directions as a reference z-axis for the
    // generated lambertian ray.
    double th = acos(direction[2]);     // Theta
    double ph = atan2(direction[1], direction[0]);  // Phi
    // Calculate the theta and phi -angles for the
    // generated ray. The theta -angle is limited to
    // hemisphere [0...PI/2]!
    double tht = asin(sqrt(rnd1()));
    double phi = rnd1() * 2.0 * M_PI;
    // Here we assume that the new ray direction is
    // calculated with respect to the (nominal) global
    // z-axis. Set the new ray direction in global
    // coordinates
    ray.direction[0] = sin(tht) * cos(phi);
    ray.direction[1] = sin(tht) * sin(phi);
    ray.direction[2] = cos(tht);
    // Rotate the ray direction so that as we would
    // have used the surface normal as the reference
    // z-axis. This transforms the ray direction into
    // local coordinate system set by the surface normal.
    ray.direction = RotateY(ray.direction, th);
    ray.direction = RotateZ(ray.direction, ph);
    //ray.direction = arma::normalise(ray.direction);
    // Add location and wavelengths
    ray.location = location;
    return (ray);
}

AbstractGeometry* LambertianPointSource::startingObject() {
    return (enclosingObject);
}


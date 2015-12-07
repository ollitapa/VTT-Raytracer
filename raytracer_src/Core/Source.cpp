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

#include "Source.h"

Source::Source(AbstractGeometry *theEnclosingObject, uint64_t n_rays) {
    // TODO Auto-generated constructor stub
    enclosingObject = theEnclosingObject;
    nRays = n_rays;
    _raysPerWave = n_rays;
    _rayIdx = 0;
    _waveSize = 1;
}

Source::~Source() {
}

void Source::setSpectrum(const vector<double>& wavelengths,
                         const vector<double>& intensities) {
    this->wavelengths = wavelengths;
    this->intensities = intensities;
    _raysPerWave = static_cast<double>(nRays) / wavelengths.size();
    _waveSize = wavelengths.size();
}

void Source::_initialiseRay(Ray& r) {
    _initMutex.lock();
    uint64_t i = _rayIdx % _waveSize;
    r.wavelength = wavelengths[i] / enclosingObject->material->refractiveIndex;
    r.wavelength_0 = wavelengths[i];
    r._n_wavepackage = (intensities[i]  / r.wavelength_0) / (_raysPerWave);
    r.flux = 1.0;
    _rayIdx++;
    _initMutex.unlock();
}


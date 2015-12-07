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

#include "LightToolsRayfileDetector.h"
unsigned int LightToolsRayfileDetector::_count = 0;

LightToolsRayfileDetector::LightToolsRayfileDetector(Json::Value jsonData) {
    _count++;
    checkRequiredJSONParam(jsonData, "name", this);
    checkRequiredJSONParam(jsonData, "stopRays", this);
    checkRequiredJSONParam(jsonData, "includeOnlyNormalDirection", this);
    // Must cooperate with refraction
    _cooperation_level = COOPERATION_LEVEL_REFRACTION;
    _hierarchy = HIERARCHY_REFRACTION;
    stringstream ss;
    ss << _count;
    // Filename for detector
    _rayTmpFilename = "LightTools" + ss.str() + "_" + jsonData["name"].asString()
                      + ".tmp";
    _rayFilename = "LightTools" + ss.str() + "_" + jsonData["name"].asString()
                   + ".ray";
    _specFilename = "LightTools" + ss.str() + "_" + jsonData["name"].asString()
                    + ".sre";
    _rayTmpFile.open(_rayTmpFilename.c_str(), ios::out | ios::binary);
    _stopRays = jsonData["stopRays"].asBool();
    _rayNum = 0;
    _cumulativeRadiantPower = 0;
    arma::vec w = arma::linspace<arma::vec>(200.0, 900.0, 701);
    arma::vec i = arma::zeros<arma::vec>(701);
    _wavelengths = new arma::vec(w);
    _intensities = new arma::vec(i);
    _includeOnlyNormalDir = jsonData["includeOnlyNormalDirection"].asInt();
}

LightToolsRayfileDetector::~LightToolsRayfileDetector() {
    delete _wavelengths;
    delete _intensities;
}

AbstractGeometry* LightToolsRayfileDetector::receiveRay(Ray& ray,
        Surface* surface, AbstractGeometry* from, AbstractGeometry* to) {
    // Check if ray is to be included.
    // _includeOnlyNormalDir < 0 : include rays that are going against
    //                              surface normal
    // _includeOnlyNormalDir > 0 : include rays that are going towards
    //                              surface normal
    // _includeOnlyNormalDir = 0  : include all rays
    double n = arma::dot(surface->normal(ray.location), ray.direction);

    if (_includeOnlyNormalDir != 0) {
        if (signum(n) != signum(_includeOnlyNormalDir))
            return (to);
    }

    _savingMutex.lock();
    // Save ray to file
    /**
     * typedef struct
     * {
     *     float x, y, z;
     *     float l, m, n;
     *     float flux;
     *  }
     */
    float loc1 = ray.location[0];
    float loc2 = ray.location[1];
    float loc3 = ray.location[2];
    _rayTmpFile.write((char*) & (loc1), 4);
    _rayTmpFile.write((char*) & (loc2), 4);
    _rayTmpFile.write((char*) & (loc3), 4);
    float dir1 = ray.direction[0];
    float dir2 = ray.direction[1];
    float dir3 = ray.direction[2];
    _rayTmpFile.write((char*) & (dir1), 4);
    _rayTmpFile.write((char*) & (dir2), 4);
    _rayTmpFile.write((char*) & (dir3), 4);
    float power = ray.radiantPower();
    _rayTmpFile.write((char*) & (power), 4);
    // Add radiant power to the spectrum
    arma::uvec idx = arma::sort_index(arma::abs(*_wavelengths - ray.wavelength_0));
    unsigned int z = arma::as_scalar(idx.at(0));
    _intensities->at(z) += power;
    // Add ray number and power
    _rayNum++;
    _cumulativeRadiantPower += power;

    // Stop ray if applicable
    if (_stopRays)
        ray.flux = 0.0;

    _savingMutex.unlock();
    return (to);
}

void LightToolsRayfileDetector::tracerDidEndTracing() {
    _rayTmpFile.close();
    // Append data from tmp-file
    ifstream ifile(_rayTmpFilename.c_str(), std::ios::in | ios::binary);

    if (!ifile.is_open())
        return;

    ofstream f;
    f.open(_rayFilename.c_str(), ios::out | ios::binary);
    // Write header
    // TODO: Fix LightTools format
    char s[5] = "LTRF";
    f.write((char*) & (s), 4); // Format version ID
    int i1 = 2;
    f.write((char*) & (i1), 4); // Some Int?
    float f1 = 0.0;
    f.write((char*) & (f1), 4); // Some Float?
    int i2 = 1;
    f.write((char*) & (i2), 4); // Some Int?
    float f2 = 0.0;
    f.write((char*) & (f2), 4); // Some Float?
    float f3 = 0.0;
    f.write((char*) & (f3), 4); // Some Float?
    int i3 = 1;
    f.write((char*) & (i3), 4); // Some Int?
    float f4 = 0.0;
    f.write((char*) & (f4), 4); // Some Float?
    float f5 = 0.0;
    f.write((char*) & (f5), 4); // Some Float?
    float f6 = 0.0;
    f.write((char*) & (f6), 4); // Some Float?
    f.write((char*) & (_cumulativeRadiantPower),
            4); // Some Float. Cumulative radiant power apparently.

    //check to see that it exists:
    if (!ifile.is_open()) {
        cout << "file not found for LightTools ray temp" << endl;

    } else {
        f << ifile.rdbuf();
    }

    ifile.close();
    // Close file
    f.close();
    // Remove temp-files
    remove(_rayTmpFilename.c_str());
    // Save spectrum
    ofstream fid;
    fid.open(_specFilename.c_str(), ios::out);
    // Header
    fid << "#\t Spectrum file for " << _rayFilename << endl;
    fid << "dfat 1.0\t" << endl; // TODO: Im not sure on of this lines
    fid << "dataname:\t " << _rayFilename << endl;
    fid << "continuous\t" << endl;
    fid << "radiometric\t" << endl;
    // Normalize spectrum
    *_intensities /= _intensities->max();

    // Save values
    for (unsigned int i = 0; i < _wavelengths->n_elem; ++i) {
        fid << _wavelengths->at(i) << "\t" << _intensities->at(i) << endl;
    }

    fid.close();
}


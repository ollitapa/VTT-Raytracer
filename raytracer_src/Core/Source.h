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

#ifndef SOURCE_H_
#define SOURCE_H_

#include "AbstractGeometry.h"
#include "Ray.h"
#include <stdint.h>
#include <mutex>

class Source {
 public:
  Source(AbstractGeometry *theEnclosingObject, uint64_t n_rays);
  virtual ~Source();

  /**
   * Return the first solid object which starts
   * to propagate the ray.
   */
  virtual AbstractGeometry* startingObject() = 0;

  /**
   * This method will generate rays from this source.
   *
   */
  virtual Ray generateRay() = 0;

  /**
   * Set the spectrum of this source
   */
  void setSpectrum(const vector<double>& wavelengths,
                   const vector<double>& intensities);

  /**
   * Object that this source is attached to.
   */
  AbstractGeometry *enclosingObject;

  /**
   * Number of rays to generate.
   */
  uint64_t nRays;

  /**
   * Discrete wavelengths used. [nm]
   */
  vector<double> wavelengths;

  /**
   * Intensities. [W/nm].
   */
  vector<double> intensities;

 protected:
  /**
   * This function will initialize the ray energy and wavelength.
   * according to its wavelength and spectrum.
   * An evenly distributed rays are assumed.
   */
  void _initialiseRay(Ray& r);

 private:

  uint64_t _rayIdx;
  double _raysPerWave;
  size_t _waveSize;
  mutex _initMutex;
};

#endif /* SOURCE_H_ */

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

#include "Ray.h"

Ray::Ray() {
  // Initialize ray
  length = 1.0;
  flux = 1.0;
  wavelength = 450.0;
  wavelength_0 = 450.0;
  direction << 0 << 0 << 1.0;
  _n_wavepackage = wavelength;
  _fluor = false;
}

Ray::~Ray() {
  // TODO Auto-generated destructor stub
}

ostream& operator<<(ostream& os, Ray& ray) {
  return (\
          os << "Ray: {" << endl \
          << "        length: " << ray.length << endl \
          << "       rad pow: " << ray.radiantPower() << endl \
          << "    wavelength: " << ray.wavelength << endl\
          << "  wavelength_0: " << ray.wavelength_0 << endl\
          << "     direction:\n" << ray.direction << endl \
          << "      location:\n" << ray.location << endl \
          << "}" << endl
         );
}

double Ray::radiantPower() {
  return (_n_wavepackage * wavelength_0 * flux);
}

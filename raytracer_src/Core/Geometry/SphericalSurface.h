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

#ifndef SPHERICALSURFACE_H_
#define SPHERICALSURFACE_H_

#include "Common.h"
#include "AbstractGeometry.h"
#include <armadillo>
#include <vector>
#include <cmath>
#include "Surface.h"

class SphericalSurface : public Surface {
 public:
  SphericalSurface(AbstractGeometry* theParent, vertex center, double radius);
  virtual ~SphericalSurface();
  /**
   * Will calculate the intersection distance to with ray and this surface
   */
  virtual bool rayIntersects(Ray &ray, double& atDistance,
                             AbstractGeometry* fromObject);

  virtual string stlRepresentation();
  vertex& normal(const vertex& distance);

  vertex center;
  double radius;
};

#endif /* SPHERICALSURFACE_H_ */

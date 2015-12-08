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

#ifndef RECTSURFACE_H_
#define RECTSURFACE_H_

#include "AbstractGeometry.h"
#include "Surface.h"
#include "Common.h"
#include "SurfaceProperty.h"
#include "OblivionSurface.h"
#include "Refraction.h"
#include <armadillo>
#include <algorithm>
#include <vector>
#include <cmath>
#include <sstream>
#include "json.h"

class RectSurface : public Surface {
 public:
  RectSurface(AbstractGeometry* theParent, Json::Value jsonData);
  RectSurface(AbstractGeometry* theParent, vector<vertex> theVertices);
  RectSurface(AbstractGeometry* theParent, vertex position, vertex u, vertex v);
  virtual ~RectSurface();

  /**
   * Will calculate the intersection distance to with ray and this surface
   */
  virtual bool rayIntersects(Ray &ray, double& atDistance,
                             AbstractGeometry* fromObject);

  virtual string stlRepresentation();

  /**
   * Returns the surface normal at given distance.
   * distance should be on the surface.
   */
  virtual vertex& normal(const vertex& distance);
  void setNormal(const vertex& normal);
  const vertex& u() const;
  const vertex& v() const;

 protected:

  /**
   * Spanning vector u. Calculated at creation.
   */
  vertex _u;

  /**
   * Spanning vector v. Calculated at creation.
   */
  vertex _v;
};

#endif /* RECTSURFACE_H_ */

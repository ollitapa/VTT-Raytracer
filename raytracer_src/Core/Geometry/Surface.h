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

#ifndef SURFACE_H_
#define SURFACE_H_

#include "AbstractGeometry.h"
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
#include "Solid.h"

using namespace std;

class Surface : public AbstractGeometry {
 public:

  Surface(AbstractGeometry* theParent);
  virtual ~Surface();

  /**
   * Will calculate the intersection distance to with ray and this surface
   */
  virtual bool rayIntersects(Ray &ray, double& atDistance,
                             AbstractGeometry* fromObject);

  /**
   * Propagates the ray through this surface.
   * If this ray has an adjacent surface and the ray is going to that
   * direction, the propagation is given to that surface by calling
   * receiveRay() of that surface.
   *
   * All receiveRay() of surfacePropreties are performed.
   *
   * After receiveRay(), the ray is propagated in the solid it is going to.
   */
  virtual AbstractGeometry* propagateRay(Ray &ray,
                                         AbstractGeometry *fromObject);

  /**
   * This method will handle the changes to the ray. Should not propagate
   * the ray.
   */
  virtual AbstractGeometry* receiveRay(Ray& ray, AbstractGeometry* from,
                                       AbstractGeometry* to);

  /**
   * Add surface property to this surfaces.
   */
  void addProperty(SurfaceProperty* property);

  /**
   * All neighboring surfaces should be associated here.
   */
  void addNeighbour(Surface* newNeighbour);
  vector<Surface*> _neighbours;

  /**
   * Returns the surface normal at given distance.
   * distance should be on the surface.
   */
  virtual vertex& normal(const vertex& distance);
  const vertex& p() const;

  /**
   * Tracer calls this method when the tracing has ended.
   */
  virtual void tracerDidEndTracing();

 protected:

  vector<SurfaceProperty*> _properties;

  AbstractGeometry* _oblivion;

  /**
   * Surface normal. Length is one.
   */
  vertex _normal;

  /**
   * Position vector for the surface
   */
  vertex _p;

  /**
   * Hierarchy of the highest surface property attached
   * to this surface.
   */
  int _surf_prop_hierarchy;

  /**
   * Default surface property.
   */
  SurfaceProperty* _refr;
};

#endif /* SURFACE_H_ */

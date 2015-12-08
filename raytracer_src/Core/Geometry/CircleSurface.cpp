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

#include "CircleSurface.h"

CircleSurface::CircleSurface(AbstractGeometry* theParent, vertex theCenter,
                             double theRadius, vertex theNormal)
    : Surface(theParent) {
  // TODO Auto-generated constructor stub
  center = theCenter;
  radius = theRadius;
  _normal = theNormal;
}

CircleSurface::~CircleSurface() {
}

bool CircleSurface::rayIntersects(Ray &ray, double& atDistance,
                                  AbstractGeometry* fromObject) {
  atDistance = 0;

  if (fromObject == this)
    return (false);

  // Plane intersection by wikipedia:
  // http://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
  vertex p0 = center;
  vertex l = ray.direction;
  vertex l0 = ray.location;
  vertex n = normal(ray.location);
  // Calculate parts of a divide
  double up = arma::dot(p0 - l0, n);
  double down = arma::dot(l, n);

  // Check if parallel or totally inside plane
  if (down < SMALL_EPSILON && down > -SMALL_EPSILON)
    return (false);

  // Calculate distance
  atDistance = up / down;

  // Check that it is not behind the line
  if (atDistance <= -SMALL_EPSILON)
    return (false);

  // Check that the location of intersection is inside the circle.
  double dRayCenter = arma::norm(
      ray.location + ray.direction * atDistance - p0);
#ifdef DEBUG_VERBOSE
  cout << "Circle at: " << atDistance << endl;
  cout << "Ray distance to circle center :" << dRayCenter << endl;
#endif

  if (dRayCenter <= radius)
    return (true);

  return (false);
}


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

#include "SphericalSurface.h"

SphericalSurface::SphericalSurface(AbstractGeometry* theParent,
                                   vertex theCenter, double theRadius)
    : Surface(theParent) {
  // TODO Auto-generated constructor stub
  center = theCenter;
  radius = theRadius;
}

SphericalSurface::~SphericalSurface() {
  // TODO Auto-generated destructor stub
}

bool SphericalSurface::rayIntersects(Ray &ray, double& atDistance,
                                     AbstractGeometry* fromObject) {
  atDistance = 0;
#ifdef DEBUG_RAYINTERSECT
  cout << "-- Surface " << objName << " of " << parent->objName <<
  " calculating intersections." << endl;
  cout << "---- fromObject: " << fromObject->objName << " of " <<
  fromObject->parent->objName << endl;
#endif

  if (fromObject == this) {
    // Line - shpere intersection.
    // See wikipedia:
    // http://en.wikipedia.org/wiki/Line-sphere_intersection
    vertex l = ray.direction;
    vertex o = ray.location;
    vertex c = center;
    //cout << ray.location <<endl;
    // Calculate if ray is going away (+) or toward (-).
    double dir = arma::dot(l, (o - c));
    // Calculate distance to ray from center of the sphere
    double d_ray_center = arma::norm(o - c);
    // Calculate stuff below sqrt
    double below = dir * dir - pow(d_ray_center, 2) + radius * radius;

    // No solution if sqrt(x), x<0
    if (below < 0)
      return (false);

    // If ray direction is outward, no positive intersection.
    if (dir > 0)
      return (false);

    // If ray is inside the sphere, only one positive direction
    // intersection exists. Return it.
    if (d_ray_center <= radius) {
      atDistance = -dir + sqrt(below);
      return (true);
    }

    // Calculate both intersection points (distance to them)
    double d2 = -dir - sqrt(below);
    double d1 = -dir + sqrt(below);

    // Check that they are not behind the ray
    if (d2 < 0 && d1 < 0)
      return (false);

    // Discard shortest
    if (d1 < d2)
      atDistance = d2;

    else
      atDistance = d1;

    //cout << d1 << ", " << d2 << endl;
    return (true);

  } else {
    // Line - shpere intersection.
    // See wikipedia:
    // http://en.wikipedia.org/wiki/Line-sphere_intersection
    vertex l = ray.direction;
    vertex o = ray.location;
    vertex c = center;
    //cout << ray.location <<endl;
    // Calculate if ray is going away (+) or toward (-).
    double dir = arma::dot(l, (o - c));
    // Calculate distance to ray from center of the sphere
    double d_ray_center = arma::norm(o - c);
    // Calculate stuff below sqrt
    double below = dir * dir - pow(d_ray_center, 2) + radius * radius;

    //cout << dir << " " << d_ray_center << " " << below << endl;
    // if (d_ray_center == INFINITY)
    //    exit(0);
    // No solution if sqrt(x), x<0
    if (below < 0)
      return (false);

    // If ray out and not inside, no positive intersection.
    if (d_ray_center > radius && dir > 0)
      return (false);

    // If ray is inside the sphere, only one positive direction
    // intersection exists. Return it.
    if (d_ray_center < radius) {
      atDistance = -dir + sqrt(below);
      return (true);
    }

    // Calculate both intersection points (distance to them)
    double d2 = -dir - sqrt(below);
    double d1 = -dir + sqrt(below);

    // Check that they are not behind the ray
    if (d2 < 0 && d1 < 0)
      return (false);

    // Select shortest
    if (d1 < d2)
      atDistance = d1;

    else
      atDistance = d2;

    //cout << d1 << ", " << d2 << endl;
    return (true);
  }
}

vertex& SphericalSurface::normal(const vertex& distance) {
  _normal = arma::normalise(distance - center);
  return (_normal);
}

string SphericalSurface::stlRepresentation() {
  string stl_snip = "";
  return (stl_snip);
}

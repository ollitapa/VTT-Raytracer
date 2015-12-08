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

#include "Sphere.h"

Sphere::Sphere(vertex theCenter, double theRadius)
    : Solid() {
  center = theCenter;
  radius = theRadius;
  surfaces.push_back(new SphericalSurface(this, center, radius));
}

Sphere::~Sphere() {
  // Delete own side surfaces
  for (std::vector<Surface*>::iterator it = surfaces.begin();
      it != surfaces.end(); ++it) {
    delete *it;
  }
}

Sphere::Sphere(Json::Value jsonData)
    : Solid() {
  checkRequiredJSONParam(jsonData, "location", this);
  checkRequiredJSONParam(jsonData, "radius", this);
  vertex position;
  Json::Value location = jsonData["location"];
  position << location[0].asDouble() << location[1].asDouble()
      << location[2].asDouble();
  center = position;
  radius = jsonData["radius"].asDouble();
  surfaces.push_back(new SphericalSurface(this, center, radius));
}

string Sphere::stlRepresentation() {
  string stl_snip = "";
  return (stl_snip);
}

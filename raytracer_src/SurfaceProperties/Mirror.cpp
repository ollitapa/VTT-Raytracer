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

#include "Mirror.h"

Mirror::Mirror() {
  _hierarchy = HIERARCHY_MIRROR;
  _cooperation_level = COOPERATION_LEVEL_MIRROR;
}

Mirror::~Mirror() {
  _hierarchy = HIERARCHY_MIRROR;
  _cooperation_level = COOPERATION_LEVEL_MIRROR;
}

AbstractGeometry* Mirror::receiveRay(Ray& ray, Surface* surface,
                                     AbstractGeometry* from,
                                     AbstractGeometry* to) {
  //Incident ray can be decomposed to normal and transverse components
  //ki = ki_nor + ki_tra;
  vertex n = surface->normal(ray.location);
  vertex ki = ray.direction;
  vertex ki_nor = arma::dot(ki, n) * n;
  vertex ki_tra = ki - ki_nor;
  vertex kr = ki_tra - ki_nor;
  // Set ray properties
  ray.direction = kr;
  // Send ray back to where it came from
  return (from);
}

Mirror::Mirror(Json::Value jsonData) {
}

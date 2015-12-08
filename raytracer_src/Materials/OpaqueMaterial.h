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

#ifndef RAYTRACER_SRC_MATERIALS_OPAQUEMATERIAL_H_
#define RAYTRACER_SRC_MATERIALS_OPAQUEMATERIAL_H_

#include "Material.h"
#include "json.h"
#include "Ray.h"

class OpaqueMaterial : public Material {
 public:
  OpaqueMaterial();
  OpaqueMaterial(Json::Value jsonData);
  virtual ~OpaqueMaterial();
  void propagationDistance(Ray &ray);
  virtual void interactWithRay(Ray &ray);
};

#endif /* RAYTRACER_SRC_MATERIALS_OPAQUEMATERIAL_H_ */

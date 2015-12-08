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

#include "Material.h"

Material::Material() {
  // TODO Auto-generated constructor stub
  refractiveIndex = 1.0;
}

Material::~Material() {
  // TODO Auto-generated destructor stub
}

void Material::interactWithRayContinuously(Ray &rayAtEnd) {
  for (unsigned int i = 0; i < interactors.size(); i++) {
    interactors[i]->performContinuousInteraction(rayAtEnd);
  }
}

void Material::interactWithRay(Ray& ray) {
}

void Material::propagationDistance(Ray& ray) {
  ray.length = INFINITY;
}

void Material::tracerDidEndTracing() {
}

void Material::addContinuousInteractor(ContinuousInteraction* actor) {
  interactors.push_back(actor);
}

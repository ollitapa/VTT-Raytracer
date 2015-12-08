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

#include "ClearMaterial.h"

ClearMaterial::ClearMaterial() {
  refractiveIndex = 1.0;
}

ClearMaterial::ClearMaterial(double refIndex) {
  refractiveIndex = refIndex;
}

ClearMaterial::ClearMaterial(Json::Value jsonData) {
  if (jsonData.isMember("refractiveIndex"))
    refractiveIndex = jsonData["refractiveIndex"].asDouble();

  else
    refractiveIndex = 1.0;
}

ClearMaterial::~ClearMaterial() {
}

void ClearMaterial::interactWithRay(Ray& ray) {
  ray.length = INFINITY;
}


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

#include "RandomBulkScatter.h"

RandomBulkScatter::RandomBulkScatter() {
  // Default values for the isotropically
  // bulk-scattering objec
  mfp = 1.0;
  refractiveIndex = 1.0;
}

RandomBulkScatter::RandomBulkScatter(double meanfreepath, double refIndex) {
  mfp = meanfreepath;
  refractiveIndex = refIndex;
}

RandomBulkScatter::RandomBulkScatter(Json::Value jsonData) {
  checkRequiredJSONParam(jsonData, "meanFreePath", this);
  checkRequiredJSONParam(jsonData, "refractiveIndex", this);
  mfp = jsonData["meanFreePath"].asDouble();
  refractiveIndex = jsonData["refractiveIndex"].asDouble();
}

RandomBulkScatter::~RandomBulkScatter() {
}

void RandomBulkScatter::interactWithRay(Ray& ray) {
  scatter(ray);
}

void RandomBulkScatter::scatter(Ray &ray) {
  // ---------- DO ISOTROPIC SCATTERING -----------------
  double tht = acos(1.0 - 2.0 * rnd1());
  double phi = rnd1() * 2.0 * M_PI;
  // ----------------------------------------------------
  ray.direction[0] = sin(tht) * cos(phi);
  ray.direction[1] = sin(tht) * sin(phi);
  ray.direction[2] = cos(tht);
}

void RandomBulkScatter::propagationDistance(Ray &ray) {
  // Determine the ray length by using previously
  // initialized (see Constructor!) Mean Free Path
  ray.length = -mfp * log(1.0 - rnd1());
}

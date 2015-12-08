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

#ifndef ISOTROPIC_H_
#define ISOTROPIC_H_

#include "SurfaceProperty.h"
#include "Surface.h"
#include <cmath>
#include "json.h"

class Isotropic : public SurfaceProperty {
 public:
  Isotropic();
  Isotropic(Json::Value jsonData);

  /**
   * Random direction for ray is generated.
   */
  AbstractGeometry* receiveRay(Ray &ray, Surface* surface,
                               AbstractGeometry* from, AbstractGeometry* to);

};

#endif /* ISOTROPIC_H_ */

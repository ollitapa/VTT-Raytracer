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

#ifndef SOLIDCONE_H_
#define SOLIDCONE_H_

#include "Solid.h"
#include "ConeSurface.h"
#include "CircleSurface.h"
#include "UtilityFunctions.h"

#include <cmath>
#include <armadillo>
#include "json.h"

class SolidCone : public Solid {
 public:
  SolidCone(Json::Value jsonData);
  virtual ~SolidCone();

  string stlRepresentation();

 protected:
  double _topR;
  double _bottomR;
  double _height;
  vertex _axialDirection;
  vertex _p;
};

#endif /* SOLIDCONE_H_ */

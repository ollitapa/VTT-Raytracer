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

#ifndef DIRECTIONALPOINTSOURCE_H_
#define DIRECTIONALPOINTSOURCE_H_

#include "Source.h"
#include "Solid.h"
#include "Ray.h"
#include "json.h"

class DirectionalPointSource : public Source {
 public:
  DirectionalPointSource(Solid* theEnclosingObject, uint64_t n_rays);
  DirectionalPointSource(Json::Value jsonData, Solid* theEnclosingObject);

  virtual ~DirectionalPointSource();

  AbstractGeometry* startingObject();
  /**
   * This method will generate rays from this source.
   * The rays are pointed to direction member.
   *
   */
  virtual Ray generateRay();

  /**
   * Direction of the rays. It is initialized to positive z-axis.
   */
  vertex direction;

  /**
   * Location of the source.
   */
  vertex location;
};

#endif /* DIRECTIONALPOINTSOURCE_H_ */

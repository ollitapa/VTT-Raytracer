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

#ifndef INTERNALSURFACE_H_
#define INTERNALSURFACE_H_

#include <armadillo>
#include "AbstractGeometry.h"
#include "Common.h"
#include "Ray.h"
#include "RectSurface.h"
#include "json.h"

using namespace std;

/**
 * Internal surface. Use only if this surface is only internal.
 * Should not touch or be adjacent to any other surface.
 */
class InternalSurface : public RectSurface {
 public:
  InternalSurface(AbstractGeometry* theParent, Json::Value jsonData);
  InternalSurface(AbstractGeometry* theParent, vector<vertex> theVertices);
  InternalSurface(AbstractGeometry* theParent, vertex position, vertex u,
                  vertex v);
  virtual ~InternalSurface();

  /**
   * Propagates the ray through this surface.
   *
   *
   * All receiveRay() of surfacePropreties are performed.
   */
  AbstractGeometry* propagateRay(Ray &ray, AbstractGeometry *fromObject);
};

#endif /* INTERNALSURFACE_H_ */

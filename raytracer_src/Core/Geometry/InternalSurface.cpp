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

#include "InternalSurface.h"

InternalSurface::InternalSurface(AbstractGeometry* theParent,
                                 Json::Value jsonData)
    : RectSurface(theParent, jsonData) {
}

InternalSurface::InternalSurface(AbstractGeometry* theParent,
                                 vector<vertex> theVertices)
    : RectSurface(theParent, theVertices) {
}

InternalSurface::InternalSurface(AbstractGeometry* theParent, vertex position,
                                 vertex u, vertex v)
    : RectSurface(theParent, position, u, v) {
}

InternalSurface::~InternalSurface() {
}

AbstractGeometry* InternalSurface::propagateRay(Ray& ray,
                                                AbstractGeometry* fromObject) {
  return (receiveRay(ray, parent, parent));
}


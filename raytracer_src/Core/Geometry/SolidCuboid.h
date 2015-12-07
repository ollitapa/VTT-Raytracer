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

#ifndef SOLIDCUBOID_H_
#define SOLIDCUBOID_H_

#include "Solid.h"
#include "RectSurface.h"
#include "Common.h"
#include <vector>
#include "json.h"


using namespace std;

/**
 * Solid cuboid with straight sides and 8 vertices.
 */
class SolidCuboid: public Solid {
  public:
    SolidCuboid(double width, double height, double depth, vertex position);
    SolidCuboid(vertex position, vector<vertex> spanningVectors);
    /**
     * JSON data for constructing a Cuboid.
     *
     * Should include:
     * "location"
     * "wx"
     * "wy"
     * "wz"
     */
    SolidCuboid(Json::Value jsonData);
    virtual ~SolidCuboid();

    /**
     * Returns a random point inside the object.
     */
    virtual vertex randomPointInside();

  private:

    vertex _x;
    vertex _y;
    vertex _z;
    vertex _p;

};

#endif /* SOLIDCUBOID_H_ */

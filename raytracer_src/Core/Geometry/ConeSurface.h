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

#ifndef CONESURFACE_H_
#define CONESURFACE_H_

#include "Surface.h"
#include <armadillo>
#include "json.h"
#include <cmath>

class ConeSurface: public Surface {
  public:
    ConeSurface(AbstractGeometry* theParent,
                double height,
                double topR,
                double bottomR,
                vertex& axialDirection,
                vertex& position);
    virtual ~ConeSurface();
    /**
     * Will calculate the intersection distance to with ray and this surface
     */
    virtual bool rayIntersects(Ray &ray, double& atDistance,
                               AbstractGeometry* fromObject);

    /**
     * Location of the bottom cone. Center of the smaller cut segment.
     */
    vertex location;

    virtual vertex& normal(const vertex& distance);

  protected:
    /*
     * Direction of the axis of the cone. The cone circumference will
     * get larger at that direction
     *
     * Should be normalized to one.
     */
    vertex _axialDirection;
    double _height;
    double _halfOpeningAngle;
    double _bottomR;
    double _topR;
    double _heightOfCut;

    bool _insideBounds(vertex& intersectPoint);
};

#endif /* CONESURFACE_H_ */

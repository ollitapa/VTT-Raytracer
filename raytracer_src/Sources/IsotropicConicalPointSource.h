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

#ifndef RAYTRACER_SRC_SOURCES_ISOTROPICCONICALPOINTSOURCE_H_
#define RAYTRACER_SRC_SOURCES_ISOTROPICCONICALPOINTSOURCE_H_

#include "Source.h"
#include "Solid.h"
#include "Common.h"
#include "Ray.h"
#include <cmath>
#include "json.h"

/**
 * Source that will produce isotropically rays to given direction from a point.
 * The rays are constrained in cone around one direction.
 */
class IsotropicConicalPointSource: public Source {
  public:
    IsotropicConicalPointSource(Solid* theEnclosingObject,
                                uint64_t n_rays,
                                double coneHalfAngle);
    IsotropicConicalPointSource(Json::Value jsonData,
                                Solid* theEnclosingObject);
    virtual ~IsotropicConicalPointSource();

    AbstractGeometry* startingObject();
    /**
     * This method will generate rays from this source.
     * The rays are pointed to normal direction of the surface and
     * distributed evenly.
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
  private:
    /**
     * Lover limit of the acos() rnd for conical spread.
     */
    double _limit_low;
};

#endif /* RAYTRACER_SRC_SOURCES_ISOTROPICCONICALPOINTSOURCE_H_ */

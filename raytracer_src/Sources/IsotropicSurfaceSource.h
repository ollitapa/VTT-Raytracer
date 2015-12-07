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

#ifndef RAYTRACER_SRC_SOURCES_ISOTROPICSURFACESOURCE_H_
#define RAYTRACER_SRC_SOURCES_ISOTROPICSURFACESOURCE_H_

#include "Ray.h"
#include "AbstractGeometry.h"
#include "Source.h"
#include "Solid.h"
#include "RectSurface.h"
#include <cmath>
#include "json.h"

/**
 *
 * Isotropic surface source emits rays isotropically from surface
 * to the direction of its normal. Currently works for rectangular
 * surfaces only.
 */
class IsotropicSurfaceSource: public Source {
  public:
    IsotropicSurfaceSource(RectSurface* theEnclosingObject, uint64_t n_rays);
    IsotropicSurfaceSource(Json::Value jsonData, Solid* theEnclosingObject);
    virtual ~IsotropicSurfaceSource();

    AbstractGeometry* startingObject();
    /**
     * This method will generate rays from this source.
     * The rays are pointed to normal direction of the surface and
     * distributed evenly.
     *
     */
    virtual Ray generateRay();

    /**
     * Direction of the rays. It is initialized to surface normal
     */
    vertex direction;

  private:

    RectSurface* _surface;
    vertex _a;
    vertex _b;
    bool _toObjectGiven;
};

#endif /* RAYTRACER_SRC_SOURCES_ISOTROPICSURFACESOURCE_H_ */

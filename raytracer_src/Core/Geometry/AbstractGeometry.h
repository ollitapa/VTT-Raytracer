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


#ifndef ABSTRACTGEOMETRY_H_
#define ABSTRACTGEOMETRY_H_

#include <list>
#include <armadillo>
#include <string>
#include <iostream>
#include "Common.h"
#include "Material.h"
#include "ClearMaterial.h"


class AbstractGeometry {
  public:
    AbstractGeometry();
    AbstractGeometry(const AbstractGeometry& other);
    virtual ~AbstractGeometry();
    virtual AbstractGeometry* propagateRay(Ray &ray,
                                           AbstractGeometry *fromObject) = 0;

    /**
     * Retuns a string that represents the object in STL-format.
     * For solids, it is the following:
     *
     *  solid Name
     *      facet normal ni nj nk
     *          outer loop
     *              vertex v1x v1y v1z
     *              vertex v2x v2y v2z
     *              vertex v3x v3y v3z
     *          endloop
     *      endfacet
     *  endsolid Name
     *
     *  For surfaces it is the following:
     *
     *      facet normal ni nj nk
     *          outer loop
     *              vertex v1x v1y v1z
     *              vertex v2x v2y v2z
     *              vertex v3x v3y v3z
     *          endloop
     *      endfacet
     *      facet normal ni nj nk
     *          outer loop
     *              vertex v1x v1y v1z
     *              vertex v2x v2y v2z
     *              vertex v3x v3y v3z
     *          endloop
     *      endfacet
     *      .
     *      .
     *      .
     *
     *      see wikipedia: http://en.wikipedia.org/wiki/STL_%28file_format%29
     */
    virtual string stlRepresentation();

    /*
     * Vertices of this object
     */
    vector<vertex> vertices;

    /*
     * Material of this object
     */
    Material *material;

    AbstractGeometry* parent;

    /*
     * Id that determines the hirearchy of the Geometry object.
     */
    double id();


    /**
    * Print operator
    */
    friend ostream& operator<<(ostream& os, const AbstractGeometry& geo);

    /**
     * Name of the object. Defaults to _id.
     * Increased automatically at object instantiation.
     */
    string objName;

    /**
     * Tracer calls this method when the tracing has ended.
     */
    virtual void tracerDidEndTracing();

    /**
     * Returns a random point inside the object.
     */
    virtual vertex randomPointInside();

  protected:

    static Material *_def_material;

    /*
     * Id that determines the hirearchy of the Geometry object.
     * Increased automatically at object instantiation.
     */
    double  _id;

    /*
     * Next available id
     */
    static double _next_id;



};

#endif /* ABSTRACTGEOMETRY_H_ */

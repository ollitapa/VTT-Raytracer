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

#ifndef SOLID_H_
#define SOLID_H_

#include "AbstractGeometry.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "Surface.h"
#include "Oblivion.h"
#include "SurfaceProperty.h"
#include <mutex>

class Solid: public AbstractGeometry {

  public:
    Solid();
    virtual ~Solid();

    /**
     * Function that will propagate the ray inside a solid object with sides.
     */
    virtual AbstractGeometry* propagateRay(Ray &ray, AbstractGeometry *fromObject);

    /**
     * Objects completely inside this solid must be enclosed.
     * Will set the objects parent to this.
     */
    void encloseObject(AbstractGeometry* obj);

    /**
     * This will add SurfaceProperty to all of the surfaces
     * associated with this object.
     */
    void addPropertyToSurfaces(SurfaceProperty* prop);

    /**
     * This will add SurfaceProperty to given surface number.
     */
    void addPropertyToSurface(SurfaceProperty* prop, unsigned int surfaceNumber);

    // List of surfaces associated with this object
    std::vector<Surface*> surfaces;

    // List of enclosed objects associated with this object
    vector<Solid*> enclosedObjects;

    virtual string stlRepresentation();

    /**
     * Tracer calls this method when the tracing has ended.
     */
    virtual void tracerDidEndTracing();

    /**
     * Lost energy due to leaking rays terminated
     */
    static double lostLeakEnergy;
  protected:
    Oblivion* _oblivion;
  private:
    vector<Surface*> _allEnclosedSurfaces;
    mutex _savingMutex;

};

#endif /* SOLID_H_ */

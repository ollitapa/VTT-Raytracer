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

#include "Solid.h"
double Solid::lostLeakEnergy = 0;

Solid::Solid() {
  enclosedObjects = vector<Solid*>();
  _oblivion = new Oblivion();
  parent = _oblivion;
  stringstream ss;
  ss << _id;
  objName = "Solid " + ss.str();
}

Solid::~Solid() {
  // TODO Auto-generated destructor stub
  delete _oblivion;
}

AbstractGeometry* Solid::propagateRay(Ray& ray, AbstractGeometry* fromObject) {
#ifdef DEBUG_VERBOSE
  cout << "Solid Prop by " << objName << "########################" << endl;
#endif

  if (dynamic_cast<Surface*>(fromObject) && ray.previousObjects.size() > 1) {
    fromObject = ray.previousObjects[id()];
  }

  // Get surfaces
  vector<Surface*> allSurf = _allEnclosedSurfaces;
  allSurf.insert(allSurf.end(), surfaces.begin(), surfaces.end());
  // Calculate intersection distances
  double min_val = ray.length;
  // Objects at minimum distance
  AbstractGeometry* minSurf = this;
  // This is true after the loop if no plane with intersection is found.
  int nIntersects = 0;

  for (std::vector<Surface*>::iterator it = allSurf.begin();
      it != allSurf.end(); ++it) {
    double d = 0;

    // Find if ray intersects with surface.
    if ((*it)->rayIntersects(ray, d, fromObject)) {
      nIntersects++;

      // Check that surface is ahead of ray.
      if (d < min_val) {  // Check if new intersection is closer
        min_val = d;
        minSurf = *it;
      }
    }
  }

#ifdef DEBUG_SOLIDPROPAGATION
  cout << nIntersects << " Intersections found inside obj: " <<
  this->objName << endl;
  cout << "   ... picking " << minSurf->objName << " of " <<
  minSurf->parent->objName << endl;
#endif
#ifdef DEBUG

  if (nIntersects == 0) {
    cout << "No intersections found!" << endl;
    ray.flux = 0;
  }

#endif

  if (min_val == INFINITY) {
    _savingMutex.lock();
    lostLeakEnergy += ray.radiantPower();
    _savingMutex.unlock();
    ray.flux = 0;
#ifdef DEBUG
    cout << "Ray leaking: " << ray << endl;
    cout << *this << endl;
    cout << "Intersection distances and if found" << endl;

    //Print distances
    for (std::vector<Surface*>::iterator it = allSurf.begin();
        it != allSurf.end(); ++it) {
      double d = 0;
      bool intersects = (*it)->rayIntersects(ray, d, fromObject);

      if (*it != fromObject) {
        cout << d << " ";

      } else {
        cout << "fromObj:" << d << " ";
      }
    }

    cout << endl;

    // Print if intersection found
    for (std::vector<Surface*>::iterator it = allSurf.begin();
        it != allSurf.end(); ++it) {
      double d = 0;
      // Find if ray intersects with surface.
      bool intersects = (*it)->rayIntersects(ray, d, fromObject);
      cout << intersects << " ";
    }

#endif
    return (minSurf);
  }

#ifdef DEBUG_VERBOSE
  cout << " min: " << min_val << endl;
#endif
#ifdef DEBUG_SOLIDPROPAGATION
  cout << "SolidProp by " << objName << ": " << fromObject->objName << " -> " <<
  minSurf->objName
  << " of " << minSurf->parent->objName << endl;
#endif
  // Go to end of ray and return object with min distance
  ray.location += ray.direction * min_val;
  ray.length = min_val;
  return (minSurf);
}

void Solid::encloseObject(AbstractGeometry* obj) {
  if (dynamic_cast<Surface*>(obj)) {
    surfaces.push_back(dynamic_cast<Surface*>(obj));
    _allEnclosedSurfaces.push_back(dynamic_cast<Surface*>(obj));
  }

  enclosedObjects.push_back(dynamic_cast<Solid*>(obj));

  if (dynamic_cast<Solid*>(obj))
    _allEnclosedSurfaces.insert(_allEnclosedSurfaces.end(),
                                dynamic_cast<Solid*>(obj)->surfaces.begin(),
                                dynamic_cast<Solid*>(obj)->surfaces.end());

  obj->parent = this;
}

void Solid::addPropertyToSurfaces(SurfaceProperty* prop) {
  for (std::vector<Surface*>::iterator it = surfaces.begin();
      it != surfaces.end(); ++it) {
    (*it)->addProperty(prop);
  }
}

string Solid::stlRepresentation() {
  string stl_snip = "";
  string name = objName;
  replace(name.begin(), name.end(), ' ', '_');
  // Start with "solid Name"
  stl_snip += "solid " + name + "\n";

  // Ask surfaces to provide facets
  for (std::vector<Surface*>::iterator it = surfaces.begin();
      it != surfaces.end(); ++it) {
    stl_snip += (*it)->stlRepresentation();
  }

  // end of solid
  stl_snip += "endsolid " + name;
  stl_snip += "\n";
  return (stl_snip);
}

void Solid::tracerDidEndTracing() {
  for (std::vector<Surface*>::iterator it = surfaces.begin();
      it != surfaces.end(); ++it) {
    (*it)->tracerDidEndTracing();
  }
}

void Solid::addPropertyToSurface(SurfaceProperty* prop,
                                 unsigned int surfaceNumber) {
  if (surfaceNumber < surfaces.size())
    surfaces[surfaceNumber]->addProperty(prop);
}

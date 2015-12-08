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

#include "AbstractGeometry.h"

// Init id
double AbstractGeometry::_next_id = 0;
Material* AbstractGeometry::_def_material = new Material();

AbstractGeometry::AbstractGeometry() {
  // Id reservation
  _id = _next_id;
  _next_id++;
  // Default material
  material = _def_material;
  parent = NULL;
  stringstream ss;
  ss << _id;
  objName = ss.str();
}

AbstractGeometry::AbstractGeometry(const AbstractGeometry& other)
    : material(other.material),
      parent(other.parent) {
  // Copy will have a new id.
  _id = _next_id;
  _next_id++;
  objName = other.objName;
}

AbstractGeometry::~AbstractGeometry() {
}

double AbstractGeometry::id() {
  return (_id);
}

ostream& operator<<(ostream& os, const AbstractGeometry& geo) {
  return (os << "Object: " << geo.objName.c_str());
}

string AbstractGeometry::stlRepresentation() {
  string myStr = "";
  return (myStr);
}

void AbstractGeometry::tracerDidEndTracing() {
}

vertex AbstractGeometry::randomPointInside() {
  arma::vec3 a;
  return (a);
}

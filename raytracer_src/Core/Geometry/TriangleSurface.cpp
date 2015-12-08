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

#include "TriangleSurface.h"

TriangleSurface::TriangleSurface(AbstractGeometry* theParent,
                                 Json::Value jsonData)
    : Surface(theParent) {
}

TriangleSurface::TriangleSurface(AbstractGeometry* theParent,
                                 vector<vertex> theVertices)
    : Surface(theParent) {
  // Spanning vectors
  vertices = theVertices;
  _p = vertices[0];
  _u = vertices[1] - vertices[0];
  _v = vertices[2] - vertices[0];
  _nu = arma::normalise(_u);
  _nv = arma::normalise(_v);
  // Surface normal
  _normal = arma::normalise(arma::cross(_u, _v));
}

TriangleSurface::TriangleSurface(AbstractGeometry* theParent, vertex position,
                                 vertex u, vertex v)
    : Surface(theParent) {
  _p = position;
  _u = u;
  _v = v;
  _nu = arma::normalise(_u);
  _nv = arma::normalise(_v);
  // Surface normal
  _normal = arma::normalise(arma::cross(_u, _v));
  vertices = vector<vertex>();
  vertices.push_back(_p);
  vertices.push_back(_p + _u);
  vertices.push_back(_p + _v);
}

TriangleSurface::~TriangleSurface() {
  // TODO Auto-generated destructor stub
}

bool TriangleSurface::rayIntersects(Ray& ray, double& atDistance,
                                    AbstractGeometry* fromObject) {
  atDistance = 0;
#ifdef DEBUG_RAYINTERSECT
  cout << "-- Triangle " << objName << " of " << parent->objName <<
  " calculating intersections." << endl;
  cout << "---- fromObject: " << fromObject->objName << " of " <<
  fromObject->parent->objName << endl;
#endif

  if (fromObject == this)
    return (false);

  vertex uu = u();
  vertex vv = v();
  vertex pp = p();

  // Check if ray is parallel to the plane => no intersection distance
  if (fabs(
      pow(arma::dot(ray.direction, _nu), 2)
          + pow(arma::dot(ray.direction, _nv), 2)
          - pow(arma::norm(ray.direction), 2)) < SMALL_EPSILON)
    return false;

  else {
    // Solve ray intersection.
    arma::mat A(3, 3);
    arma::mat B(3, 1);
    A.col(0) = uu;
    A.col(1) = vv;
    A.col(2) = -ray.direction;
    B.col(0) = ray.location - pp;
    vertex x = arma::vec(3);
    bool solution = arma::solve(x, A, B);

    if (!solution)
      return (false);

    if (x[0] <= 1 && x[0] >= 0 && x[1] <= 1 && x[1] >= 0 && x[0] + x[1] <= 1) {
      atDistance = x[2];

      if (atDistance < 0)
        return (false);

      return (true);  // Ray intersects

    } else
      return (false);  // Ray is not on this surface
  }

  // Should never reach here...
  return (false);
}

string TriangleSurface::stlRepresentation() {
  std::stringstream ss;
  string tab = "    ";
  ss << "facet normal 0 0 0\n";
  ss << tab << "outer loop\n";
  // Construct the triangular geometry of the surface
  vec v1 = _p;
  vec v2 = _p + _u;
  vec v3 = _p + _v;
  ss << tab << tab << "vertex " << v1[0] << " " << v1[1] << " " << v1[2]
      << "\n";
  ss << tab << tab << "vertex " << v2[0] << " " << v2[1] << " " << v2[2]
      << "\n";
  ss << tab << tab << "vertex " << v3[0] << " " << v3[1] << " " << v3[2]
      << "\n";
  ss << tab << "endloop\n";
  ss << "endfacet\n";
  return ss.str();
}

const vertex& TriangleSurface::u() const {
  return (_u);
}

const vertex& TriangleSurface::v() const {
  return (_v);
}

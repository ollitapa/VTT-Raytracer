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

#include "SolidCone.h"

SolidCone::SolidCone(Json::Value jsonData)
    : Solid() {
  checkRequiredJSONParam(jsonData, "location", this);
  checkRequiredJSONParam(jsonData, "axialDirection", this);
  checkRequiredJSONParam(jsonData, "topRadius", this);
  checkRequiredJSONParam(jsonData, "bottomRadius", this);
  checkRequiredJSONParam(jsonData, "height", this);
  vertex position;
  // Spanning vectors
  Json::Value l = jsonData["location"];
  position << l[0].asDouble() << l[1].asDouble() << l[2].asDouble();
  vertex u;

  if (jsonData.isMember("axialDirection")) {
    Json::Value d = jsonData["axialDirection"];
    u << d[0].asDouble() << d[1].asDouble() << d[2].asDouble();

  } else
    u << 0 << 0 << 1;

  double topR = jsonData["topRadius"].asDouble();
  double bottomR = jsonData["bottomRadius"].asDouble();
  _height = jsonData["height"].asDouble();

  if (topR < bottomR) {
    double temp = topR;
    topR = bottomR;
    bottomR = temp;
  }

  _topR = topR;
  _bottomR = bottomR;
  _p = position;
  _axialDirection = arma::normalise(u);
  CircleSurface* b = new CircleSurface(this, _p, bottomR, -_axialDirection);
  CircleSurface* t = new CircleSurface(this, (_p + _axialDirection * _height),
                                       topR, _axialDirection);
  ConeSurface* cone = new ConeSurface(this, _height, topR, bottomR,
                                      _axialDirection, _p);
  surfaces.push_back(b);
  surfaces.push_back(t);
  surfaces.push_back(cone);
}

SolidCone::~SolidCone() {
  // TODO Auto-generated destructor stub
}

string SolidCone::stlRepresentation() {
  vector<vertex> points;
  double ax = _axialDirection[0];
  double ay = _axialDirection[1];
  double az = _axialDirection[2];
  // Direction of the axial vector
  double th = acos(az);     // Theta
  double ph = atan2(ay, ax);  // Phi
  // Points on surface are generated around z-axis
  // and rotated to so that the new axis direction is _axialDirection
  vertex z;
  z << 0 << 0 << 1;

  for (int i = 0; i < 2000; ++i) {
    // Generate random height for points
    double rnd_h = rndrangeDouble(0.0, _height);
    // Distance to cone surface in cylindrical coordinates at that
    // height
    double d = rnd_h * (_topR - _bottomR) / _height;
    // Random direction on cone surface.
    double phi = rndrangeDouble(0.0, 2.0 * M_PI);
    // Direction vector
    vertex n;
    n << cos(phi) << sin(phi) << 0;
    // Vector at cone surface
    vertex nn = rnd_h * z + (d + _bottomR) * n;
    // Rotate axis back to _axialDirection
    nn = RotateY(nn, th);
    nn = RotateZ(nn, ph);
    // Move cone to location
    points.push_back(nn + _p);
  }

  // Top circle
  for (int i = 0; i < 000; ++i) {
    double d = rndrangeDouble(0.0, _topR);
    double phi = rndrangeDouble(0.0, 2.0 * M_PI);
    vertex n;
    n << cos(phi) << sin(phi) << 0;
    vertex nn = _height * z + d * n;
    nn = RotateY(nn, th);
    nn = RotateZ(nn, ph);
    points.push_back(nn + _p);
  }

  // Bottom circle
  for (int i = 0; i < 000; ++i) {
    double d = rndrangeDouble(0.0, _bottomR);
    double phi = rndrangeDouble(0.0, 2.0 * M_PI);
    vertex n;
    n << cos(phi) << sin(phi) << 0;
    vertex nn = d * n;
    nn = RotateY(nn, th);
    nn = RotateZ(nn, ph);
    points.push_back(nn + _p);
  }

  // Generate STL
  string stl_snip = "";
  string name = objName;
  // Name shall not include spaces
  replace(name.begin(), name.end(), ' ', '_');
  // Start with "solid Name"
  stl_snip += "solid " + name + "\n";
  // Get triangular facets
  stl_snip += triangulate(points);
  // end of solid
  stl_snip += "endsolid " + name;
  stl_snip += "\n";
  return (stl_snip);
}

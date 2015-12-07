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

#include "SolidCuboid.h"


SolidCuboid::SolidCuboid(vertex position,
                         vector<vertex> spanningVectors) : Solid() {
    vertices = vector<vertex>();
    surfaces = vector<Surface*>();
    vertex x = spanningVectors[0];
    vertex y = spanningVectors[1];
    vertex z = spanningVectors[2];
    surfaces.push_back(new RectSurface(this, position    , z, y));
    surfaces.push_back(new RectSurface(this, position + x, y, z));
    surfaces.push_back(new RectSurface(this, position    , x, z));
    surfaces.push_back(new RectSurface(this, position + y, z, x));
    surfaces.push_back(new RectSurface(this, position    , y, x));
    surfaces.push_back(new RectSurface(this, position + z, x, y));
    vertex v1 = position;
    vertex v2 = position + x;
    vertex v3 = position + x + y;
    vertex v4 = position + y;
    vertex v5 = position + z;
    vertex v6 = position + z + x;
    vertex v7 = position + z + x + y;
    vertex v8 = position + z + y;
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
    _x = x;
    _y = y;
    _z = z;
    _p = position;
}

SolidCuboid::SolidCuboid(double width, double height, double depth,
                         vertex position) : Solid() {
    vertex x, y, z;
    x << width << 0 << 0;
    y << 0 << depth << 0 ;
    z <<  0 << 0 << height;
    vertices = vector<vertex>();
    surfaces = vector<Surface*>();
    surfaces.push_back(new RectSurface(this, position    , z, y));
    surfaces.push_back(new RectSurface(this, position + x, y, z));
    surfaces.push_back(new RectSurface(this, position    , x, z));
    surfaces.push_back(new RectSurface(this, position + y, z, x));
    surfaces.push_back(new RectSurface(this, position    , y, x));
    surfaces.push_back(new RectSurface(this, position + z, x, y));
    vertex v1 = position;
    vertex v2 = position + x;
    vertex v3 = position + x + y;
    vertex v4 = position + y;
    vertex v5 = position + z;
    vertex v6 = position + z + x;
    vertex v7 = position + z + x + y;
    vertex v8 = position + z + y;
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
    _x = x;
    _y = y;
    _z = z;
    _p = position;
}

SolidCuboid::SolidCuboid(Json::Value jsonData): Solid() {
    checkRequiredJSONParam(jsonData, "wx", this);
    checkRequiredJSONParam(jsonData, "wy", this);
    checkRequiredJSONParam(jsonData, "wz", this);
    checkRequiredJSONParam(jsonData, "location", this);
    double width = jsonData["wx"].asDouble();
    double depth = jsonData["wy"].asDouble();
    double height = jsonData["wz"].asDouble();
    vertex position;
    Json::Value location = jsonData["location"];
    position << location[0].asDouble()
             << location[1].asDouble()
             << location[2].asDouble();

    if (jsonData.isMember("origin")) {
        if (jsonData["origin"].asString() == "BaseCenter") {
            vertex nP;
            nP << position[0] - width / 2.0
               << position[1] - depth / 2.0
               << position[2];
            position = nP;
        }
    }

    vertex x, y, z;
    x << width << 0 << 0;
    y << 0 << depth << 0 ;
    z <<  0 << 0 << height;
    vertices = vector<vertex>();
    surfaces = vector<Surface*>();
    surfaces.push_back(new RectSurface(this, position    , z, y));
    surfaces.push_back(new RectSurface(this, position + x, y, z));
    surfaces.push_back(new RectSurface(this, position    , x, z));
    surfaces.push_back(new RectSurface(this, position + y, z, x));
    surfaces.push_back(new RectSurface(this, position    , y, x));
    surfaces.push_back(new RectSurface(this, position + z, x, y));
    vertex v1 = position;
    vertex v2 = position + x;
    vertex v3 = position + x + y;
    vertex v4 = position + y;
    vertex v5 = position + z;
    vertex v6 = position + z + x;
    vertex v7 = position + z + x + y;
    vertex v8 = position + z + y;
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
    _x = x;
    _y = y;
    _z = z;
    _p = position;
}

SolidCuboid::~SolidCuboid() {
    // Delete own side surfaces
    for (std::vector<Surface*>::iterator it = surfaces.begin();
            it != surfaces.end(); ++it) {
        delete *it;
    }
}

vertex SolidCuboid::randomPointInside() {
    vertex xx = rnd1() * _x;
    vertex yy = rnd1() * _y;
    vertex zz = rnd1() * _z;
    return (xx + yy + zz + _p);
}

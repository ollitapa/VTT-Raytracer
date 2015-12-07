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

#include "ConeSurface.h"


ConeSurface::ConeSurface(AbstractGeometry* theParent,
                         double height,
                         double topR,
                         double bottomR,
                         vertex& axialDirection,
                         vertex& position)
    : Surface(theParent) {
    _height = height;

    if (topR < bottomR) {
        double temp = topR;
        topR = bottomR;
        bottomR = temp;
    }

    location = position;
    _p = position;
    _axialDirection = arma::normalise(axialDirection);
    _halfOpeningAngle = atan((topR - bottomR) / _height);
    _topR = topR;
    _bottomR = bottomR;
    _heightOfCut = _bottomR / ((topR - bottomR) / _height);
}

ConeSurface::~ConeSurface() {
    // TODO Auto-generated destructor stub
}

bool ConeSurface::rayIntersects(Ray &ray, double& atDistance,
                                AbstractGeometry* fromObject) {
    // Define that the cone is at orig, so move ray accordingly
    vertex l0 = ray.location - location + _axialDirection * _heightOfCut;
    vertex l = ray.direction;
    vertex d = _axialDirection;
    double theta = _halfOpeningAngle;
    double a = pow(arma::dot(l, d), 2) - pow(cos(theta), 2);
    double b = 2 * (arma::dot(l, d) * arma::dot(l0, d) - arma::dot(l,
                    l0) * pow(cos(theta), 2));
    double c = pow(arma::dot(l0, d), 2) - arma::dot(l0, l0) * pow(cos(theta), 2);
    double belowSqrt = pow(b, 2) - 4 * a * c;

    // Check if intersects at all
    if (belowSqrt < 0)
        return (false);

    double d1 = 0;
    double d2 = INFINITY;

    // Check if only one intersection
    if (a == 0) {
        d1 = -c / b;

        if (fromObject == this) {
            vertex v = ray.location + ray.direction * atDistance;
            return (_insideBounds(v));
        }

    } else {
        d1 = (-b + sqrt(belowSqrt)) / (2 * a);
        d2 = (-b - sqrt(belowSqrt)) / (2 * a);
    }

#ifdef DEBUG_VERBOSE
    cout << "Cone intersects at distances: " << d1 << " " << d2 << endl;
#endif

    if (fromObject == this) {
        if (d1 < SMALL_EPSILON && d2 < SMALL_EPSILON)
            return (false);

        if (d1 < d2)
            atDistance = d2;

        else
            atDistance = d1;

        vertex v = ray.location + ray.direction * atDistance;
        return (_insideBounds(v));

    } else {
        // Check that they are not behind the ray
        // If d1 is < 0 then also d2 < 0.
        if (d1 < SMALL_EPSILON)
            d1 = INFINITY;

        // if d2 < 0 and d1 > 0 then we know that distance = d1
        if (d2 < SMALL_EPSILON)
            d1 = INFINITY;

        if (d1 == INFINITY && d2 == INFINITY)
            return (false);

        vertex v = ray.location + ray.direction * d1;

        if (!_insideBounds(v))
            d1 = INFINITY;

        v = ray.location + ray.direction * d2;

        if (!_insideBounds(v))
            d2 = INFINITY;

        if (d1 == INFINITY && d2 == INFINITY)
            return (false);

        // Select shortest if d1 > 0 and d2 > 0

        if (d1 < d2)
            atDistance = d1;

        else
            atDistance = d2;
    }

    // Check also if we are on the correct cone
    return (true);
}
bool ConeSurface::_insideBounds(vertex& intersectPoint) {
    vertex v = intersectPoint - location;
    double projection = arma::dot(v, _axialDirection);

    if (projection < 0 || projection > _height)
        return (false);

    return (true);
}


vertex& ConeSurface::normal(const vertex& distance) {
    vertex p = distance - location;
    double a = (_topR - _bottomR) / _height;
    vertex r = -arma::dot(p, _axialDirection) * _axialDirection + p;
    r = arma::normalise(r);
    vertex n = -a * _axialDirection + r;
    _normal = arma::normalise(n);
    return (_normal);
}

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

#include "Refraction.h"

Refraction::Refraction() {
    _hierarchy = HIERARCHY_REFRACTION;
    _cooperation_level = COOPERATION_LEVEL_REFRACTION;
}

Refraction::~Refraction() {
    // TODO Auto-generated destructor stub
}

AbstractGeometry* Refraction::receiveRay(Ray& ray, Surface* surface,
        AbstractGeometry* from, AbstractGeometry* to) {
    AbstractGeometry* recobj;
    vertex sn;
    vertex n = surface->normal(ray.location);
    double c = arma::dot(ray.direction, n);

    if (c > 0) {
        sn = -n;
        c *= -1;

    } else
        sn = n;

    // Refraction indices
    double n1RI = from->material->refractiveIndex;
    double n2RI = to->material->refractiveIndex;
    // Fraction of the refractive indices
    double r =  n1RI / n2RI;
    // Factor to determine total internal reflection
    double fact = 1.0 - r * r * (1 - c * c);

    if (fact < 0) {
        // Total internal reflection
        ray.direction = ray.direction - 2.0 * c * sn;
        recobj = from;

    } else {
        // Calculate the Fresnel coefficients to determine whether
        // the ray is going to pass through or reflect due to Fresnel
        // reflection
        double theta_i      = -acos(c);         // Incident ray angle
        double theta_t      = asin(r * sin(theta_i));   // Transmitted ray angle
        double costheta_i   = -c;               // Cosine of the incident ray angle
        double costheta_t   = cos(
                                  theta_t);         // Cosine of the transmitted ray angle
        // Reflectance for s-polarization
        double Rs = pow(fabs((n1RI * costheta_i - n2RI * costheta_t) /
                             (n1RI * costheta_i + n2RI * costheta_t)), 2.0);
        // Reflectance for p-polarization
        double Rp = pow(fabs((n1RI * costheta_t - n2RI * costheta_i) /
                             (n1RI * costheta_t + n2RI * costheta_i)), 2.0);
        // Total reflectance for unpolarized light
        double R = (Rs + Rp) / 2.0;
        // Transmittance for unpolarized light
        double T = 1 - R;

        // throw a dice whether the ray is going through or reflects back
        if (rnd1() <= T) {
            // Transmit the light.
            // Refacting ray direction
            ray.direction = r * ray.direction + (-r * c - sqrt(fact)) * sn;
            // Refractive ray wavelength
            ray.wavelength *= (n1RI / n2RI);
            recobj = to;

        } else {
            // Fresnel reflection!
            ray.direction = ray.direction - 2.0 * c * sn;
            recobj = from;
        }
    }

#ifdef DEBUG

    if (from == to)
        cout << "Same object from and to in refraction!!!!" << endl;

#endif
    // Return receiving object.
    return (recobj);
}

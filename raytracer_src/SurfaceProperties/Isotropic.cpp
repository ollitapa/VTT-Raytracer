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

#include "Isotropic.h"



Isotropic::Isotropic()
{
    _hierarchy = HIERARCHY_DIFFUSE_ISOTROPIC;
    _cooperation_level = COOPERATION_LEVEL_DIFFUSE_ISOTROPIC;
}

Isotropic::Isotropic(Json::Value jsonData) {
    _hierarchy = HIERARCHY_DIFFUSE_ISOTROPIC;
    _cooperation_level = COOPERATION_LEVEL_DIFFUSE_ISOTROPIC;
}

AbstractGeometry* Isotropic::receiveRay(Ray& ray, Surface* surface,
        AbstractGeometry* from, AbstractGeometry* to)
{

	vertex n = surface->normal(ray.location);

	// Check the surface normal direction.
	// If the dot product is positive, then
	// the surface normal is pointing in the
	// wrong direction (outside) and must be
	// reversed by multiplying with -1
	if(arma::dot(ray.direction,n) > 0.0)
	{
		n*=-1;
	}

	// Pick the theta and phi -angle values of the
	// surface normal. We need these values when we use
	// the surface normal as a reference z-axis for the
	// scattered ray.
	double th = acos(n[2]);		// Theta
	double ph = atan2(n[1],n[0]);	// Phi

	// Calculate the theta and phi -angles for the
	// scattered ray. The theta -angle is limited to
	// hemisphere [0...PI/2]!
	// Here we assume that the new ray direction is
	// calculated with respect to the (nominal) global
	// z-axis.
	double tht = acos(rnd1());
	double phi = rnd1()*2.0*M_PI;

	// Set the new ray direction in global coordinates
	ray.direction[0] = sin(tht)*cos(phi);
	ray.direction[1] = sin(tht)*sin(phi);
	ray.direction[2] = cos(tht);

	// Rotate the ray direction so that as we would
	// have used the surface normal as the reference
	// z-axis. This transforms the ray direction into
	// local coordinate system set by the surface normal.
	ray.direction = RotateY(ray.direction,th);
	ray.direction = RotateZ(ray.direction,ph);

	return(from);
}


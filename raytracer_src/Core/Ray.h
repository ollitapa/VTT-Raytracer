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

#ifndef RAY_H_
#define RAY_H_

#include <vector>
#include <map>
#include <armadillo>
#include <iostream>
#include "Common.h"

class AbstractGeometry;

using namespace std;

class Ray {
public:
	Ray();
	Ray(float energy);
	virtual ~Ray();

	double flux;

	/**
	 * Current wavelength of the ray
	 */
	double wavelength;

	/**
	 * Wavelength of the ray at vacuum. n=1. Nanometers
	 */
	double wavelength_0;


	double _n_wavepackage;

	/**
	 * Radiant power of this ray. [W]
	 */
	double radiantPower();

	/**
	 * Ray location
	 */
	vertex location;

	/**
	 * Ray direction
	 */
	vertex direction;

	/**
	 * Length of the ray
	 */
	double length;

	/**
	 * Parameter is True if the ray has undergone fluorescence.
	 */
	bool _fluor;

	/**
	* Print operator
	*/
	friend ostream& operator<<(ostream& os, Ray& ray);

	/**
	 * A map of previous objects the ray was in before this
	 * iteration.
	 */
	map<double,AbstractGeometry*> previousObjects;

};

#endif /* RAY_H_ */

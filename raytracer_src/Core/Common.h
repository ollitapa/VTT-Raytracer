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

#ifndef COMMON_H_
#define COMMON_H_

#include <armadillo>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace arma;

typedef arma::vec vertex;

#define mm 1e-3
#define um 1e-6
#define nm 1e-9
#define SMALL_EPSILON   1e-15
#define SMALL_ENERGY    0.00000001
#define SMALL_FLUX      0.00000001
#define h_c_Jnm         1.986446e-16 //nm J  (nanometer joules)
#define h_c_Jum         1.986446e-19 //um J  (micrometer joules)

#ifndef M_PI
#define M_PI            4*std::atan(1)
#endif

#define HIERARCHY_REFRACTION 0
#define HIERARCHY_ABSORPTION 1
#define HIERARCHY_DIFFUSE_ISOTROPIC 2
#define HIERARCHY_DIFFUSE_LAMBERTIAN 3
#define HIERARCHY_MIRROR 666

#define COOPERATION_LEVEL_REFRACTION 0
#define COOPERATION_LEVEL_ABSORPTION -1
#define COOPERATION_LEVEL_DIFFUSE_ISOTROPIC 2
#define COOPERATION_LEVEL_DIFFUSE_LAMBERTIAN 2
#define COOPERATION_LEVEL_MIRROR 3

/*
 *
 * Seed for random generator.
 *
 */
unsigned rndSeed();

/*
 *
 * rnd1(): Generate a random number between 0 and 1
 *
 */

double rnd1();

/*
 *
 * rndrange(): Generate a random number between min and max
 *
 */
template<typename Type>
Type rndrangeDouble(Type min, Type max) {

#ifdef __APPLE__// TODO: Apple does not suppor thread_local. Random generation compromized.
  static std::mt19937 generator(rndSeed());
#else
  static thread_local std::mt19937 generator(rndSeed());
#endif
  
  std::uniform_real_distribution < Type > distribution(min, max);
  return distribution(generator);
}

/*
 *
 * rndrange(): Generate a random number between min and max
 *
 */
template<typename Type2>
Type2 rndrangeInt(Type2 min, Type2 max) {

#ifdef __APPLE__// TODO: Apple does not suppor thread_local. Random generation compromized.
  static std::mt19937 generator(rndSeed());
#else
  static thread_local std::mt19937 generator(rndSeed());
#endif

  std::uniform_int_distribution < Type2 > distribution(min, max);
  return distribution(generator);
}

/**
 * Sign of the number. Negative: -1, Positive: 1, Zero: 0
 */
template<typename T> inline constexpr
int signum(T x, std::false_type is_signed) {
  return T(0) < x;
}

/**
 * Sign of the number. Negative: -1, Positive: 1, Zero: 0
 */
template<typename T> inline constexpr
int signum(T x, std::true_type is_signed) {
  return (T(0) < x) - (x < T(0));
}

/**
 * Sign of the number. Negative: -1, Positive: 1, Zero: 0
 */
template<typename T> inline constexpr
int signum(T x) {
  return signum(x, std::is_signed<T>());
}

/*
 *
 * ipol(): Interpolate a given data
 *
 */

arma::vec ipol(arma::vec& xx, arma::vec& yy, arma::vec& xi);

/*
 *
 * find1(): Returns indices of vector xx having values larger than a
 *
 */

arma::vec find1(arma::vec &xx, double a);

/*
 *
 * interp1p(): Interpolate a value yi corresponding to xi
 *             from given data arrays (x,y)
 *
 */

double interp1p(arma::vec& x, arma::vec& y, double xi);

/*
 *
 * RotateX(): Rotate a vector k w.r.t x-axis with value
 *        val (rad)
 *
 */
vec RotateX(vec& k, double val);

/*
 *
 * RotateY(): Rotate a vector k w.r.t y-axis with value
 *        val (rad)
 *
 */
vec RotateY(vec& k, double val);

/*
 *
 * RotateZ(): Rotate a vector k w.r.t z-axis with value
 *        val (rad)
 *
 */
vec RotateZ(vec& k, double val);

#endif /* COMMON_H_ */

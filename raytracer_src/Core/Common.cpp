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

#include "Common.h"

unsigned rndSeed() {
  // Generators for seeding random generators for each thread.
  static random_device rd;
  static mt19937 twister(
      std::chrono::system_clock::now().time_since_epoch().count());
  // Use random_device only if it is implemented non-deterministically.
  unsigned seed = rd();

  if (rd.entropy() != 0) {
    cout << "Seeding Random generator with: " << seed;
    cout << ", using random_device(): entropy = " << rd.entropy() << endl;

  } else {  // Otherwise use pseudo random mt19937 with clock seeding.
    seed = twister();
    cout << "Seeding Random generator with: " << seed;
    cout << ", using mt19937()" << endl;
  }

  return (seed);
}

double rnd1() {
  static thread_local std::mt19937 generator(rndSeed());
  static thread_local std::uniform_real_distribution<double> distribution(0.0,
                                                                          1.0);
  return distribution(generator);
}

arma::vec ipol(arma::vec &xx, arma::vec &yy, arma::vec &xi) {
  arma::vec yi = arma::zeros < arma::vec > (xi.n_elem);
  int N = xx.n_elem;
  unsigned int kk;
  int i0;
  double x1, x2, y1, y2;
  double a, b;

  /* run from 0 to 420 --> 421 elements */
  for (kk = 0; kk <= xi.n_elem - 1; kk++) {
    /* find the indices of xx which have larger values than xi(kk) */
    arma::vec ii = find1(xx, xi(kk));

    /* Handle end point */
    if (ii.empty()) {
      yi(kk) = yy(N - 1);

    } else {
      i0 = ii(0);

      /* Handle first point and last point */
      if (i0 == 0) {
        yi(kk) = yy(0);

      } else if (i0 == N - 1) {
        yi(kk) = yy(N - 1);

      } else {
        x1 = xx(i0 - 1);
        x2 = xx(i0);
        y1 = yy(i0 - 1);
        y2 = yy(i0);
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;
        yi(kk) = a * xi(kk) + b;
      }
    }
  }

  return yi;
}

/* Returns indices of vector xx having values larger than a */
arma::vec find1(arma::vec &xx, double a) {
  int N = xx.n_elem;
  arma::vec yy;
  int i;
  int l = 0;

  if (N != 0) {
    for (i = 0; i <= N - 1; i++) {
      if (xx(i) > a) {
        yy.resize(l + 1);
        yy(l) = i;
        l++;
      }
    }
  }

  return yy;
}

double interp1p(arma::vec& x, arma::vec& y, double xi) {
  double dx = x(2) - x(1);
  int i1 = (int) ((xi - x(0)) / dx);
  int i2 = i1 + 1;
  double x1 = x(i1);
  double x2 = x(i2);
  double y1 = y(i1);
  double y2 = y(i2);
  double a = (y2 - y1) / (x2 - x1);
  double b = y1 - a * x1;
  double yi = a * xi + b;
  return yi;
}

vec RotateX(vec& k, double val) {
  mat A;
  // Construct rotation matrix
  A << 1 << 0 << 0 << endr << 0 << cos(val) << -sin(val) << endr << 0
      << sin(val) << cos(val) << endr;
  return A * k;
}

vec RotateY(vec& k, double val) {
  mat A;
  // Construct rotation matrix
  A << cos(val) << 0 << sin(val) << endr << 0 << 1 << 0 << endr << -sin(val)
      << 0 << cos(val) << endr;
  return A * k;
}

vec RotateZ(vec& k, double val) {
  mat A;
  // Construct rotation matrix
  A << cos(val) << -sin(val) << 0 << endr << sin(val) << cos(val) << 0 << endr
      << 0 << 0 << 1 << endr;
  return A * k;
}


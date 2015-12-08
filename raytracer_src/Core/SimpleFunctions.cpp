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

#include "SimpleFunctions.h"

ifstream::pos_type filesize(const char* filename) {
  ifstream in(filename, ifstream::ate | ifstream::binary);
  ifstream::pos_type s = in.tellg();
  in.close();
  return (s);
}

unsigned int findClosestIndex(arma::vec& v, double value) {
  arma::uword i;
  double min_val = arma::abs(v - value).min(i);
  return (i);
}
unsigned int findClosestIndex(arma::vec* v, double value) {
  arma::uword i;
  double min_val = arma::abs(*v - value).min(i);
  return (i);
}


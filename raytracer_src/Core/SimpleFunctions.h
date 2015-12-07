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

#ifndef RAYTRACER_SRC_FILEFUNCTIONS_H_
#define RAYTRACER_SRC_FILEFUNCTIONS_H_

#include <fstream>
#include <string>
#include <sstream>
#include <armadillo>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && (!defined(__CYGWIN__) || !defined(__MINGW__) )
#else
#include <cxxabi.h>
#endif

#include <sys/stat.h>
#include "json.h"

using namespace std;

/**
 * @return The size of the file in bytes.
 */
ifstream::pos_type filesize(const char* filename);

/**
 * Find closest index of value in a vec
 */
unsigned int findClosestIndex(arma::vec& v, double value);
unsigned int findClosestIndex(arma::vec* v, double value);

/**
 * JSON check if parameter is present
 * Will exit if parameter not found!!
 */
template<typename T>
inline void checkRequiredJSONParam(Json::Value& jsonData, string param,
                                   T* sender) {
  if (!jsonData.isMember(param)) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::stringstream ss;
    ss << sender;
    string demangled = ss.str();
#else
    int status;
    char * demangled = abi::__cxa_demangle(typeid(sender).name(), 0, 0, &status);
#endif
    cerr << endl << endl;
    cerr << demangled << ": missing " << param << endl;
    cerr << "Exiting!" << endl;
    exit(-1);
  }
}

template<typename TT>
inline void fileExists(string name, TT* sender) {
  struct stat buffer;

  if (stat(name.c_str(), &buffer) != 0) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::stringstream ss;
    ss << sender;
    string demangled = ss.str();
#else
    int status;
    char * demangled = abi::__cxa_demangle(typeid(sender).name(), 0, 0, &status);
#endif
    cerr << endl << endl;
    cerr << demangled << ": File not found! " << name << endl;
    cerr << "Exiting!" << endl;
    exit(-1);
  }
}
#endif /* RAYTRACER_SRC_FILEFUNCTIONS_H_ */

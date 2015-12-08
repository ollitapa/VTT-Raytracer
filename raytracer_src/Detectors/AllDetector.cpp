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

#include "AllDetector.h"

// Init count
int AllDetector::count = 0;

AllDetector::AllDetector() {
  count++;
  stringstream ss;
  ss << count;
  // Filename for datector
  string fname = "AllDetector_" + ss.str() + ".bin";
  _file.open(fname.c_str(), ios::out | ios::binary);
  cout << fname << endl;
  // Write header
  string header =
      "Ray detector data                           \n"
          "                                            \n"
          "Version: 1                                  \n"
          "                                            \n"
          "format:                                     \n"
          "char header[314],radpwr[f8],wavelength[f8],location[3,f8],direction[3,f8],surface[char8]\n";
  //cout << header << endl;
  //cout << header.size() << endl;
  //exit(-1);
  _file.write(header.c_str(), 314);
}

AllDetector::AllDetector(Json::Value jsonData) {
  count++;
  checkRequiredJSONParam(jsonData, "name", this);
  stringstream ss;
  ss << count;
  // Filename for datector
  string fname = "AllDetector_" + ss.str() + "_" + jsonData["name"].asString()
      + ".bin";
  _file.open(fname.c_str(), ios::out | ios::binary);
  cout << fname << endl;
  // Write header
  string header =
      "Ray detector data                           \n"
          "                                            \n"
          "Version: 1                                  \n"
          "                                            \n"
          "format:                                     \n"
          "char header[314],radpwr[f8],wavelength[f8],location[3,f8],direction[3,f8],surface[char8]\n";
  //cout << header << endl;
  //cout << header.size() << endl;
  //exit(-1);
  _file.write(header.c_str(), 314);
}

AllDetector::~AllDetector() {
}

AbstractGeometry* AllDetector::receiveRay(Ray& ray, Surface* surface,
                                          AbstractGeometry* from,
                                          AbstractGeometry* to) {
  _savingMutex.lock();
  double p = ray.radiantPower();
  _file.write((char*) &p, 8);
  _file.write((char*) &ray.wavelength, 8);
  _file.write((char*) &ray.location[0], 8);
  _file.write((char*) &ray.location[1], 8);
  _file.write((char*) &ray.location[2], 8);
  _file.write((char*) &ray.direction[0], 8);
  _file.write((char*) &ray.direction[1], 8);
  _file.write((char*) &ray.direction[2], 8);
  _file.write((char*) &surface, 8);
  _savingMutex.unlock();
  return (to);
}

void AllDetector::tracerDidEndTracing() {
  _file.close();
}

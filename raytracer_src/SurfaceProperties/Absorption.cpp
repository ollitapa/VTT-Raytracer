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

#include "Absorption.h"

bool Absorption::_configured = false;
string Absorption::_tmpFilenamePoints = "dummy1";
string Absorption::_tmpFilenameData = "dummy2";
string Absorption::_absorptionFilename = "dummy3";
ofstream Absorption::_absorptionFile;
ofstream Absorption::_pointsFile;
unsigned long long Absorption::_absorptionNumber = 0;
bool Absorption::_saveAbsorption = true;
vector<double> Absorption::_points;
vector<double> Absorption::_absorption;
mutex Absorption::_savingMutex;

Absorption::Absorption() {
  // TODO Auto-generated constructor stub
  absorptionCoefficient = 0.0;
  _hierarchy = HIERARCHY_ABSORPTION;
  _cooperation_level = COOPERATION_LEVEL_ABSORPTION;

  if (Absorption::_configured) {
    Absorption::_points.reserve(ABSORPTION_BUFFER_SIZE * 8 * 3);
    Absorption::_absorption.reserve(ABSORPTION_BUFFER_SIZE * 8);
    Absorption::_tmpFilenamePoints = "absorption_surface_points.tmp";
    Absorption::_tmpFilenameData = "absorption_surface_data.tmp";
    Absorption::_saveAbsorption = true;
    Absorption::_initAbsorptionFile();
    Absorption::_absorptionFilename = "SurfaceAbsorption.vtp";
    Absorption::_configured = true;
  }
}

Absorption::Absorption(Json::Value jsonData) {
  checkRequiredJSONParam(jsonData, "absorptionCoefficient", this);
  checkRequiredJSONParam(jsonData, "SaveAbsorptionDistribution", this);
  absorptionCoefficient = jsonData["absorptionCoefficient"].asDouble();
  _hierarchy = HIERARCHY_ABSORPTION;
  _cooperation_level = COOPERATION_LEVEL_ABSORPTION;

  if (!Absorption::_configured) {
    Absorption::_points.reserve(ABSORPTION_BUFFER_SIZE * 8 * 3);
    Absorption::_absorption.reserve(ABSORPTION_BUFFER_SIZE * 8);
    Absorption::_tmpFilenamePoints = "absorption_surface_points.tmp";
    Absorption::_tmpFilenameData = "absorption_surface_data.tmp";

    if (jsonData["SaveAbsorptionDistribution"].asBool()) {
      Absorption::_saveAbsorption = true;
      Absorption::_initAbsorptionFile();

    } else
      Absorption::_saveAbsorption = false;

    Absorption::_absorptionNumber = 0;

    if (jsonData.isMember("AbsorptionFilename"))
      Absorption::_absorptionFilename =
          jsonData["AbsorptionFilename"].asString();

    else
      Absorption::_absorptionFilename = "SurfaceAbsorption.vtp";

    Absorption::_configured = true;
  }
}

Absorption::~Absorption() {
  // TODO Auto-generated destructor stub
}

AbstractGeometry* Absorption::receiveRay(Ray& ray, Surface* surface,
                                         AbstractGeometry* from,
                                         AbstractGeometry* to) {
  Absorption::_savingMutex.lock();
  double lostE = ray.radiantPower();
  ray.flux *= (1 - absorptionCoefficient);
  lostE -= ray.radiantPower();

  if (_saveAbsorption)
    _saveAbsorptionData(ray.location, lostE);

  Absorption::_savingMutex.unlock();
  return to;
}

void Absorption::_initAbsorptionFile() {
  Absorption::_absorptionFile.open(_tmpFilenameData.c_str(),
                                   std::ios::binary | std::ios::out);
  Absorption::_pointsFile.open(_tmpFilenamePoints.c_str(),
                               std::ios::binary | std::ios::out);
}

void Absorption::_saveAbsorptionData(vertex& place, double data) {
  int i = Absorption::_absorptionNumber % ABSORPTION_BUFFER_SIZE;
  Absorption::_absorption[i] = data;
  Absorption::_points[i * 3] = static_cast<double>(place[0]);
  Absorption::_points[i * 3 + 1] = static_cast<double>(place[1]);
  Absorption::_points[i * 3 + 2] = static_cast<double>(place[2]);

  if (i == ABSORPTION_BUFFER_SIZE - 1)
    _saveTempAbsorptionFile(i);

  Absorption::_absorptionNumber++;
}

void Absorption::_saveTempAbsorptionFile(int size) {
  for (int i = 0; i < size; ++i) {
    Absorption::_absorptionFile.write(
        reinterpret_cast<char*>(&(Absorption::_absorption[i])), 8);
  }

  for (int j = 0; j < size * 3; ++j) {
    Absorption::_pointsFile.write(
        reinterpret_cast<char*>(&(Absorption::_points[j])), 8);
  }
}

void Absorption::tracerDidEndTracing() {
  if (Absorption::_saveAbsorption) {
    Absorption::_saveAbsorption = false;
    _saveVTPAbsorptionfile();
  }
}

void Absorption::_saveVTPAbsorptionfile() {
  // Save remaining buffer
  _saveTempAbsorptionFile(_absorptionNumber % ABSORPTION_BUFFER_SIZE);
  Absorption::_absorptionFile.close();
  Absorption::_pointsFile.close();
  string fname = _absorptionFilename;
  // Endianness check
  int nn = 1;
  string endianness = "BigEndian";

  // little endian if true
  if (*(char *) &nn == 1)
    endianness = "LittleEndian";

  ofstream f;
  f.open(fname.c_str(), ios::out | ios::binary);
  f << fixed;  // Fixed formatting
  f
      << "<VTKFile type=\"PolyData\" version=\"1.0\" header_type=\"UInt64\" byte_order=\""
      << endianness << "\">\n";
  f << "    <PolyData>\n";
  f << "        <Piece NumberOfPoints=\"" << Absorption::_absorptionNumber;
  f
      << "\" NumberOfLines=\"0\" NumberOfVerts=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">\n";
  f << "            <Points>\n";
  f
      << "                <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\" offset=\"0\">\n";
  f << "                </DataArray>\n";
  f << "            </Points>\n";
  f << "            <PointData Scalars=\"Absorption\">\n";
  f
      << "                <DataArray type=\"Float64\" Name=\"Absorption\" NumberOfComponents=\"1\" format=\"appended\" offset=\""
      << Absorption::_absorptionNumber * 3 * 8 + 8 << "\">\n";
  f << "                </DataArray>\n";
  f << "            </PointData>\n";
  f << "        </Piece>\n";
  f << "    </PolyData>\n";
  f << "    <AppendedData encoding=\"raw\">\n";
  f << "        _";
  // Abs points
  ifstream::pos_type fsize = filesize(_tmpFilenamePoints.c_str());
  ifstream ifile(_tmpFilenamePoints.c_str(), ios::in | ios::binary);

  //check to see that it exists:
  if (!ifile.is_open()) {
    cout << "file not found for abs points temp" << endl;

  } else {
    f.write((char*) &fsize, 8);
    f << ifile.rdbuf();
  }

  ifile.close();
  // Abs data
  fsize = filesize(_tmpFilenameData.c_str());
  ifstream cfile(_tmpFilenameData.c_str(), ios::in | ios::binary);

  //check to see that it exists:
  if (!cfile.is_open()) {
    cout << "file not found for abs data temp" << endl;

  } else {
    f.write((char*) &fsize, 8);
    f << cfile.rdbuf();
  }

  cfile.close();
  f << "\n";
  f << "    </AppendedData>\n";
  f << "</VTKFile>\n";
  // Remove temp-files
  remove(Absorption::_tmpFilenamePoints.c_str());
  remove(Absorption::_tmpFilenameData.c_str());
}

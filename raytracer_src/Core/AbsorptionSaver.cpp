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

#include "AbsorptionSaver.h"

AbsorptionSaver* AbsorptionSaver::instance = NULL;

AbsorptionSaver::AbsorptionSaver() {
    _tmpFilenameData = "Abs_data.tmp";
    _tmpFilenamePoints = "Abs_points.tmp";
    absorptionFilename  = "AbsorptionData.vtp";
    _absorptionNumber = 0;
    initAbsorptionFile();
    _saved = false;
}

void AbsorptionSaver::initAbsorptionFile() {
    _absorptionFile.open(_tmpFilenameData.c_str(), ios::out | ios::binary);
    _pointsFile.open(_tmpFilenamePoints.c_str(), ios::out | ios::binary);
    //_absorptionFile << "x;y;z;energy"<<endl;
}

void AbsorptionSaver::saveAbsorptionData(vertex& place, double data) {
    _savingMutex.lock();
#ifdef SAVEASCIIRAYS
    _pointsFile << place[0] << " " << place[1] << " " << place[2] << " ";
    _absorptionFile << data << " ";
#endif
#ifndef SAVEASCIIRAYS
    _pointsFile.write((char*)&place[0], 8);
    _pointsFile.write((char*)&place[1], 8);
    _pointsFile.write((char*)&place[2], 8);
    _absorptionFile.write((char*)&data, 8);
#endif
    _absorptionNumber++;
    _savingMutex.unlock();
}


void AbsorptionSaver::saveVTPAbsorptionfile() {
    if (_saved)
        return;

    _absorptionFile.close();
    _pointsFile.close();
    string fname = absorptionFilename;
    ofstream f;
    f.open(fname.c_str(), ios::out | ios::binary);
    // Endianness check
    int nn = 1;
    string endianness = "BigEndian";

    // little endian if true
    if (*(char *)&nn == 1)
        endianness = "LittleEndian";

    f << "<VTKFile type=\"PolyData\" version=\"1.0\" header_type=\"UInt64\" byte_order=\""
      << endianness << "\">\n";;
    f << "    <PolyData>\n";
    f << "        <Piece NumberOfPoints=\"" << _absorptionNumber;
    f
            << "\" NumberOfLines=\"0\" NumberOfVerts=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">\n";
    f << "            <Points>\n";
#ifdef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n                    ";
    ifstream ifile(_tmpFilenamePoints.c_str(), std::ios::in);

    //check to see that it exists:
    if (!ifile.is_open()) {
        cout << "file not found for abs data points temp" << endl;

    } else {
        f << ifile.rdbuf();
    }

    ifile.close();
    f << "\n";
    f << "                </DataArray>\n";
#endif
#ifndef SAVEASCIIRAYS
    f << fixed; // Fixed formatting
    f << "                <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\" offset=\"0\">\n";
    f << "                </DataArray>\n";
#endif
    f << "            </Points>\n";
    f << "            <PointData Scalars=\"Absorption\">\n";
#ifdef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" Name=\"Absorption\">\n                    ";
    ifstream data_file(_tmpFilenameData.c_str(), std::ios::in);

    //check to see that it exists:
    if (!data_file.is_open()) {
        cout << "file not found for abs data temp" << endl;

    } else {
        f << data_file.rdbuf();
    }

    f << "\n";
    f << "                </DataArray>\n";
#endif
#ifndef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" Name=\"Absorption\" NumberOfComponents=\"1\" format=\"appended\" offset=\""
      << _absorptionNumber * 3 * 8 + 8 << "\">\n";
    f << "                </DataArray>\n";
#endif
    f << "            </PointData>\n";
    f << "        </Piece>\n";
    f << "    </PolyData>\n";
#ifndef SAVEASCIIRAYS
    f << "    <AppendedData encoding=\"raw\">\n";
    f << "        _";
    // Points
    ifstream::pos_type fsize = filesize(_tmpFilenamePoints.c_str());
    ifstream ifile(_tmpFilenamePoints.c_str(), ios::in | ios::binary);

    //check to see that it exists:
    if (!ifile.is_open()) {
        cout << "file not found for abs point temp" << endl;

    } else {
        f.write((char*)&fsize, 8);
        f << ifile.rdbuf();
    }

    ifile.close();
    // Absorption
    fsize = filesize(_tmpFilenameData.c_str());
    ifstream cfile(_tmpFilenameData.c_str(), ios::in | ios::binary);

    //check to see that it exists:
    if (!cfile.is_open()) {
        cout << "file not found for abs data temp" << endl;

    } else {
        f.write((char*)&fsize, 8);
        f << cfile.rdbuf();
    }

    cfile.close();
    f << "\n";
    f << "    </AppendedData>\n";
#endif
    f << "</VTKFile>\n";
    f.close();
    // Remove temp-files
    remove(_tmpFilenamePoints.c_str());
    remove(_tmpFilenameData.c_str());
    _saved = true;
}

void AbsorptionSaver::tracerDidEndTracing() {
    if (instance != NULL)
        instance->saveVTPAbsorptionfile();
}

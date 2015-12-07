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

#include "TracerController.h"

TracerController::TracerController() {
    maxIterations = 1000.0;
    maxThreads = 10;
#ifdef SAVERAYS
    // Temporary ray files
    _rayTmpFilename = "ray_paths.tmp";
    _rayTmpFile.open(_rayTmpFilename.c_str(), ios::binary | ios::out);
    _rayOffsetsFilename = "ray_offsets.tmp";
    _rayOffsetsFile.open(_rayOffsetsFilename .c_str(), ios::binary | ios::out);
    _rayPathsFilename = "ray_paths.vtp";
    _rayColorFilename = "ray_color.tmp";
    _rayColorFile.open(_rayColorFilename.c_str(), ios::binary | ios::out);
#endif
    // No rays traced:
    _totalPointNumber = 0;
    _totalRayNumber = 0;
    _maxIterRays = 0;
    _lostPower = 0;
    _lostPowerFluor = 0;
    _raysLeftToTrace = 0;
    //_lostrays = LostRaySaver::getInstance();
}

TracerController::TracerController(Json::Value jsonData) {
    if (jsonData.isMember("maxThreads"))
        maxThreads = jsonData["maxThreads"].asUInt();

    else
        maxThreads = 10;

    if (jsonData.isMember("maxIterations"))
        maxIterations = jsonData["maxIterations"].asDouble();

    else
        maxIterations = 1000.0;

#ifdef SAVERAYS

    if (jsonData.isMember("rayPathsFilename"))
        _rayPathsFilename = jsonData["rayPathsFilename"].asString();

    else
        _rayPathsFilename = "ray_paths.vtp";

    // Temporary ray files
    _rayTmpFilename = "ray_paths.tmp";
    _rayTmpFile.open(_rayTmpFilename.c_str(), ios::binary | ios::out);
    _rayOffsetsFilename = "ray_offsets.tmp";
    _rayOffsetsFile.open(_rayOffsetsFilename.c_str(), ios::binary | ios::out);
    _rayColorFilename = "ray_color.tmp";
    _rayColorFile.open(_rayColorFilename.c_str(), ios::binary | ios::out);
#endif
    // No rays traced:
    _totalPointNumber = 0;
    _totalRayNumber = 0;
    _maxIterRays = 0;
    _lostPower = 0;
    _lostPowerFluor = 0;
    _raysLeftToTrace = 0;
    //_lostrays = LostRaySaver::getInstance();
}


TracerController::~TracerController() {
#ifdef SAVERAYS
    _rayTmpFile.close();
    _rayOffsetsFile.close();
    _rayColorFile.close();
#endif
}

void TracerController::addSource(Source* newSource) {
    _sources.push_back(newSource);
}

void TracerController::startTracing() {
    cout << "Tracing starting..." << endl;
#ifndef SAVERAYS
    cout << "Rays will not be saved..." << endl;
#endif

    // Go through every source and trace their rays.
    for (std::vector<Source*>::iterator s = _sources.begin();
            s != _sources.end(); ++s) {
        cout << "tracing source..." << endl;
        // Set the number of rays left to trace
        _raysLeftToTrace = (*s)->nRays;
        // Tracer work threads are stored here
        vector<thread> ts;

        // Start all threads
        for (unsigned int var = 0; var < maxThreads; ++var) {
            this_thread::sleep_for(chrono::milliseconds(1));
            ts.push_back(thread(&TracerController::_traceRay, this, *s));
        }

        // Wait for all rays to be traced.
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << maxThreads << " threads running" << endl;

        // Join all threads
        for (thread &t : ts) {
            if (t.joinable()) {
                t.join();
            }
        }

        cout << "joined all threads!" << endl;
        cout << "Number of rays that reached max iterations and were terminated: "
             << _maxIterRays << endl;
        cout << "Lost radiant power in terminations: " << _lostPower << endl;
        cout << "Lost fluorescent radiant power in terminations: " << _lostPowerFluor <<
             endl;
        cout << "Lost radiant power in Ray Leaking situations: " <<
             Solid::lostLeakEnergy << endl;
        cout << "Lost energy together: " << _lostPower + Solid::lostLeakEnergy << endl;
        cout << "Source traced." << endl;
    }

    // Format rayfiles
#ifdef SAVERAYS
    _saveVTPRayfile();
#endif
    //_lostrays->endTracing();
    cout << "Tracing ended!" << endl;
}

void TracerController::_traceRay(Source* s) {
    while (true) {
        _rayStartMutex.lock();

        if (_raysLeftToTrace > 0) {
            _raysLeftToTrace--;
            _rayStartMutex.unlock();

        } else {
            _rayStartMutex.unlock();
            break;
        }

        AbstractGeometry* toGeo;
        AbstractGeometry* fromGeo;
        AbstractGeometry* toGeoNew;
        // Generate ray
        Ray r = s->generateRay();
        // Starting geometry
        _rayWriteMutex.lock();
        toGeo = s->startingObject();
        fromGeo = s->startingObject();
        _rayWriteMutex.unlock();
        // Ray data structure
        RayPathData rayData = {};
        rayData.offsets.reserve(_maxIterRays * 8);
        rayData.colors.reserve(_maxIterRays * 8);
        rayData.pathpoints.reserve(_maxIterRays * 8 * 3);
        rayData.rayNumber = 0;
        rayData.lastWavelength = 0;
        rayData.pointNumber = 0;
        // This just for ray positions
#ifdef SAVERAYS
        _saveNewRayPath(r, rayData);
#endif
        // Propagate until small energy or max number of iterations
        double iter = 0;

        while (r.flux > SMALL_FLUX) {
            // Check if too many iterations, so that we do not end up
            // in endless loop.
            if (iter > maxIterations) {
                //cout << "Max number of iterations reached!" << endl;
                //cout << "Starting new ray..." << endl;
                _maxIterRays++;
                _powerSavingMutex.lock();

                if (r._fluor) {
                    _lostPower += r.radiantPower();
                    _lostPowerFluor += r.radiantPower();

                } else
                    _lostPower += r.radiantPower();

                _powerSavingMutex.unlock();
                //_lostrays->saveRay(r);
                break;
            }

            // Mean free path
            toGeo->material->propagationDistance(r);
            // Propagate
            toGeoNew = toGeo->propagateRay(r, fromGeo);
            // Interact with continous material
            toGeo->material->interactWithRayContinuously(r);

            // Interact if not at surface
            if (!dynamic_cast<Surface*>(toGeo))
                toGeoNew->material->interactWithRay(r);

            // Setup new round
            fromGeo = toGeo;
            toGeo = toGeoNew;
            // Log position
#ifdef SAVERAYS
            _saveRayPathPoint(r, rayData);
#endif
            // Iteration done
            iter++;
        }

#ifdef SAVERAYS
        _saveRayEnd(rayData);
        _saveRayPathData(rayData);
#endif
    }

    cout << "end thread" << endl;
}

void TracerController::_saveNewRayPath(Ray& ray, RayPathData& data) {
    // Save location
    data.pathpoints.push_back(ray.location[0]);
    data.pathpoints.push_back(ray.location[1]);
    data.pathpoints.push_back(ray.location[2]);
    data.pointNumber++;
    data.lastWavelength = ray.wavelength;
}

void TracerController::_saveRayPathPoint(Ray& ray, RayPathData& data) {
    // Save location
    data.pathpoints.push_back(ray.location[0]);
    data.pathpoints.push_back(ray.location[1]);
    data.pathpoints.push_back(ray.location[2]);
    data.pointNumber++;

    // Save new ray if wavelenght changed.
    if (ray.wavelength != data.lastWavelength) {
        _saveRayEnd(data);
        _saveNewRayPath(ray, data);
    }
}

void TracerController::_saveRayEnd(RayPathData& data) {
    data.offsets.push_back(data.pointNumber);
    data.colors.push_back(data.lastWavelength);
    data.rayNumber++;
}

void TracerController::_saveRayPathData(RayPathData& data) {
    _rayWriteMutex.lock();

    // Add numbers from other rays allready saved.
    for (unsigned int var = 0; var < data.offsets.size(); ++var) {
        data.offsets[var] += _totalPointNumber;
    }

#ifdef SAVEASCIIRAYS

    for (unsigned int var = 0; var < data.offsets.size(); ++var) {
        _rayOffsetsFile << data.offsets[var] << " ";
    }

    for (unsigned int var = 0; var < data.colors.size(); ++var) {
        _rayColorFile << data.colors[var] << " ";
    }

    for (unsigned int var = 0; var < data.pathpoints.size(); ++var) {
        _rayTmpFile << data.pathpoints[var] << " ";
    }

#endif
#ifndef SAVEASCIIRAYS

    for (unsigned int var = 0; var < data.offsets.size(); var++) {
        _rayOffsetsFile.write((char*) & (data.offsets[var]), 8);
    }

    for (unsigned int var = 0; var < data.colors.size(); var++) {
        _rayColorFile.write((char*) & (data.colors[var]), 8);
    }

    for (unsigned int var = 0; var < data.pathpoints.size(); var++) {
        _rayTmpFile.write((char*) & (data.pathpoints[var]), 8);
    }

#endif
    _totalPointNumber += data.pointNumber;
    _totalRayNumber += data.rayNumber;
    _rayWriteMutex.unlock();
}



void TracerController::_saveVTPRayfile() {
    _rayTmpFile.close();
    _rayOffsetsFile.close();
    _rayColorFile.close();
    string fname = _rayPathsFilename;
    ofstream f;
    f.open(fname.c_str(), ios::out | ios::binary);
    // Endianness check
    int nn = 1;
    string endianness = "BigEndian";

    // little endian if true
    if (*(char *)&nn == 1)
        endianness = "LittleEndian";

    f << "<VTKFile type=\"PolyData\" version=\"1.0\" header_type=\"UInt64\" byte_order=\""
      << endianness << "\">\n";
    f << "    <PolyData>\n";
    f << "        <Piece NumberOfPoints=\"" << _totalPointNumber;
    f << "\" NumberOfLines=\"" << _totalRayNumber;
    f << "\" NumberOfVerts=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">\n";
    f << "            <Points>\n";
#ifdef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n                    ";
    ifstream ifile(_rayTmpFilename.c_str(), std::ios::in);

    //check to see that it exists:
    if (!ifile.is_open()) {
        cout << "file not found for ray path temp" << endl;

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
    f << "            <CellData Scalars=\"Wavelength\">\n";
#ifdef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" Name=\"Wavelength\" NumberOfComponents=\"1\" format=\"ascii\">\n                    ";
    ifstream cfile(_rayColorFilename.c_str(), std::ios::in);

    //check to see that it exists:
    if (!cfile.is_open()) {
        cout << "file not found for ray wavelength temp" << endl;

    } else {
        f << cfile.rdbuf();
    }

    cfile.close();
    f << "\n";
    f << "                </DataArray>\n";
#endif
#ifndef SAVEASCIIRAYS
    f << "                <DataArray type=\"Float64\" Name=\"Wavelength\" NumberOfComponents=\"1\" format=\"appended\" offset=\""
      << _totalPointNumber * 3 * 8 + 8 << "\">\n";
    f << "                </DataArray>\n";
#endif
    f << "            </CellData>\n";
    f << "            <Lines>\n";
#ifdef SAVEASCIIRAYS
    f << "                <DataArray type=\"UInt64\"  Name=\"connectivity\" format=\"ascii\">\n                    ";

    for (double var = 0; var < _pointNumber; ++var) {
        f << var << " ";
    }

    f << "\n";
    f << "                </DataArray>\n";
    f << "                <DataArray type=\"UInt64\" Name=\"offsets\">\n                    ";
    ifstream off_file(_rayOffsetsFilename.c_str(), std::ios::in);

    //check to see that it exists:
    if (!off_file.is_open()) {
        cout << "file not found for ray path temp" << endl;

    } else {
        f << off_file.rdbuf();
    }

    f << "\n";
    f << "                </DataArray>\n";
#endif
#ifndef SAVEASCIIRAYS
    f << "                <DataArray type=\"UInt64\"  Name=\"connectivity\" format=\"appended\" offset=\""
      << _totalPointNumber * 3 * 8 + 8 + _totalRayNumber * 8 + 8 << "\">\n";
    f << "                </DataArray>\n";
    f << "                <DataArray type=\"UInt64\" Name=\"offsets\" format=\"appended\" offset=\""
      << _totalPointNumber * 3 * 8 + 8 + _totalRayNumber * 8 + 8  + _totalPointNumber
      * 8 + 8 << "\">\n";
    f << "                </DataArray>\n";
#endif
    f << "            </Lines>\n";
    f << "        </Piece>\n";
    f << "    </PolyData>\n";
#ifndef SAVEASCIIRAYS
    f << "    <AppendedData encoding=\"raw\">\n";
    f << "        _";
    // Ray path
    ifstream::pos_type fsize = filesize(_rayTmpFilename.c_str());
    ifstream ifile(_rayTmpFilename.c_str(), std::ios::in | ios::binary);

    //check to see that it exists:
    if (!ifile.is_open()) {
        cout << "file not found for ray path temp" << endl;

    } else {
        f.write((char*)&fsize, 8);
        f << ifile.rdbuf();
    }

    ifile.close();
    // Ray color
    fsize = filesize(_rayColorFilename.c_str());
    ifstream cfile(_rayColorFilename.c_str(), std::ios::in | ios::binary);

    //check to see that it exists:
    if (!cfile.is_open()) {
        cout << "file not found for ray wavelength temp" << endl;

    } else {
        f.write((char*)&fsize, 8);
        f << cfile.rdbuf();
    }

    cfile.close();
    // Connectivity
    fsize = _totalPointNumber * 8;
    f.write((char*)&fsize, 8);

    for (long long var = 0; var < _totalPointNumber; ++var) {
        f.write((char*)&var, 8);
    }

    // Offsets
    fsize = filesize(_rayOffsetsFilename.c_str());
    ifstream off_file(_rayOffsetsFilename.c_str(), ios::in | ios::binary);

    //check to see that it exists:
    if (!off_file.is_open()) {
        cout << "file not found for ray path temp" << endl;

    } else {
        f.write((char*)&fsize, 8);
        f << off_file.rdbuf();
    }

    off_file.close();
    f << "\n";
    f << "    </AppendedData>\n";
#endif
    f << "</VTKFile>\n";
    f.close();
    cout << "Rays saved" << endl;
    // Remove temp-files
    remove(_rayTmpFilename.c_str());
    remove(_rayOffsetsFilename.c_str());
    remove(_rayColorFilename.c_str());
}



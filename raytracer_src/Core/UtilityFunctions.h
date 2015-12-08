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

#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include "Common.h"
#include <armadillo>
#include "json.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <ctype.h>
#include <exception>
#include <stdint.h>

class SolidCone;

#include "AbstractGeometry.h"
#include "Surface.h"
#include "CircleSurface.h"
#include "RectSurface.h"
#include "Solid.h"
#include "SolidCone.h"
#include "SolidCuboid.h"
#include "SolidPoly.h"

/**
 * Find common surfaces of two Solids.
 * TODO: Currently supports only SolidCuboid and Cone
 */
vector<Surface*> findCommonSurfaces(AbstractGeometry* aa, AbstractGeometry* bb);

vector<Surface*> findCommonSurfacesWithCone(SolidCuboid* a, SolidCone* b);

/**
 * Export geometry to STL file Geometry.stl
 */
void exportStl(vector<AbstractGeometry*>& objList);

/**
 * Triangulation routine to make facets from pointcloud for 3D presentation.
 *
 * @return String with facets in STL-format
 */
string triangulate(vector<vertex>& points);

/**
 * Import routine for ascii STL-geometry. STL file can have multiple
 * separate object with names.
 *
 * Look for line "solid somename"
 *
 * @return map of imported objects. Accessible by names.
 */
map<string, AbstractGeometry*> importStl(string& filename);

/**
 * Import routine for STL-geometry. STL-file should only
 * include one object. The name of the object is taken from the filename.
 * File can be ascii or binary.
 *
 * @param[out] nametag Name of the object from the filename.
 * @param filename Path to the file
 *
 * @return Imported object.
 */
AbstractGeometry* importSingleStl(string& filename);

/**
 * Import routine for ascii STL-geometry. STL-file should only
 * include one object. The name of the object is taken from the filename.
 *
 * @param[out] nametag Name of the object from the filename.
 * @param filename Path to the file
 *
 * @return map of imported objects. Accessible by names.
 */
AbstractGeometry* importSingleStlASCII(string& filename);

/**
 * Import routine for binary STL-geometry. STL-file should only
 * include one object. The name of the object is taken from the filename.
 *
 * @param[out] nametag Name of the object from the filename.
 * @param filename Path to the file
 *
 * @return map of imported objects. Accessible by names.
 */
AbstractGeometry* importSingleStlBinary(string& filename);

#endif /* UTILITYFUNCTIONS_H_ */

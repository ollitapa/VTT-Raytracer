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

#include "UtilityFunctions.h"

vector<Surface*> findCommonSurfaces(AbstractGeometry* aa,
                                    AbstractGeometry* bb) {
  vector<Surface*> result;

  // Handle solidcuboid and cone
  if (dynamic_cast<SolidCuboid*>(aa) && dynamic_cast<SolidCone*>(bb)) {
    result = findCommonSurfacesWithCone(dynamic_cast<SolidCuboid*>(aa),
                                        dynamic_cast<SolidCone*>(bb));
    return (result);
  }

  if (dynamic_cast<SolidCuboid*>(bb) && dynamic_cast<SolidCone*>(aa)) {
    result = findCommonSurfacesWithCone(dynamic_cast<SolidCuboid*>(bb),
                                        dynamic_cast<SolidCone*>(aa));
    return (result);
  }

  // This function only works for cuboids.
  if (!dynamic_cast<SolidCuboid*>(aa) || !dynamic_cast<SolidCuboid*>(bb))
    return (result);

  SolidCuboid* a = dynamic_cast<SolidCuboid*>(aa);
  SolidCuboid* b = dynamic_cast<SolidCuboid*>(bb);
  int iA = 0;
  int iB = 0;

  for (vector<Surface*>::iterator surf = a->surfaces.begin();
      surf != a->surfaces.end(); ++surf) {
    RectSurface* s = dynamic_cast<RectSurface*>(*surf);
    vertex p1 = s->p();
    vertex n1 = s->normal(p1);
    iB = 0;

    for (vector<Surface*>::iterator adjSurf = b->surfaces.begin();
        adjSurf != b->surfaces.end(); ++adjSurf) {
      RectSurface* adjS = dynamic_cast<RectSurface*>(*adjSurf);

      if (adjS == s)
        continue;

      vertex p2 = adjS->p();
      vertex n2 = adjS->normal(p2);
      // Check if planes are parallel and
      double up = arma::dot(p2 - p1, n1);
      double down = abs(arma::dot(n1, n2));

      if (down == 1 && up == 0) {
        vector<vertex>& verts = s->vertices;

        for (vector<vertex>::iterator vv = verts.begin(); vv != verts.end();
            ++vv) {
          Ray r;
          r.location = *vv - n2;
          r.direction = n2;
          r.length = INFINITY;
          double d;

          if (s->rayIntersects(r, d, adjS)) {
            result.push_back(*surf);
            result.push_back(*adjSurf);
            break;
          }
        }
      }

      iB++;
    }

    iA++;
  }

  return (result);
}

vector<Surface*> findCommonSurfacesWithCone(SolidCuboid* a, SolidCone* b) {
  vector<Surface*> result;
  // Top and bottom of the cone
  vector<CircleSurface*> coneTB;
  coneTB.push_back(dynamic_cast<CircleSurface*>(b->surfaces[0]));
  coneTB.push_back(dynamic_cast<CircleSurface*>(b->surfaces[1]));

  for (vector<Surface*>::iterator surf = a->surfaces.begin();
      surf != a->surfaces.end(); ++surf) {
    RectSurface* s = dynamic_cast<RectSurface*>(*surf);
    vertex p1 = s->p();
    vertex n1 = s->normal(p1);

    for (vector<CircleSurface*>::iterator adjSurf = coneTB.begin();
        adjSurf != coneTB.end(); ++adjSurf) {
      CircleSurface* circle = *adjSurf;
      vertex n2 = circle->normal(circle->center);
      // Check if planes are parallel and
      double up = arma::dot(circle->center - p1, n1);
      double down = abs(arma::dot(n1, n2));

      //cout << down << "  " << up << endl;

      if (down == 1 && -SMALL_EPSILON < up && up < SMALL_EPSILON) {
        // TODO: All co-planar surfaces are adjacent to circles.
        result.push_back(*surf);
        result.push_back(circle);
        /**
         for (unsigned int var = 0; var < s->vertices.size(); ++var) {
         vertex t = s->vertices[var];
         cout << arma::norm( circle->center - t ) << endl;
         if(arma::norm( circle->center - t ) <= circle->radius )
         {
         result.push_back(*surf);
         result.push_back(circle);
         break;
         }
         }*/
      }

      // Chek
    }
  }

  return (result);
}

void exportStl(vector<AbstractGeometry*>& objList) {
  // --------------------------------------------------
  // CONSTRUCT GEOMETRY FILE OF THE OBJECTS
  cout << "Exporting Geometry to STL...." << endl;
  cout << "Number of objects: " << objList.size() << endl;
  string geometryFileName = "Geometry.stl";
  // Write/append them into file
  ofstream ofile;
  ofile.open(geometryFileName.c_str());

  // Generate a geometry file out of object
  for (unsigned int i = 0; i < objList.size(); i++) {
    string name = objList[i]->objName;
    cout << "exporting: " << objList[i]->objName << endl;

    if (dynamic_cast<Surface*>(objList[i])) {
      string stl_snip = "";
      replace(name.begin(), name.end(), ' ', '_');
      // Start with "solid Name"
      ofile << "solid " << name << "\n";
    }

    ofile << objList[i]->stlRepresentation();

    if (dynamic_cast<Surface*>(objList[i])) {
      string stl_snip = "";
      replace(name.begin(), name.end(), ' ', '_');
      // Start with "solid Name"
      ofile << "endsolid " << name << "\n";
    }
  }

  ofile.close();
  cout << "Done!" << endl << endl;
}

string triangulate(vector<vertex>& points) {
  // Write points into file
  ofstream ofile;
  ofile.open("qhulldata.dat.tmp");
  ofile << "3" << endl;  // Dimension for qhull
  ofile << points.size() << endl;  // Number of points for qhull

  // Generate a geometry file out of object
  for (unsigned int i = 0; i < points.size(); i++) {
    // Format is x y z for each point
    ofile << points[i][0] << " " << points[i][1] << " " << points[i][2] << endl;
  }

  ofile.close();
  FILE *fpipe;
  string command = "qhull i Qt  < qhulldata.dat.tmp";
  char line[256];

  // Run qhull command on external pipe. Triangulates points.
  if (!(fpipe = (FILE*) popen(command.c_str(), "r"))) {
    // If fpipe is NULL
    perror("Problems with pipe");
    exit(1);
  }

  std::stringstream ss;
  string tab = "    ";
  // Read line with "number of facets" data
  fgets(line, sizeof line, fpipe);

  // Write triangular vertices to STL-format
  while (fgets(line, sizeof line, fpipe)) {
    vertex v = vertex(string(line));
    ss << "facet normal 0 0 0\n";
    ss << tab << "outer loop\n";
    // Construct the triangular geometry of the surface
    vec v1 = points[v[0]];
    vec v2 = points[v[1]];
    vec v3 = points[v[2]];
    ss << tab << tab << "vertex " << v1[0] << " " << v1[1] << " " << v1[2]
        << "\n";
    ss << tab << tab << "vertex " << v2[0] << " " << v2[1] << " " << v2[2]
        << "\n";
    ss << tab << tab << "vertex " << v3[0] << " " << v3[1] << " " << v3[2]
        << "\n";
    ss << tab << "endloop\n";
    ss << "endfacet\n";
  }

  pclose(fpipe);
  remove("qhulldata.dat.tmp");
  return (ss.str());
}

map<string, AbstractGeometry*> importStl(string& filename) {
  ifstream f(filename.c_str(), std::ios::in);
  string line;
  vector<vertex> verts;
  map<string, AbstractGeometry*> geom;

  if (f.is_open()) {
    while (getline(f, line)) {
      size_t i = line.find("solid ");

      if (i != string::npos) {
        string n = line.substr(i + 6);
        n.erase(remove_if(n.begin(), n.end(), ::isspace), n.end());

        // Find outer loop
        while (getline(f, line)) {
          // outer loop
          size_t j = line.find("outer loop");

          if (j != string::npos) {
            //  vertex v1x v1y v1z
            getline(f, line);
            size_t k = line.find("vertex ");
            vertex v1(line.substr(k + 7));
            verts.push_back(v1);
            // vertex v2x v2y v2z
            getline(f, line);
            k = line.find("vertex ");
            vertex v2(line.substr(k + 7));
            verts.push_back(v2);
            // vertex v3x v3y v3z
            getline(f, line);
            k = line.find("vertex ");
            vertex v3(line.substr(k + 7));
            verts.push_back(v3);
            getline(f, line);  // endloop
            getline(f, line);  // endfacet
          }

          // endsolid
          else if (line.find("endsolid") != string::npos) {
            geom[n] = new SolidPoly(verts);
            geom[n]->objName = n;
            verts.clear();
            break;
          }
        }
      }
    }

    f.close();

  } else {
    cout << "Unable to open stl file" << endl;
    exit(-1);
  }

  return (geom);
}

AbstractGeometry* importSingleStl(string& filename) {
  AbstractGeometry* geom;

  try {
    cout << "Importing STL: BINARY format..." << endl;
    geom = importSingleStlBinary(filename);

  } catch (...) {
    cout << "Switching to ASCII format..." << endl;
    geom = importSingleStlASCII(filename);
  }

  cout << "Imported!" << endl;
  return (geom);
}

AbstractGeometry* importSingleStlASCII(string& filename) {
  ifstream f(filename.c_str(), std::ios::in);
  string line;
  vector<vertex> verts;
  AbstractGeometry* geom;

  if (f.is_open()) {
    while (getline(f, line)) {
      size_t i = line.find("solid ");

      if (i != string::npos) {
        string n = line.substr(i + 6);
        n.erase(remove_if(n.begin(), n.end(), ::isspace), n.end());

        // Find outer loop
        while (getline(f, line)) {
          // outer loop
          size_t j = line.find("outer loop");

          if (j != string::npos) {
            //  vertex v1x v1y v1z
            getline(f, line);
            size_t k = line.find("vertex ");
            vertex v1(line.substr(k + 7));
            verts.push_back(v1);
            // vertex v2x v2y v2z
            getline(f, line);
            k = line.find("vertex ");
            vertex v2(line.substr(k + 7));
            verts.push_back(v2);
            // vertex v3x v3y v3z
            getline(f, line);
            k = line.find("vertex ");
            vertex v3(line.substr(k + 7));
            verts.push_back(v3);
            getline(f, line);  // endloop
            getline(f, line);  // endfacet
          }

          // endsolid
          else if (line.find("endsolid") != string::npos) {
            geom = new SolidPoly(verts);
            verts.clear();
            break;
          }
        }
      }
    }

    f.close();

  } else {
    cout << "Unable to open stl file" << endl;
    exit(-1);
  }

  return (geom);
}

AbstractGeometry* importSingleStlBinary(string& filename) {
  /*
   UINT8[80] � Header
   UINT32 � Number of triangles

   foreach triangle
   REAL32[3] � Normal vector
   REAL32[3] � Vertex 1
   REAL32[3] � Vertex 2
   REAL32[3] � Vertex 3
   UINT16 � Attribute byte count
   end
   */
  ifstream f(filename.c_str(), std::ios::in | std::ios::binary);
  string line;
  vector<vertex> verts;
  AbstractGeometry* geom;
  char* vx = new char[4];
  char* vy = new char[4];
  char* vz = new char[4];
  char* byte_count = new char[2];
  char* header = new char[80];
  char* n_tiangles = new char[4];

  if (f.is_open()) {
    f.read(header, 80);
    cout << string(header) << endl;
    cout << "N_triangles: ";
    f.read(n_tiangles, sizeof(uint32_t));
    uint32_t* triag = (uint32_t*) n_tiangles;
    cout << *triag << endl;

    // Check if filesize matches...
    // This determines if it is binary
    if ((long) filesize(filename.c_str()) != 80 + 4 + *triag * 50) {
      cout << "... not a binary file!" << endl;
      throw(-1);
    }

    while ((long) f.tellg() != -1) {
      // Facet normal, not saved
      f.read(vx, sizeof(uint32_t));
      f.read(vy, sizeof(uint32_t));
      f.read(vz, sizeof(uint32_t));
      //  vertex v1x v1y v1z
      f.read(vx, sizeof(uint32_t));
      f.read(vy, sizeof(uint32_t));
      f.read(vz, sizeof(uint32_t));
      vertex v1;
      v1 << float(*vx) << float(*vy) << float(*vz);
      verts.push_back(v1);
      //  vertex v2x v2y v2z
      f.read(vx, sizeof(uint32_t));
      f.read(vy, sizeof(uint32_t));
      f.read(vz, sizeof(uint32_t));
      vertex v2;
      v2 << float(*vx) << float(*vy) << float(*vz);
      verts.push_back(v2);
      //  vertex v3x v3y v3z
      f.read(vx, sizeof(uint32_t));
      f.read(vy, sizeof(uint32_t));
      f.read(vz, sizeof(uint32_t));
      vertex v3;
      v3 << float(*vx) << float(*vy) << float(*vz);
      verts.push_back(v3);
      // Byte count, not used for anythting
      f.read(byte_count, sizeof(uint16_t));
    }

    f.close();
    geom = new SolidPoly(verts);
    verts.clear();

  } else {
    cout << "Unable to open stl file" << endl;
    exit(-1);
  }

  delete[] vx;
  delete[] vy;
  delete[] vz;
  delete[] byte_count;
  delete[] header;
  delete[] n_tiangles;
  return (geom);
}


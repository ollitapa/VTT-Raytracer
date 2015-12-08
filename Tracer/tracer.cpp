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

/*
 * tracer.cpp
 *
 *  Created on: 13.1.2015
 *      Author: otolli
 */

#include "Common.h"
#include "UtilityFunctions.h"
#include "SurfaceNormalSource.h"
#include "Surface.h"
#include "Isotropic.h"
#include "ClearMaterial.h"
#include "Absorption.h"
#include "Ray.h"
#include "Refraction.h"
#include "SolidCuboid.h"
#include "TracerController.h"
#include "AllDetector.h"
#include "Sphere.h"
#include "InternalSurface.h"
#include "LambertianPointSource.h"
#include "DirectionalPointSource.h"
#include "IsotropicPointSource.h"
#include "Mirror.h"
#include "Lambertian.h"
#include "RandomBulkScatter.h"
#include "SolidCone.h"
#include "IsotropicVolumeSource.h"
#include "OpaqueMaterial.h"
#include "BeerLambertLaw.h"
#include "GeneralScatterer.h"
#include <armadillo>
#include <vector>
#include "json.h"
#include "GeneralFluorescence.h"
#include <fstream>
#include <map>
#include "SimpleFunctions.h"
#include "IsotropicTransparent.h"
#include "IsotropicSurfaceSource.h"
#include "ZemaxRayfileDetector.h"
#include "LightToolsRayfileDetector.h"
#include "IsotropicConicalPointSource.h"

/**
 * Raytracer program.
 *
 * Usage: tracer input.json
 */
int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " inputfile.json" << endl;
    return EXIT_FAILURE;
  }

  Json::Value root;   // will contains the root value after parsing
  Json::Reader reader;
  std::ifstream json_stream(argv[1]);
  bool parsingSuccessful = reader.parse(json_stream, root);

  if (!parsingSuccessful) {
    // report to the user the failure and their locations in the document.
    std::cout << "Failed to parse configuration\n"
        << reader.getFormattedErrorMessages();
    return (-1);
  }

  // Construct materials
  map<string, Material*> materials;
  const Json::Value mater = root["materials"];

  for (unsigned int i = 0; i < mater.size(); ++i) {
    Json::Value item = mater[i];
    string name = item["name"].asString();
    Material* a;

    // Create material based on given type
    if (item["type"] == "Clear") {
      cout << "ClearMat" << endl;
      a = new ClearMaterial(item);

    } else if (item["type"] == "RandomBulkScatter") {
      cout << "RandomBulkScatter" << endl;
      a = new RandomBulkScatter(item);

    } else if (item["type"] == "GeneralScatterer") {
      cout << "GeneralScatterer" << endl;
      a = new GeneralScatterer(item);

    } else if (item["type"] == "GeneralFluorescence") {
      cout << "Generalfluorescence" << endl;
      a = new GeneralFluorescence(item);

    } else if (item["type"] == "Opaque") {
      cout << "Opaque material" << endl;
      a = new OpaqueMaterial(item);

    } else {
      cout << item << endl << "Type not supported!" << endl;
    }

    // Check if any continuously interacting properties are present
    if (item.isMember("continuousInteractors")) {
      const Json::Value contInts = item["continuousInteractors"];

      for (unsigned int j = 0; j < contInts.size(); ++j) {
        Json::Value item2 = contInts[j];
        ContinuousInteraction* actor;

        if (item2["type"] == "BeerLambert") {
          cout << "BeerLambert law used" << endl;
          actor = new BeerLambertLaw(item2, a);
          a->addContinuousInteractor(actor);

        } else {
          cerr << item2 << endl << "Type not supported!" << endl;
        }
      }
    }

    // Add material to map
    materials[name] = a;
    cout << "--- name: " << name << endl;
  }

  cout << endl;
  cout << endl;
  map<string, AbstractGeometry*> geometries;
  geometries.clear();
  // Construct geometry
  const Json::Value geom = root["geometry"];

  for (unsigned int index = 0; index < geom.size(); ++index) {
    Json::Value item = geom[index];
    string name = item["name"].asString();
    AbstractGeometry* a;
    cout << endl;
    cout << "Object #######################" << endl;
    cout << "--- name: " << name << endl;

    // Create an object based on given type
    if (item["type"] == "Cuboid") {
      cout << "Cuboid" << endl;
      a = new SolidCuboid(item);

    } else if (item["type"] == "Sphere") {
      cout << "Sphere" << endl;
      a = new Sphere(item);

    } else if (item["type"] == "RectSurface") {
      cout << "RectSurface" << endl;
      a = new InternalSurface(NULL, item);

    } else if (item["type"] == "Cone") {
      cout << "Cone" << endl;
      a = new SolidCone(item);

    } else if (item["type"] == "ObjectSTL") {
      cout << "SolidPoly object from STL" << endl;
      string fff = item["filepath"].asString();
      a = importSingleStl(fff);

    } else {
      cout << item << endl << "Type not supported!" << endl;
    }

    // Enclose objects
    if (!item.isMember("insideof")) {
      cerr << "Geometry: " << name << " insideOf missing!!" << endl;
      exit(-1);
    }

    string insideof = item["insideof"].asString();

    if (insideof != "None") {
      cout << "--- inside of " << geometries[insideof]->objName << endl;
      dynamic_cast<Solid*>(geometries[insideof])->encloseObject(a);
    }

    // Surface Properties
    if (item.isMember("surfaceProperties")) {
      const Json::Value prop = item["surfaceProperties"];

      for (unsigned int j = 0; j < prop.size(); ++j) {
        Json::Value p = prop[j];
        SurfaceProperty* property;

        if (p["type"] == "Absorption") {
          cout << "--- property Absorption" << endl;
          property = new Absorption(p);

        } else if (p["type"] == "Lambertian") {
          cout << "--- property Lambertian" << endl;
          property = new Lambertian(p);

        } else if (p["type"] == "Isotropic") {
          cout << "--- property Isotropic" << endl;
          property = new Isotropic(p);

        } else if (p["type"] == "IsotropicTransparent") {
          cout << "--- property IsotropicTransparent" << endl;
          property = new IsotropicTransparent(p);

        } else if (p["type"] == "Mirror") {
          cout << "--- property Mirror" << endl;
          property = new Mirror(p);

        } else {
          cout << item << endl << "Property type not supported!" << endl;
          exit(-1);
        }

        if (dynamic_cast<Surface*>(a)) {
          dynamic_cast<Surface*>(a)->addProperty(property);

        } else {
          // Check if property is going to be attached to all or just
          // a list of specific surfaces.
          if (p.isMember("attachedTo")) {
            const Json::Value nu = p["attachedTo"];

            for (unsigned int kk = 0; kk < nu.size(); ++kk) {
              dynamic_cast<Solid*>(a)->addPropertyToSurface(property,
                                                            nu[kk].asUInt());
              cout << "----- attached to surf " << nu[kk].asString() << endl;
            }

          } else {
            dynamic_cast<Solid*>(a)->addPropertyToSurfaces(property);
            cout << "----- attached to all surfaces" << endl;
          }
        }
      }
    }

    cout << endl;

    // Detector Properties
    if (item.isMember("detectors")) {
      const Json::Value prop = item["detectors"];

      for (unsigned int j = 0; j < prop.size(); ++j) {
        Json::Value p = prop[j];
        SurfaceProperty* property;

        if (p["type"] == "AllDetector") {
          cout << "--- detector AllDetector" << endl;
          property = new AllDetector(p);

        } else if (p["type"] == "ZemaxRayfile") {
          cout << "--- detector Zemax rayfile" << endl;
          property = new ZemaxRayfileDetector(p);

        } else if (p["type"] == "LightToolsRayfile") {
          cout << "--- detector LightTools rayfile" << endl;
          property = new LightToolsRayfileDetector(p);

        } else {
          cout << item << endl << "Detector type not supported!" << endl;
          exit(-1);
        }

        if (dynamic_cast<Surface*>(a)) {
          dynamic_cast<Surface*>(a)->addProperty(property);

        } else {
          // Check if property is going to be attached to all or just
          // a list of specific surfaces.
          if (p.isMember("attachedTo")) {
            const Json::Value nu = p["attachedTo"];

            for (unsigned int kk = 0; kk < nu.size(); ++kk) {
              dynamic_cast<Solid*>(a)->addPropertyToSurface(property,
                                                            nu[kk].asUInt());
              cout << "----- attached to surf " << nu[kk].asString() << endl;
            }

          } else {
            dynamic_cast<Solid*>(a)->addPropertyToSurfaces(property);
            cout << "----- attached to all surfaces" << endl;
          }
        }
      }
    }

    // Add material to object
    if (item.isMember("material"))
      a->material = materials[item["material"].asString()];

    a->objName = name;
    // Add object to geometries array.
    geometries[name] = a;
  }

  cout << endl;
  cout << endl;
  vector<Source*> listOfSources;
  // Construct Sources
  const Json::Value sources = root["sources"];

  for (unsigned int index = 0; index < sources.size(); ++index) {
    Json::Value item = sources[index];
    string name = item["name"].asString();
    Source* a;
    cout << endl;
    cout << "Source #######################" << endl;

    // Enclosing object
    if (!item.isMember("insideOf")) {
      cerr << "Source: " << name << " insideOf missing!!" << endl;
      exit(-1);
    }

    string insideof = item["insideOf"].asString();

    // Create an object based on given type
    if (item["type"] == "DirectionalPoint") {
      cout << "DirectionalPoint source" << endl;
      a = new DirectionalPointSource(
          item, dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "LambertianPoint") {
      cout << "Lambertian Point source" << endl;
      a = new LambertianPointSource(item,
                                    dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "IsotropicPoint") {
      cout << "Isotropic Point source" << endl;
      a = new IsotropicPointSource(item,
                                   dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "IsotropicConicalPointSource") {
      cout << "Isotropic Conical Point source" << endl;
      a = new IsotropicConicalPointSource(
          item, dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "SurfaceNormal") {
      cout << "Surface normal source" << endl;
      a = new SurfaceNormalSource(item,
                                  dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "IsotropicSurface") {
      cout << "Isotropic surface source" << endl;
      a = new IsotropicSurfaceSource(
          item, dynamic_cast<Solid*>(geometries[insideof]));

    } else if (item["type"] == "IsotropicVolume") {
      cout << "Isotropic volume source" << endl;
      a = new IsotropicVolumeSource(item,
                                    dynamic_cast<Solid*>(geometries[insideof]));

    } else {
      cout << item << endl << "Type not supported!" << endl;
    }

    // Add object to array.
    listOfSources.push_back(a);
    cout << "--- " << name << endl;
  }

  cout << "###################" << endl;
  cout << endl;
  cout << endl;
  // Export geometry to STL
  vector<AbstractGeometry*> objList;

  for (map<string, AbstractGeometry*>::iterator it = geometries.begin();
      it != geometries.end(); ++it) {
    objList.push_back(it->second);
  }

  // Export geometry
  exportStl(objList);
  // Common surfaces find common surfaces of solid objects
  cout << "Finding common surfaces with adjacent objects..." << endl;

  for (unsigned int i = 0; i < objList.size(); ++i) {
    for (unsigned int j = 0; j < objList.size(); ++j) {
      if (!dynamic_cast<Solid*>(objList[i])
          || !dynamic_cast<Solid*>(objList[j]))
        continue;

      if (objList[i] == objList[j])
        continue;

      vector<Surface*> common = findCommonSurfaces(objList[i], objList[j]);

      for (unsigned int ii = 0; ii < common.size(); ii += 2) {
        common[ii]->addNeighbour(common[ii + 1]);
#ifdef DEBUG
        cout << common[ii]->parent->objName <<
        " is adjacent to " <<
        common[ii + 1]->parent->objName << endl;
#endif
      }
    }
  }

  cout << "... done." << endl;

  if (root["general"].isMember("DoNotTrace")) {
    cout << "DoNotTrace is set! Exiting..." << endl;
    exit(0);
  }

  // Tracer /////////////////////////////////////////////////
  cout << "########## Setting up Tracer ################" << endl;
  TracerController tracer(root["general"]);

  // Add sources
  for (unsigned int i = 0; i < listOfSources.size(); ++i) {
    tracer.addSource(listOfSources[i]);
  }

  // Trace stuff
  tracer.startTracing();

  // Let material know that tracing ended
  for (unsigned int i = 0; i < objList.size(); ++i) {
    objList[i]->tracerDidEndTracing();
    objList[i]->material->tracerDidEndTracing();
  }

  AbsorptionSaver::tracerDidEndTracing();
  cout << "########## Tracing done ################" << endl;
  return (0);
}

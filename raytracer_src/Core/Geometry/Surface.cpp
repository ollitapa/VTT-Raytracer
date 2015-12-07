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

#include "Surface.h"

Surface::Surface(AbstractGeometry* theParent) {
    _oblivion = new OblivionSurface();
    parent = theParent;
    // Spanning vectors
    vertices = vector<vertex>();
    _properties = vector<SurfaceProperty*>();
    _refr = new Refraction();
    _properties.push_back(_refr);
    _surf_prop_hierarchy = 0;
}

Surface::~Surface() {
    delete _oblivion;
}

bool Surface::rayIntersects(Ray &ray, double& atDistance,
                            AbstractGeometry* fromObject) {
    // Should never reach here...
    cerr << "Abstract surface rayIntersects!!!" << endl;
    return (false);
}

AbstractGeometry* Surface::propagateRay(Ray &ray,
                                        AbstractGeometry *fromObject) {
#ifdef DEBUG_VERBOSE
    cout << "Surface Prop by " << objName << "########################" << endl;
#endif
#ifdef DEBUG

    if (dynamic_cast<Surface*>(fromObject)) {
        cout << "Surface to surface propagation not allowed!" << endl;
        cout << "exiting!" << endl;
        exit(-1);
    }

#endif
    AbstractGeometry* toObject;
    ray.previousObjects.clear();

    // Check if ray is going to out of this surfaces parent object
    if (fromObject == parent) {
        if (_neighbours.size() > 0) {
#ifdef DEBUG_VERBOSE
            cout << "------- Neighbours: " << endl;
#endif
            // Find actual surfaces that intersect the ray.
            vector<Surface*> coSurf;

            for (vector<Surface*>::iterator it = _neighbours.begin();
                    it != _neighbours.end(); ++it) {
#ifdef DEBUG_VERBOSE
                cout << "--------- " << (*it)->objName << endl;
#endif
                double distance = 0;
                Ray rr = ray;
                rr.location = ray.location - ray.direction;
                rr.length = 2.0;

                if ((*it)->rayIntersects(rr, distance, this)) {
                    coSurf.push_back(*it);
                }
            }

            Surface* handlerSurf = this;
            double handlerID = parent->id();
            AbstractGeometry* intendedObj;
            Surface* receiverSurf =  this;
            double recID = -10;

            if (coSurf.size() > 0) {
                handlerSurf = coSurf[0];
                handlerID = coSurf[0]->parent->id();

                // find most senior object
                for (vector<Surface*>::iterator it = coSurf.begin();
                        it != coSurf.end(); ++it) {
                    // TODO This is using object creation as a rule.
                    if ((*it)->parent->id() > handlerID) {
                        handlerID = (*it)->parent->id();
                        handlerSurf = *it;
                    }
                }

                // Chekc if this surface is even more senior
                if (parent->id() > handlerID)
                    handlerSurf = this;

                // find most senior to-object that can receive the ray
                for (vector<Surface*>::iterator it = coSurf.begin();
                        it != coSurf.end(); ++it) {
                    // TODO This is using object creation as a rule.
                    if ((*it)->parent->id() > recID &&
                            arma::dot((*it)->normal(ray.location),
                                      ray.direction) < 0) {
                        recID = (*it)->parent->id();
                        receiverSurf = *it;
                    }
                }

                if (receiverSurf == this) {
                    recID = parent->id();

                    for (vector<Surface*>::iterator it = coSurf.begin();
                            it != coSurf.end(); ++it) {
                        if ((*it)->parent->id() < recID) {
                            intendedObj = (*it)->parent->parent;
                            receiverSurf = *it;
                            recID = (*it)->parent->id();
                        }
                    }

                    ray.previousObjects[intendedObj->id()] =  receiverSurf;

                } else {
                    intendedObj = receiverSurf->parent;
                    ray.previousObjects[intendedObj->id()] =  receiverSurf;
                }

            } else {
                receiverSurf = this;
                handlerSurf = this;
                intendedObj = parent->parent;
                ray.previousObjects[intendedObj->id()] =  receiverSurf;
                ray.previousObjects[parent->id()] = this;
            }

#ifdef DEBUG_VERBOSE
            cout << "handlerSurf " << handlerSurf->objName << " (" <<
                 handlerSurf->parent->objName << ")" << endl;
            cout << "receiverSurf " << receiverSurf->objName << " (" <<
                 receiverSurf->parent->objName << ")" << endl;
#endif
            // Add previous surfaces to parent and from objects.
            ray.previousObjects[fromObject->id()] = this;
            ray.previousObjects[parent->id()] = this;
            // Handle ray receiving
            toObject = handlerSurf->receiveRay(ray, fromObject, intendedObj);

        } else {
            // Add previous surfaces to parent objects.
            ray.previousObjects[parent->id()] = this;
            ray.previousObjects[parent->parent->id()] = this;
            // Ray is going out of parent, but no surface is receiving
            // e.g. parent is totally enclosed object.
            toObject = receiveRay(ray, parent, parent->parent);
        }

    } else {
        ray.previousObjects[fromObject->id()] = this;
        ray.previousObjects[parent->id()] = this;
        // This happens if this surface is receiving surface
        toObject = receiveRay(ray, fromObject, parent);
    }

#ifdef DEBUG_VERBOSE
    cout << "SurfProp by " << objName << ": "  <<  fromObject->objName << " -> " <<
         toObject->objName << endl;
    cout << "ray loc: " << ray.location << "ray dir: " << ray.direction << endl;
#endif
    return (toObject);
}


AbstractGeometry* Surface::receiveRay(Ray& ray,
                                      AbstractGeometry* from, AbstractGeometry* to) {
    // Give each surface property the chance to handle the ray.
    AbstractGeometry* toTemp = to;
    AbstractGeometry* toBeTemp = to;

    for (vector<SurfaceProperty*>::iterator it = _properties.begin();
            it != _properties.end();
            ++it) {
        toBeTemp = (*it)->receiveRay(ray, this, from, toTemp);

        if (dynamic_cast<Solid*>(toBeTemp))
            toTemp = toBeTemp;
    }

    return (toTemp);
}

void Surface::addNeighbour(Surface* newNeighbour) {
    // Check that neighbour is not already listed.
    if (find(_neighbours.begin(), _neighbours.end(),
             newNeighbour) == _neighbours.end()) {
        cout << "Set neighbor: " << newNeighbour->objName << " ("
             << newNeighbour->parent->objName << ") for "
             << objName << " (" << parent->objName << ")" << endl;
        _neighbours.push_back(newNeighbour);
        newNeighbour->addNeighbour(this);
    }
}

vertex& Surface::normal(const vertex& distance) {
    return _normal;
}

const vertex& Surface::p() const {
    return _p;
}

void Surface::addProperty(SurfaceProperty* property) {
    vector<int> toErase;
    // Go through every property in attached properties
    // and remove all that do not cooperate with this property.
    int i = 0;

    for (vector<SurfaceProperty*>::iterator it = _properties.begin();
            it != _properties.end();
            ++it) {
        if ((*it)->cooperationLevel() != property->cooperationLevel() &&
                (*it)->cooperationLevel() != -1) {
            toErase.push_back(i);
        }

        i++;
    }

    for (vector<int>::iterator it = toErase.begin();
            it != toErase.end();
            ++it) {
        _properties.erase(_properties.begin() + *it);
    }

    if (property->hierarchy() > _surf_prop_hierarchy)
        _surf_prop_hierarchy = property->hierarchy();

    _properties.push_back(property);
}

void Surface::tracerDidEndTracing() {
    for (vector<SurfaceProperty*>::iterator it = _properties.begin();
            it != _properties.end();
            ++it) {
        (*it)->tracerDidEndTracing();
    }
}

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

#include "GeneralScatterer.h"

GeneralScatterer::GeneralScatterer(Json::Value jsonData) {
  // Check for required keys
  checkRequiredJSONParam(jsonData, "refractiveIndex", this);
  checkRequiredJSONParam(jsonData, "distributionData", this);
  checkRequiredJSONParam(jsonData, "particleDensities", this);
  // Check for required files
  fileExists(jsonData["distributionData"].asString(), this);
  refractiveIndex = jsonData["refractiveIndex"].asDouble();
  Json::Value densities = jsonData["particleDensities"];

  for (unsigned int i = 0; i < densities.size(); ++i) {
    rho.push_back(densities[i].asDouble());
  }

  // Try to load the scatter data tables
  /* Open an existing file. */
  cout << "Loading: " << jsonData["distributionData"].asCString() << endl;
  hid_t file_id = H5Fopen(jsonData["distributionData"].asCString(),
                          H5F_ACC_RDWR, H5P_DEFAULT);
  cout << "File open... " << endl;
  // Load wavelengths used
  cout << "Loading wavelengths... " << endl;
  hid_t dset = H5Dopen2(file_id, "wavelengths", H5P_DEFAULT);
  hid_t dataspace = H5Dget_space(dset);
  unsigned int ndims = H5Sget_simple_extent_ndims(dataspace);
  hsize_t dsetDims[ndims];
  H5Sget_simple_extent_dims(dataspace, dsetDims, NULL);
  double buf4[dsetDims[0]];
  H5Dread(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf4);
  // Save wavelengths to array
  wavelenghts_0 = new vec(buf4, dsetDims[0]);  // In micrometers
  // *wavelenghts_0 = 1000.0 * (*wavelenghts_0) ; // In nanometers
  // Read particle IDs
  cout << "Loading particleIDs... " << endl;
  hid_t dataset_id = H5Dopen2(file_id, "/particleID", H5P_DEFAULT);
  dataspace = H5Dget_space(dataset_id);
  ndims = H5Sget_simple_extent_ndims(dataspace);
  hsize_t dims[ndims];
  H5Sget_simple_extent_dims(dataspace, dims, NULL);
  long long int buf[dims[0]];
  H5Dread(dataset_id, H5T_NATIVE_INT64_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
  // Save the IDs to a vector
  arma::ivec v(buf, dims[0]);
  cout << "particle count: " << dims[0] << endl;
  // Now load data for each particle id
  cout << "Loading particleData... " << endl;
  hid_t group = H5Gopen2(file_id, "particleData", H5P_DEFAULT);

  for (unsigned int i = 0; i < v.size(); ++i) {
    stringstream ss;
    ss << v[i];
    string idx = ss.str();
    cout << "Loading particle id: " << idx << endl;
    cout << "Inverse CDF..." << endl;
    hid_t p_group = H5Gopen2(group, idx.c_str(), H5P_DEFAULT);
    hid_t invCDF = H5Dopen2(p_group, "inverseCDF", H5P_DEFAULT);
    dataspace = H5Dget_space(invCDF);
    ndims = H5Sget_simple_extent_ndims(dataspace);
    hsize_t dimsCDF[ndims];
    H5Sget_simple_extent_dims(dataspace, dimsCDF, NULL);

    if (ndims != 2) {
      cerr << "wrong dimensions in inverseCDF array in HDF5!" << endl;
      exit(-1);
    }

    double* buf2 = new double[dimsCDF[0] * dimsCDF[1]];
    //double buf2[dimsCDF[0]*dimsCDF[1]];
    H5Dread(invCDF, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf2);
    mat* m = new arma::mat(buf2, dimsCDF[0], dimsCDF[1]);
    inplace_trans(*m);
    _inverseThetaCDF.push_back(m);
    delete buf2;
    cout << "Cross sections..." << endl;
    hid_t crossS = H5Dopen2(p_group, "crossSections", H5P_DEFAULT);
    dataspace = H5Dget_space(crossS);
    ndims = H5Sget_simple_extent_ndims(dataspace);
    hsize_t crossSDims[ndims];
    H5Sget_simple_extent_dims(dataspace, crossSDims, NULL);

    if (ndims != 1) {
      cerr << "wrong dimensions in crossSections array in HDF5!" << endl;
      exit(-1);
    }

    double buf3[crossSDims[0]];
    H5Dread(crossS, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf3);
    scatteringCrossSections.push_back(new vec(buf3, crossSDims[0]));
  }

  cout << "Loading done!" << endl;
  /* Close the file. */
  H5Fclose(file_id);
  //Initialize!!
  _initializeScatterer();
}

void GeneralScatterer::_initializeScatterer() {
  // For every system wavelength, calculate the Mean Free Path
  // and store it into MFP vector.
  for (unsigned int i = 0; i < wavelenghts_0->size(); i++) {
    // Calculate the Mean Free Path for the particle
    // mixture for the wavelength lambda.
    // The resulting mfp will be in MICROMETERS
    double alpha = 0.0;

    for (unsigned int j = 0; j < rho.size(); j++) {
      // rho  corresponding to particle type j.
      // cross section corresponding to particle type j and wavelength i
      alpha += rho[j] * scatteringCrossSections[j]->at(i);
    }

    // Calculate the mean free path for the wavelength lambda
    double mfp = 1.0 / alpha;
    // cout << mfp << endl;
    // Convert the mean free path to METERS
    // (this is the unit we use generally at the moment)
    mfp *= um;
    // Store the mean free path for the wavelength lambda (index i)
    meanFreePaths.push_back(mfp);
    // For this wavelength, allocate a vec containing
    // the hit probabilities to different particle types
    vec* probs = new vec(rho.size());

    // Calculate the probability factors for hitting different
    // particle types when the wavelength is lambda.
    for (unsigned int j = 0; j < rho.size(); j++) {
      // Save the separate particle type
      // hit probabilities @ lambda_ind into
      // probs vector
      probs->at(j) = rho[j] * scatteringCrossSections[j]->at(i) / alpha;
    }

    // push the newly calculated hit probability
    // vector into vector container PROB.
    // These vectors will be used as a cumulative distribution
    // function when the monte-carlo selection is made to which
    // particle type the ray hits.
    probabilityOfHittingParticleType.push_back(probs);
  }

  _numberOfParticles = rho.size();
  _numberOfThetaAngles = _inverseThetaCDF[0]->n_rows;
}

GeneralScatterer::~GeneralScatterer() {
  // Lazy coding, no proper freeing of *
}

void GeneralScatterer::interactWithRay(Ray& ray) {
  // Determine particle type
  unsigned int ptype = _randomParticleType(ray);
  // Scatter according to that particle type.
  scatter(ray, ptype);
}

void GeneralScatterer::propagationDistance(Ray& ray) {
  // Calculate the vector index corresponding to the
  // current wavelength of the ray.
  unsigned int lambda_ind = findClosestIndex(wavelenghts_0, ray.wavelength_0);
  // Determine the ray length by using previously
  // calculated (see Constructor!) Mean Free Path
  // (MFP) table
  // NOTE!!!
  // When you parallelize this, use thread-safe
  // random number generator rand_r with the seed
  // stored into ray.
  ray.length = -meanFreePaths[lambda_ind] * log(rnd1());
}

void GeneralScatterer::scatter(Ray& ray, unsigned int particleType) {
  double tht;
  double phi;
  int th_ind;
  // At this point we should know the particle type
  // to which the ray hits (CurrentPtype). This was
  // determined by the DetermineParticleType -function.
  // Or in the case of Phosphor, it was determined before this
  // function call.
  // Set now the correct Mie-scattering table that
  // corresponds to this particle type.
  mat* iCDF = _inverseThetaCDF[particleType];
  // Generate a random number between 0... NANG-1 and turn
  // that into integer index variable. Then use this index
  // variable to pick the correct angle-value from the
  // MIE_PPDF.dat tables.
  th_ind = rndrangeInt(0ull, _numberOfThetaAngles - 1);
  // Calculate the theta and phi values corresponding
  // to the current direction vector of the ray
  double th = acos(ray.direction[2]);         // arccos(cos(theta))
  double ph = atan2(ray.direction[1], ray.direction[0]);  // arctan(y/x)
  // Rotate the coordinate system so that the ray
  // direction vector is aligned with Z-axis
  ray.direction = RotateZ(ray.direction, -ph);
  ray.direction = RotateY(ray.direction, -th);
  // ---------- DO MIE SCATTERING -----------------------
  // Use the th_ind to pick the corresponding theta angle
  // from the MIE_PDF -matrix (each column corresponds to
  // different wavelength and each row value gives the
  // theta-angle).
  // Convert the obtained theta-value also into radians.
  // Phi-angle is chosen randomly and it is between [0,2*PI]
  // Calculate the vector index corresponding to the
  // current wavelength of the ray.
  unsigned int lambda_ind = findClosestIndex(wavelenghts_0, ray.wavelength_0);
  tht = M_PI * iCDF->at(th_ind, lambda_ind) / 180.0;
  phi = rnd1() * 2.0 * M_PI;
  // ----------------------------------------------------
  // Set the new direction for the ray
  ray.direction[0] = sin(tht) * cos(phi);
  ray.direction[1] = sin(tht) * sin(phi);
  ray.direction[2] = cos(tht);
  // Rotate the coordinate system back to
  // where it used to be before scattering
  ray.direction = RotateY(ray.direction, th);
  ray.direction = RotateZ(ray.direction, ph);
}

unsigned int GeneralScatterer::_randomParticleType(Ray& ray) {
  double CumulProb = 0.0;
  // Take a random number
  // NOTE!!!
  // When you parallelize this, use thread safe
  // random number generator rand_r with seed address stored
  // into ray.
  double rndnumber = rnd1();

  // Now go through the different particle types
  // and determine which particle type the obtained
  // random number corresponds to.
  for (unsigned int i = 0; i < rho.size(); i++) {
    // Calculate the vector index corresponding to the
    // current wavelength of the ray.
    unsigned int lambda_ind = findClosestIndex(wavelenghts_0, ray.wavelength_0);
    // Pick the hit probability corresponding
    // to current wavelength
    CumulProb += probabilityOfHittingParticleType[lambda_ind]->at(i);

    // If the propability value rndnumber is within
    // the current CumulProb value (with cumulative
    // sum = 1 if summed over all the particle types)
    // then the current particle type is i
    if (rndnumber <= CumulProb) {
      // If the probability is within the
      // range corresponding to index i
      return (i);
    }
  }

  return (0);
}

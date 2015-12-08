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

#include "IrradianceDetector.h"

// Init count
int IrradianceDetector::count = 0;

IrradianceDetector::IrradianceDetector(Surface* detectorObject, int XPixels,
                                       int YPixels, int N_RAYS) {
  // TODO Auto-generated constructor stub
  count++;
  // Initialize the total flux to zero
  TotalFlux = 0.0;
  // Set the pixel count
  XPix = XPixels;
  YPix = YPixels;
  uvec = detectorObject->u() / norm(detectorObject->u());
  vvec = detectorObject->v() / norm(detectorObject->v());
  pos = detectorObject->p();
  RS_width = norm(detectorObject->u());
  RS_height = norm(detectorObject->v());
  pos += (RS_width / 2.0 * uvec + RS_height / 2.0 * vvec);
  // Initialize step-variables
  XSTEP = RS_width / XPix;
  YSTEP = RS_height / YPix;
  // matrix for the Flux Data
  FluxData.resize(XPix, YPix);
  FluxData.zeros();
  // Set the total ray count
  TotalNumberOfRays = N_RAYS;
  // Resize the detected rays detector to contain zero element
  DetectedRays.resize(0);
  cout << "IrradianceDetector constructed" << endl;
}

IrradianceDetector::~IrradianceDetector() {
}

AbstractGeometry* IrradianceDetector::receiveRay(Ray& ray, Surface* surface,
                                                 AbstractGeometry* from,
                                                 AbstractGeometry* to) {
  RecordRayData(ray);
  return (to);
}

void IrradianceDetector::RecordRayData(Ray& ray) {
  // Do data recording here
  DetectedRays.push_back(ray);
}

void IrradianceDetector::Irradiance(string& DetectorDataFileName,
                                    int NUM_THREADS) {
  // Calculate the Flux/M^2 data
  ConstructIrradianceData(NUM_THREADS);
  double Percentage = (double) DetectedRays.size() / (double) TotalNumberOfRays;
  cout << endl;
  cout << "----------- DETECTOR " << DetectorDataFileName
      << " INFO ------------" << endl;
  cout << "# Total Flux = " << TotalFlux << endl;
  cout << "# Detected Rays = " << DetectedRays.size() << endl;
  cout << "# Total Number Of Rays = " << TotalNumberOfRays << endl;
  cout << "# Percentage Of Rays Hit = " << Percentage << endl;
  cout << endl;
  ofstream ofile;
  ofstream ofile2;
  string gnup = ".gp";
  string gnufile = DetectorDataFileName + gnup;
  // Write Regular file
  ofile.open(DetectorDataFileName.c_str());
  ofile << fixed << setprecision(10);
  ofile << "# Total Flux = " << TotalFlux << endl;
  ofile << "# Detected Rays = " << DetectedRays.size() << endl;
  ofile << "# Total Number Of Rays = " << TotalNumberOfRays << endl;
  ofile << "# Percentage Of Rays Hit = " << Percentage << endl;
  ofile << endl << endl;

  for (int i = 0; i <= XPix - 1; i++) {
    for (int j = 0; j <= YPix - 1; j++) {
      ofile << FluxData(i, j) << "   ";
    }

    ofile << endl;
  }

  ofile.close();
  // Write Gnuplot PM3D MAP-loader file
  ofile2.open(gnufile.c_str());
  ofile2 << "unset key" << endl;
  ofile2 << "set view map" << endl;
  ofile2 << "set xtics " << RS_width / XPix
      << " border in scale 0,0.001 nomirror norotate offset character 0, 0, 0 autojustify"
      << endl;
  ofile2 << "set ytics " << RS_height / YPix
      << " border in scale 0,0.001 mirror norotate offset character 0, 0, 0 autojustify"
      << endl;
  ofile2 << "set xtics format \"\"" << endl;
  ofile2 << "set ytics format \"\"" << endl;
  ofile2
      << "set ztics border in scale 0,0 nomirror norotate offset character 0, 0, 0 autojustify"
      << endl;
  ofile2
      << "set rtics axis in scale 0,0 nomirror norotate offset character 0, 0, 0 autojustify"
      << endl;
  ofile2 << "set title \"IRRADIANCE [W/M^2]\\n" << "Detected Rays:"
      << DetectedRays.size() << "(" << TotalNumberOfRays << ")\\n"
      << "Total Power: " << TotalFlux << " W\\n" << "Peak Irradiance:"
      << max(max(FluxData)) << " W/M^2\" " << "offset 0,3" << endl;
  ofile2 << "set xrange [" << -RS_width / 2.0 << ":" << RS_width / 2.0
      << "] noreverse nowriteback" << endl;
  ofile2 << "set yrange [" << -RS_height / 2.0 << ":" << RS_height / 2.0
      << "] noreverse nowriteback" << endl;
  ofile2 << "set xlabel \"DETECTOR X COORDINATE [Meters]\"" << endl;
  ofile2 << "set ylabel \"DETECTOR Y COORDINATE [Meters]\"" << endl;
  ofile2 << "set cbtics format \"%12.3f\"" << endl;
  ofile2 << "set cbtics scale 0" << endl;
  ofile2 << "set mxtics 2" << endl;
  ofile2 << "set mytics 2" << endl;
  ofile2 << "set style line 13 lc rgb 'white' lt 1 lw 0.5" << endl;

  if (XPix % 2 != 0)
    ofile2 << "set grid noxtics noytics mxtics mytics ls 13, ls 13" << endl;

  else
    ofile2 << "set grid xtics ytics nomxtics nomytics ls 13, ls 13" << endl;

  ofile2 << "set palette defined ( 0 'blue', 3 'green', 6 'yellow', 10 'red' )"
      << endl;
  ofile2 << "set size square" << endl;
  ofile2 << "splot '" << DetectorDataFileName << "' u (($1-" << (XPix - 1) / 2.0
      << ")/" << XPix / RS_width << "):(-($2-" << (YPix - 1.0) / 2.0 << ")/"
      << YPix / RS_height << "):3 matrix with image" << endl;
  ofile2.close();
}

void IrradianceDetector::IrradianceCrossSectionVertical(
    int ColNumber, string& CrossSectionDataFileName) {
  ofstream ofile;
  ofile.open(CrossSectionDataFileName.c_str());
  ofile << fixed << setprecision(5);
  ofile << setw(12) << fixed << setprecision(10) << -RS_height / 2.0 << "     "
      << FluxData(YPix - 1, ColNumber) << endl;

  for (int j = 0; j <= YPix - 1; j++) {
    ofile << setw(12) << fixed << setprecision(10)
        << -RS_height / 2.0 + YSTEP / 2.0 + j * YSTEP << "     "
        << FluxData(YPix - 1 - j, ColNumber) << endl;
  }

  ofile << setw(12) << fixed << setprecision(10) << RS_height / 2.0 << "     "
      << FluxData(0, ColNumber) << endl;
  ofile.close();
}

void IrradianceDetector::IrradianceCrossSectionHorizontal(
    int RowNumber, string& CrossSectionDataFileName) {
  ofstream ofile;
  ofile.open(CrossSectionDataFileName.c_str());
  ofile << fixed << setprecision(5);
  ofile << setw(12) << fixed << setprecision(10) << -RS_width / 2.0 << "     "
      << FluxData(RowNumber, 0) << endl;

  for (int j = 0; j <= XPix - 1; j++) {
    ofile << setw(12) << fixed << setprecision(10)
        << -RS_width / 2.0 + XSTEP / 2.0 + j * XSTEP << "     "
        << FluxData(RowNumber, j) << endl;
  }

  ofile << setw(12) << fixed << setprecision(10) << RS_width / 2.0 << "     "
      << FluxData(RowNumber, XPix - 1) << endl;
  ofile.close();
}

void IrradianceDetector::ConstructIrradianceData(int NUM_THREADS) {
  int upperlimit = DetectedRays.size() - 1;
  concurrent_vector<double> FDATA(XPix * YPix);

  for (int i = 0; i <= YPix - 1; i++)
    for (int j = 0; j <= XPix - 1; j++)
      FDATA[j + i * XPix] = 0.0;

  double Fluxval = 0.0;

  if (DetectedRays.size() > 0) {
#pragma omp parallel shared(FDATA,Fluxval) num_threads(NUM_THREADS)
    {
      int i;
      mat A;
      A.insert_cols(0, uvec);
      A.insert_cols(1, vvec);

#pragma omp for reduction(+:Fluxval) private(i)

      for (i = 0; i <= upperlimit; i++) {
        // Upper Left corner of the detector
        vec CornerPosition = pos - RS_width / 2 * uvec + RS_height / 2 * vvec;
        vec b = DetectedRays[i].location - CornerPosition;
        vec X(2);
        bool result = solve(X, A, b);
        int xindex = floor(fabs(X(0)) / XSTEP);  // pixel position in x-direction
        int yindex = floor(fabs(X(1)) / YSTEP);  // pixel position in y-direction
        // Calculate Flux/M^2 and insert it into position (yindex,xindex)
        FDATA[xindex + yindex * XPix] += DetectedRays[i].radiantPower()
            / (XSTEP * YSTEP);
        Fluxval += DetectedRays[i].radiantPower();
      }  // PARALLEL FOR

    }  // PARALLEL SECTION
    TotalFlux = Fluxval;

    for (int i = 0; i <= YPix - 1; i++)  // rows
      for (int j = 0; j <= XPix - 1; j++)  // cols
        FluxData(i, j) = FDATA[j + i * XPix];
  }  // IF
}

void IrradianceDetector::Spectrum(string& SpectrumDataFileName) {
  double lambda_i = 0.38;         // Initial wavelength
  double lambda_f = 0.80;         // Final wavelength
  int NN = 101;                   // Wavelength sampling
  // Wavelength step size
  double deltal = (lambda_f - lambda_i) / (NN - 1);
  double l;
  double FL;
  double sumT = 0.0;
  double lambdaw;
  double lambdac;
  vec spectrumT(NN);
  int i, j;
  i = j = 0;

  for (l = lambda_i; l <= lambda_f; l += deltal) {
    lambdaw = deltal;
    lambdac = l;
    sumT = 0.0;
    // Parallelized section begins here
#pragma omp parallel default(shared) private(i)
    {
      // Parallelized for-loop starts here
#pragma omp for reduction(+:sumT)
      for (i = 0; i <= DetectedRays.size() - 1; i++) {
        if (DetectedRays[i].wavelength >= (lambdac - lambdaw / 2.0)
            && DetectedRays[i].wavelength <= (lambdac + lambdaw / 2.0)) {
          // Spectrum for the transmitted flux
          sumT += DetectedRays[i].radiantPower();
        }
      }
    }
    spectrumT(j) = sumT;
    j++;
  }

  spectrumT /= max(spectrumT);
  // Open a file for the output spectrum data
  ofstream ofile;
  ofile.open(SpectrumDataFileName.c_str());

  for (j = 0; j <= NN - 1; j++) {
    l = lambda_i + j * deltal;
    // Print out lambda vs. transmitted flux
    ofile << l << "    " << spectrumT(j) << endl;
  }

  ofile.close();
  cout << "Spectrum File Written!" << endl;
}


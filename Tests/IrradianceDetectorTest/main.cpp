/*
 * main.cpp
 *
 *  Created on: 15.9.2014
 *      Author: otolli
 */




/*
 * main.cpp
 *
 *  Created on: 20.8.2014
 *      Author: otolli
 */

#include "Common.h"
#include "UtilityFunctions.h"
#include "SurfaceNormalSource.h"
#include "Surface.h"
#include "Isotropic.h"
#include "MieBulkScatter.h"
#include "MiePhosphor.h"
#include "ClearMaterial.h"
#include "Absorption.h"
#include "Ray.h"
#include "Refraction.h"
#include <armadillo>
#include "SolidCuboid.h"
#include "TracerController.h"
#include <vector>
#include "AllDetector.h"
#include "Sphere.h"
#include "Triangle.h"
#include "InternalSurface.h"
#include "LambertianPointSource.h"
#include "DirectionalPointSource.h"
#include "Mirror.h"
#include "IrradianceDetector.h"
#include "RandomBulkScatter.h"

/**
 * Test program to test raytracing
 */

int main(int argc, char** argv)
{
	// The number of rays to be traced
	double N_RAYS = std::atof(argv[1]);
	cout << "N_RAYS = " << N_RAYS << endl;

	// ---------------------------------- CONSTRUCT THE GEOMETRY OF THE SYSTEM -------------------------------

	// ---------------------------------------------- Air Container ------------------------------------------
	vertex pos2;
	pos2 << 0 << 0 << 0;
	Sphere* air 		= new Sphere(pos2,10000.0);	// 200 meters radius
	air->material		= new ClearMaterial(1.0);	// air material
	Absorption absorb;
	absorb.absorptionCoefficient = 1.0;
	air->addPropertyToSurfaces(&absorb);

	// -------------------------------------------------- Cube 1 ---------------------------------------------
	double WIDTH_CUBE1		= 100.0; //10000*um;
	double HEIGHT_CUBE1		= 100.0; //10000*um;
	double LENGTH_CUBE1		= 2000*um;
	double MFP_CUBE1		= 200*um;
	double REFRACTIVEINDEX_CUBE1	= 1.0;
	vertex V_z, V_y, V_x, pos1;
	V_x << WIDTH_CUBE1 << 0 << 0;	// X-dimension
	V_y << 0 << HEIGHT_CUBE1 << 0;	// Y-dimension
	V_z << 0 << 0 << LENGTH_CUBE1;	// Z-dimension
	pos1 << -WIDTH_CUBE1/2.0 << -HEIGHT_CUBE1/2.0 << -LENGTH_CUBE1/2.0;
	std::vector<vertex> vv;
	vv.push_back(V_x);
	vv.push_back(V_y);
	vv.push_back(V_z);
	SolidCuboid* cube1 		= new SolidCuboid(pos1, vv);

	//  MIE BULK-SCATTERING / FLUORESCENCE MATERIAL

	vector<string> CExtFileNames;
	vector<string> MiePdfFilenames;
	vector<string> PdfLambdaFileNames;
	vector<string> ExcitationSpectrumFileNames;
	vector<double> ConversionEfficiencies;
	vector<double> RHO;

	// GREEN PHOSPHOR
	double rho_green 		= 37244093.0/powf(10000.0,3.0); // Particle densities in µm^3
	double CE_green 		= 1.0;				// Conversion efficiency
	string mie_pdf_fname_green	= "../../Datafiles/SINGLESIZEPARTICLE/5umradius_n1.7_nbg1.0/MIE_PPDF_GREEN.dat";
	string mie_cext_fname_green	= "../../Datafiles/SINGLESIZEPARTICLE/5umradius_n1.7_nbg1.0/MIE_CEXT_EFF_GREEN.dat";
	string pdf_lambda_fname_green	= "../../Datafiles/GEM_PDF_SAMPLED.dat";
	string exitation_sp_green	= "../../Datafiles/EX_GREEN.dat";
	CExtFileNames.push_back(mie_cext_fname_green);
	MiePdfFilenames.push_back(mie_pdf_fname_green);
	PdfLambdaFileNames.push_back(pdf_lambda_fname_green);
	ExcitationSpectrumFileNames.push_back(exitation_sp_green);
	RHO.push_back(rho_green);
	ConversionEfficiencies.push_back(CE_green);


	// RED PHOSPHOR
	double rho_red			= 10474649/powf(10000.0,3.0);	// Particle densities in µm^3
	double CE_red			= 0.95;				// Conversion efficiency
	string mie_pdf_fname_red	= "../../Datafiles/SINGLESIZEPARTICLE/5umradius_n1.7_nbg1.0/MIE_PPDF_GREEN.dat";
	string mie_cext_fname_red	= "../../Datafiles/SINGLESIZEPARTICLE/5umradius_n1.7_nbg1.0/MIE_CEXT_EFF_GREEN.dat";
	string pdf_lambda_fname_red	= "../../Datafiles/REM_PDF_SAMPLED.dat";
	string exitation_sp_red		= "../../Datafiles/EX_RED.dat";
	CExtFileNames.push_back(mie_cext_fname_red);
	MiePdfFilenames.push_back(mie_pdf_fname_red);
	PdfLambdaFileNames.push_back(pdf_lambda_fname_red);
	ExcitationSpectrumFileNames.push_back(exitation_sp_red);
	RHO.push_back(rho_red);
	ConversionEfficiencies.push_back(CE_red);


//	cube1->material 		= new ClearMaterial(REFRACTIVEINDEX_CUBE1);
// 	cube1->material			= new RandomBulkScatter(MFP_CUBE1,REFRACTIVEINDEX_CUBE1);
//	cube1->material 		= new MieBulkScatter(CExtFileNames,MiePdfFilenames,RHO,REFRACTIVEINDEX_CUBE1);
	cube1->material 		= new MiePhosphor(CExtFileNames,
							  MiePdfFilenames,
							  RHO,
							  PdfLambdaFileNames,
							  ExcitationSpectrumFileNames,
							  ConversionEfficiencies,
							  REFRACTIVEINDEX_CUBE1);

	Refraction refr;
	cube1->addPropertyToSurfaces(&refr);


	// --------------------------------- IrradianceDetector for transmitted light ----------------------------
	double WIDTH_DET1	= 4000.0; //80000*um;
	double HEIGHT_DET1	= 4000.0; //80000*um;
	double ZPOS_DET1	= 3000*um;
	int XPIXELS_DET1	= 101;
	int YPIXELS_DET1	= 101;

	vertex pos_det;
	V_x(0) = WIDTH_DET1; V_x(1) = 0.0; V_x(2) = 0.0;	// X-dimension
	V_y(0) = 0.0; V_y(1) = HEIGHT_DET1; V_y(2) = 0.0;	// Y-dimension
	pos_det << -WIDTH_DET1/2.0 << -HEIGHT_DET1/2.0 << ZPOS_DET1;
	InternalSurface* det1 = new InternalSurface(air,pos_det,V_x,V_y);
	IrradianceDetector detect1(det1,XPIXELS_DET1,YPIXELS_DET1,N_RAYS);
	det1->addProperty(&detect1);
	Absorption absorb_det;
	absorb_det.absorptionCoefficient = 1.0;
	det1->addProperty(&absorb_det);

	// -------------------------------------- IrradianceDetector for reflected light --------------------------

	double WIDTH_DET2	= 4000.0; //80000*um;
	double HEIGHT_DET2	= 4000.0; //80000*um;
	double ZPOS_DET2	= -3000*um;
	int XPIXELS_DET2	= 101;
	int YPIXELS_DET2	= 101;

	vertex pos_det2;
	V_x(0) = WIDTH_DET2; V_x(1) = 0.0; V_x(2) = 0.0;	// X-dimension
	V_y(0) = 0.0; V_y(1) = HEIGHT_DET2; V_y(2) = 0.0;	// Y-dimension
	pos_det2 << -WIDTH_DET2/2.0 << -HEIGHT_DET2/2.0 << ZPOS_DET2;
	InternalSurface* det2 = new InternalSurface(air,pos_det2,V_x,V_y);
	IrradianceDetector detect2(det2,XPIXELS_DET2,YPIXELS_DET2,N_RAYS);
	det2->addProperty(&detect2);
	Absorption absorb_det2;
	absorb_det2.absorptionCoefficient = 1.0;
	det2->addProperty(&absorb_det2);
	// ---------------------------------------------------------------------------------------------------------




	air->encloseObject(cube1);
	air->surfaces.push_back(det1);
	air->surfaces.push_back(det2);





	// --------------------------------------------------

/*	// Common surfaces
	vector<Surface*> common = findCommonSurfaces(cube1, cube2);
	for(unsigned int ii = 0; ii < common.size(); ii+=2)
	{
		common[ii]->addNeighbour(common[ii+1]);
	}
*/


	// ------------------------------------------ SOURCE ----------------------------------
	vertex source_pos(3);
	source_pos(0) = 0.0*um; source_pos(1) = 0.0*um; source_pos(2) = -2000.0*um;

	vertex source_dir(3);
	source_dir(0) = 0.0; source_dir(1) = 0.0; source_dir(2) = 1;

	// LambertianPointSource s1(air,source_pos,0.45);
	DirectionalPointSource s1(air,source_dir,source_pos,0.45);

	// Rays
	s1.nRays	= N_RAYS;
	s1.rayFlux 	= (double)(1.0/(double)N_RAYS);
	// ------------------------------------------------------------------------------------



	// Tracer
	TracerController tracer;
	tracer.addSource(&s1);

	// Trace stuff
	tracer.startTracing();

	cout << "TRACING ENDED!" << endl;

	// ---------------------------- SAVE DETECTOR DATA -------------------------------------
	string DetData = "Irradiance1.dat";
	detect1.Irradiance(DetData,1);
	string DetDataHorizontal = "HORIZONTALIrradiance1.dat";
	detect1.IrradianceCrossSectionHorizontal(round(XPIXELS_DET1/2),DetDataHorizontal);
	string DetDataVertical = "VERTICALIrradiance1.dat";
	detect1.IrradianceCrossSectionVertical(round(YPIXELS_DET1/2),DetDataVertical);
	string Det1Spectrum = "Spectrum_det1.dat";
	detect1.Spectrum(Det1Spectrum);



	string DetData2 = "Irradiance2.dat";
	detect2.Irradiance(DetData2,1);
	string DetDataHorizontal2 = "HORIZONTALIrradiance2.dat";
	detect2.IrradianceCrossSectionHorizontal(round(XPIXELS_DET2/2),DetDataHorizontal2);
	string DetDataVertical2 = "VERTICALIrradiance2.dat";
	detect2.IrradianceCrossSectionVertical(round(YPIXELS_DET2/2),DetDataVertical2);
	string Det2Spectrum = "Spectrum_det2.dat";
	detect2.Spectrum(Det2Spectrum);
	// -------------------------------------------------------------------------------------

	cout << "DELETING OBJECTS" << endl;

	delete cube1->material;
	cout << "Cube1 material deleted" << endl;
	delete cube1;
	cout << "Cube1 deleted" << endl;

	delete air->material;
	cout << "air material deleted" << endl;
	delete air;
	cout << "air deleted" << endl;

	return (0);
}



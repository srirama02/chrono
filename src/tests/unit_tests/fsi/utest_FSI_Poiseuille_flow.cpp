// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Wei Hu
// =============================================================================
//
// Unit test for poiseuille flow. This unit test uses analytical solution to 
// verify the implementation
// =============================================================================

// General Includes
#include <assert.h>
#include <stdlib.h>
#include <ctime>

// Chrono includes
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/utils/ChUtilsGenerators.h"

// Chrono fsi includes
#include "chrono_fsi/ChSystemFsi.h"
#include "chrono_fsi/utils/ChUtilsGeneratorFsi.h"
#include "chrono_fsi/utils/ChUtilsJSON.h"

// Chrono namespaces
using namespace chrono;
using namespace chrono::collision;
using namespace chrono::fsi;

// Set a tolerance to control the test
const double rel_Tol = 1.0e-2;
float error_rel;

//------------------------------------------------------------------
// dimension of the fluid domain
//------------------------------------------------------------------
float bxDim = 0.2;
float byDim = 0.1;
float bzDim = 0.2;

float fxDim = bxDim;
float fyDim = byDim;
float fzDim = bzDim;

//------------------------------------------------------------------
// Analytical solution of the poiseuille flow
//------------------------------------------------------------------
float PoiseuilleAnalytical(float Z,
                           float L,
                           float time,
                           std::shared_ptr<fsi::SimParams> paramsH){
    float nu   = paramsH->mu0/paramsH->rho0;
    float F    = paramsH->bodyForce3.x;
    float initSpace0 = paramsH->MULT_INITSPACE * paramsH->HSML;
    float pi = 3.1415926;

    float L_modify = L + initSpace0;
    float Z_modify = Z + 0.5*initSpace0;

    float theory = 1.0/(2.0*nu)*F*Z_modify*(L_modify-Z_modify);

    for (int n=0; n<50; n++){
        theory = theory - 
        4.0*F*pow(L_modify,2)/(nu*pow(pi,3)*pow(2*n+1,3))*sin(pi*Z_modify*(2*n+1)/L_modify)*exp(-pow(2*n+1,2)*pow(pi,2)*nu*time/pow(L_modify,2));
    }

    return theory;
}

//------------------------------------------------------------------
// Create the wall boundary and the BCE particles
//------------------------------------------------------------------
void CreateSolidPhase(ChSystemSMC& mphysicalSystem,
                      ChSystemFsi& myFsiSystem,
                      std::shared_ptr<fsi::SimParams> paramsH) {
    auto mysurfmaterial = chrono_types::make_shared<ChMaterialSurfaceSMC>();

    // Set common material Properties
    mysurfmaterial->SetYoungModulus(6e4);
    mysurfmaterial->SetFriction(0.3f);
    mysurfmaterial->SetRestitution(0.2f);
    mysurfmaterial->SetAdhesion(0);

    // Ground body
    auto ground = chrono_types::make_shared<ChBody>();
    ground->SetIdentifier(-1);
    ground->SetBodyFixed(true);
    ground->SetCollide(true);
    ground->GetCollisionModel()->ClearModel();

    float initSpace0 = myFsiSystem.GetIniSpace();

    // Bottom and Top wall
    ChVector<> sizeWall(bxDim / 2, byDim / 2 + 0 * initSpace0, 2 * initSpace0);
    ChVector<> posBottom(0, 0, -3 * initSpace0);
    ChVector<> posTop(0, 0, bzDim + 1 * initSpace0);

    chrono::utils::AddBoxGeometry(ground.get(), mysurfmaterial, sizeWall, posBottom, chrono::QUNIT, true);
    ground->GetCollisionModel()->BuildModel();
    mphysicalSystem.AddBody(ground);

    // Add BCE particles to the bottom and top wall boundary
    myFsiSystem.AddBceBox(paramsH, ground, posBottom, chrono::QUNIT, sizeWall);
}

// ===============================
int main(int argc, char* argv[]) {
    // Create a physical system and a corresponding FSI system
    ChSystemSMC mphysicalSystem;
    ChSystemFsi myFsiSystem(mphysicalSystem);

    // Get the pointer to the system parameter and use a 
    // JSON file to fill it out with the user parameters
    std::shared_ptr<fsi::SimParams> paramsH = myFsiSystem.GetSimParams();

    // Use the default input file or you may enter 
    // your input parameters as a command line argument
    std::string inputJson = GetChronoDataFile("fsi/input_json/demo_FSI_Poiseuille_flow_Explicit.json");
    // fsi::utils::ParseJSON(inputJson, paramsH, fsi::mR3(bxDim, byDim, bzDim));
    myFsiSystem.SetSimParameter(inputJson, paramsH, ChVector<>(bxDim, byDim, bzDim));

    // Reset the domain size to handle periodic boundary condition
    float initSpace0 = paramsH->MULT_INITSPACE * paramsH->HSML;
    ChVector<> cMin(-bxDim / 2 - initSpace0 / 2, -byDim / 2 - initSpace0 / 2, - 5.0 * initSpace0);
    ChVector<> cMax( bxDim / 2 + initSpace0 / 2,  byDim / 2 + initSpace0 / 2, bzDim + 5.0 * initSpace0);
    myFsiSystem.SetPeriodicBC(cMin, cMax, paramsH);

    // Set up the solver based on the input value of the prameters
    myFsiSystem.SetFluidDynamics(paramsH->fluid_dynamic_type);
    myFsiSystem.SetFluidSystemLinearSolver(paramsH->LinearSolver);// this is only for ISPH

    // Setup sub doamins for a faster neighbor search
    myFsiSystem.SetSubDomain(paramsH);

    // Create SPH particles for the fluid domain
    chrono::utils::GridSampler<> sampler(initSpace0);
    ChVector<> boxCenter(-bxDim / 2 + fxDim / 2 , 0, fzDim * 0.5);
    ChVector<> boxHalfDim(fxDim / 2, fyDim / 2, fzDim / 2);
    std::vector<ChVector<>> points = sampler.SampleBox(boxCenter, boxHalfDim);
    size_t numPart = points.size();
    for (int i = 0; i < numPart; i++) {
        float v_x = PoiseuilleAnalytical(points[i].z(), bzDim, 0.5, paramsH);
        myFsiSystem.AddSphMarker(ChVector<>(points[i].x(), points[i].y(), points[i].z()),
                                 ChVector<>(paramsH->rho0, paramsH->BASEPRES, paramsH->mu0),
                                 paramsH->HSML, -1,
                                 ChVector<>(v_x, 0.0, 0.0));
    }
    myFsiSystem.AddRefArray(0, (int)numPart, -1, -1);

    // Create SPH particles for the solid domain
    CreateSolidPhase(mphysicalSystem, myFsiSystem, paramsH);

    // Finalize the setup before the simulation
    myFsiSystem.Finalize();

    float time = 0;
    int stepEnd = 200;
    for (int tStep = 0; tStep < stepEnd + 1; tStep++) {
        myFsiSystem.DoStepDynamics_FSI();
        time += paramsH->dT;
    
        // Copy data from device to host
        thrust::host_vector<fsi::Real4> posRadH = myFsiSystem.GetFsiData()->sphMarkersD2->posRadD;
        thrust::host_vector<fsi::Real3> velMasH = myFsiSystem.GetFsiData()->sphMarkersD2->velMasD;

        // std::vector<ChVector<>> ParPos;
        // myFsiSystem.GetParticlePos(ParPos);
        // std::vector<ChVector<>> ParVel;
        // myFsiSystem.GetParticleVel(ParVel);
        
        // Calculate the relative error of the solution
        float error = 0.0;
        float abs_val = 0.0;
        for (int i = 0; i < numPart; i++) {
            float pos_Z = posRadH[i].z;
            float vel_X = velMasH[i].x;
            float vel_X_ana = PoiseuilleAnalytical(pos_Z, bzDim, time + 0.5, paramsH);
            error = error + pow(vel_X - vel_X_ana, 2);
            abs_val = abs_val + pow(vel_X_ana, 2);
        }
        error_rel = sqrt(error/abs_val);
        if ((tStep>1) && (error_rel > rel_Tol)){
            printf("\n step = %d, error_rel =  %0.8f \n",  tStep, error_rel);
            return 1;
        }
    }

    printf("\n  error_rel =  %0.8f \n", error_rel);
    return 0;
}
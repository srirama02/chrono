// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2023 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Author: Radu Serban
// =============================================================================
//
// Polaris wheeled vehicle on SPH terrain (initialized from particle data files)
//
// =============================================================================

#include <cstdio>
#include <string>
#include <stdexcept>
#include <iomanip>

#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/utils/ChUtilsInputOutput.h"

#include "chrono_fsi/ChSystemFsi.h"

#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/driver/ChPathFollowerDriver.h"
#include "chrono_vehicle/tracked_vehicle/vehicle/TrackedVehicle.h"
#include "chrono_vehicle/utils/ChUtilsJSON.h"
#include "chrono_vehicle/terrain/SPHTerrain.h"

#include "chrono_thirdparty/filesystem/path.h"

#include "chrono/assets/ChVisualSystem.h"
#ifdef CHRONO_OPENGL
    #include "chrono_fsi/visualization/ChFsiVisualizationGL.h"
#endif
#ifdef CHRONO_VSG
    #include "chrono_fsi/visualization/ChFsiVisualizationVSG.h"
#endif

#include "demos/vehicle/SetChronoSolver.h"

using namespace chrono;
using namespace chrono::fsi;
using namespace chrono::vehicle;

using std::cout;
using std::cin;
using std::endl;

// ===================================================================================================================

// Run-time visualization system (OpenGL or VSG)
ChVisualSystem::Type vis_type = ChVisualSystem::Type::VSG;

// ===================================================================================================================

std::shared_ptr<TrackedVehicle> CreateVehicle(ChSystem& sys, const ChCoordsys<>& init_pos);
std::shared_ptr<ChBezierCurve> CreatePath(const std::string& path_file);
void CreateTrackBCEMarkers(std::shared_ptr<TrackedVehicle> vehicle, ChSystemFsi& sysFSI);

// ===================================================================================================================

int main(int argc, char* argv[]) {
    // Set model and simulation parameters
    std::string terrain_dir = "terrain/sph/S-lane_RMS";

    double density = 1700;
    double cohesion = 5e3;
    double friction = 0.8;
    double youngs_modulus = 1e6;
    double poisson_ratio = 0.3;

    double target_speed = 7.0;
    double tend = 30;
    double step_size = 5e-4;
    double active_box_hdim = 0.4;

    bool visualization = true;             // run-time visualization
    double visualizationFPS = 0;           // frames rendered per second (0: every frame)
    bool visualization_sph = true;         // render SPH particles
    bool visualization_bndry_bce = false;  // render boundary BCE markers
    bool visualization_rigid_bce = false;  // render wheel BCE markers
    bool chase_cam = true;                 // chase-cam or fixed camera

    bool verbose = true;

    // Create the Chrono system
    ChSystemNSC sys;
    SetChronoSolver(sys, ChSolver::Type::BARZILAIBORWEIN, ChTimestepper::Type::EULER_IMPLICIT_LINEARIZED);

    // Create the SPH terrain system
    SPHTerrain terrain(sys, 0.02);
    terrain.SetVerbose(verbose);
    ChSystemFsi& sysFSI = terrain.GetSystemFSI();

    // Set SPH parameters and soil material properties
    const ChVector<> gravity(0, 0, -9.81);
    sysFSI.Set_G_acc(gravity);
    sys.Set_G_acc(gravity);

    ChSystemFsi::ElasticMaterialProperties mat_props;
    mat_props.Young_modulus = youngs_modulus;
    mat_props.Poisson_ratio = poisson_ratio;
    mat_props.stress = 0;  // default
    mat_props.viscosity_alpha = 0.5;
    mat_props.viscosity_beta = 0.0;
    mat_props.mu_I0 = 0.04;
    mat_props.mu_fric_s = friction;
    mat_props.mu_fric_2 = friction;
    mat_props.average_diam = 0.005;
    mat_props.friction_angle = CH_C_PI / 10;  // default
    mat_props.dilation_angle = CH_C_PI / 10;  // default
    mat_props.cohesion_coeff = 0;             // default
    mat_props.kernel_threshold = 0.8;

    sysFSI.SetElasticSPH(mat_props);
    sysFSI.SetDensity(density);
    sysFSI.SetCohesionForce(cohesion);

    sysFSI.SetActiveDomain(ChVector<>(active_box_hdim));
    sysFSI.SetDiscreType(false, false);
    sysFSI.SetWallBC(BceVersion::ORIGINAL);
    sysFSI.SetSPHMethod(FluidDynamics::WCSPH);
    sysFSI.SetStepSize(step_size);

    sysFSI.SetOutputLength(0);

    // Construct the terrain using SPH particles and BCE markers from files
    cout << "Create terrain..." << endl;
    terrain.Construct(vehicle::GetDataFile(terrain_dir + "/sph_particles.txt"),
                      vehicle::GetDataFile(terrain_dir + "/bce_markers.txt"));

    // Create vehicle
    cout << "Create vehicle..." << endl;
    ChVector<> veh_init_pos(5.0, 0, 0.7);
    auto vehicle = CreateVehicle(sys, ChCoordsys<>(veh_init_pos, QUNIT));

    // Create the track shoe BCE markers
    CreateTrackBCEMarkers(vehicle, sysFSI);

    // Initialize the terrain system
    terrain.Initialize();

    ChVector<> aabb_min, aabb_max;
    terrain.GetAABB(aabb_min, aabb_max);
    cout << "  SPH particles:     " << sysFSI.GetNumFluidMarkers() << endl;
    cout << "  Bndry BCE markers: " << sysFSI.GetNumBoundaryMarkers() << endl;
    cout << "  AABB:              " << aabb_min << "   " << aabb_max << endl;

    // Create driver
    cout << "Create path..." << endl;
    auto path = CreatePath(terrain_dir + "/path.txt");
    double x_max = path->getPoint(path->getNumPoints() - 2).x() - 3.0;
    ChPathFollowerDriver driver(*vehicle, path, "my_path", target_speed);
    driver.GetSteeringController().SetLookAheadDistance(2.0);
    driver.GetSteeringController().SetGains(1.0, 0, 0);
    driver.GetSpeedController().SetGains(0.6, 0.05, 0);
    driver.Initialize();

    // Create run-time visualization
#ifndef CHRONO_OPENGL
    if (vis_type == ChVisualSystem::Type::OpenGL)
        vis_type = ChVisualSystem::Type::VSG;
#endif
#ifndef CHRONO_VSG
    if (vis_type == ChVisualSystem::Type::VSG)
        vis_type = ChVisualSystem::Type::OpenGL;
#endif

    std::shared_ptr<ChFsiVisualization> visFSI;
    if (visualization) {
        switch (vis_type) {
            case ChVisualSystem::Type::OpenGL:
#ifdef CHRONO_OPENGL
                visFSI = chrono_types::make_shared<ChFsiVisualizationGL>(&sysFSI, verbose);
#endif
                break;
            case ChVisualSystem::Type::VSG: {
#ifdef CHRONO_VSG
                visFSI = chrono_types::make_shared<ChFsiVisualizationVSG>(&sysFSI, verbose);
#endif
                break;
            }
        }

        visFSI->SetTitle("Tracked vehicle SPH deformable terrain");
        visFSI->SetSize(1280, 720);
        visFSI->AddCamera(ChVector<>(0, 8, 0.5), ChVector<>(0, -1, 0));
        visFSI->SetCameraMoveScale(0.2f);
        visFSI->EnableFluidMarkers(visualization_sph);
        visFSI->EnableBoundaryMarkers(visualization_bndry_bce);
        visFSI->EnableRigidBodyMarkers(visualization_rigid_bce);
        visFSI->SetRenderMode(ChFsiVisualization::RenderMode::SOLID);
        visFSI->SetParticleRenderMode(ChFsiVisualization::RenderMode::SOLID);
        visFSI->SetSPHColorCallback(
            chrono_types::make_shared<HeightColorCallback>(ChColor(0.10f, 0.40f, 0.65f), aabb_min.z(), aabb_max.z()));
        visFSI->AttachSystem(&sys);
        visFSI->Initialize();
    }

    // Simulation loop
    DriverInputs driver_inputs = {0, 0, 0};
    int render_steps = (visualizationFPS > 0) ? (int)std::round((1.0 / visualizationFPS) / step_size) : 1;
    double t = 0;
    int frame = 0;

    if (x_max < veh_init_pos.x())
        x_max = veh_init_pos.x() + 0.25;

    cout << "Start simulation..." << endl;
    TerrainForces shoe_forces_left(vehicle->GetNumTrackShoes(LEFT));
    TerrainForces shoe_forces_right(vehicle->GetNumTrackShoes(RIGHT));

    while (t < tend) {
        const auto& veh_loc = vehicle->GetPos();

        // Stop before end of patch
        if (veh_loc.x() > x_max)
            break;

        // Set current driver inputs
        driver_inputs = driver.GetInputs();

        if (t < 0.5) {
            driver_inputs.m_throttle = 0;
            driver_inputs.m_braking = 1;
        } else {
            ChClampValue(driver_inputs.m_throttle, driver_inputs.m_throttle, (t - 0.5) / 0.5);
        }

        // Run-time visualization
        if (visualization && frame % render_steps == 0) {
            if (chase_cam) {
                ChVector<> cam_loc = veh_loc + ChVector<>(-6, 6, 0.5);
                ChVector<> cam_point = veh_loc;
                visFSI->UpdateCamera(cam_loc, cam_point);
            }
            if (!visFSI->Render())
                break;
        }
        if (!visualization) {
            cout << sysFSI.GetSimTime() << "  " << sysFSI.GetRTF() << endl;
        }

        // Synchronize sy^stems
        driver.Synchronize(t);
        vehicle->Synchronize(t, driver_inputs, shoe_forces_left, shoe_forces_right);

        // Advance system state
        driver.Advance(step_size);
        sysFSI.DoStepDynamics_FSI();
        t += step_size;

        frame++;
    }

    return 0;
}

// ===================================================================================================================

std::shared_ptr<TrackedVehicle> CreateVehicle(ChSystem& sys, const ChCoordsys<>& init_pos) {
    std::string vehicle_json = "M113/vehicle/M113_Vehicle_SinglePin.json";
    std::string engine_json = "M113/powertrain/M113_EngineSimple.json";
    std::string transmission_json = "M113/powertrain/M113_AutomaticTransmissionSimpleMap.json";

    // Create and initialize the vehicle
    auto vehicle = chrono_types::make_shared<TrackedVehicle>(&sys, vehicle::GetDataFile(vehicle_json));
    vehicle->Initialize(init_pos);
    vehicle->GetChassis()->SetFixed(false);

    vehicle->SetChassisVisualizationType(VisualizationType::NONE);
    vehicle->SetSprocketVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetIdlerVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetSuspensionVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetIdlerWheelVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetRoadWheelVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetRollerVisualizationType(VisualizationType::PRIMITIVES);
    vehicle->SetTrackShoeVisualizationType(VisualizationType::PRIMITIVES);

    // Create and initialize the powertrain system
    auto engine = ReadEngineJSON(vehicle::GetDataFile(engine_json));
    auto transmission = ReadTransmissionJSON(vehicle::GetDataFile(transmission_json));
    auto powertrain = chrono_types::make_shared<ChPowertrainAssembly>(engine, transmission);
    vehicle->InitializePowertrain(powertrain);

    return vehicle;
}

std::shared_ptr<ChBezierCurve> CreatePath(const std::string& path_file) {
    // Open input file
    std::ifstream ifile(vehicle::GetDataFile(path_file));
    std::string line;

    // Read number of knots and type of curve
    size_t numPoints;
    size_t numCols;

    std::getline(ifile, line);
    std::istringstream iss(line);
    iss >> numPoints >> numCols;

    assert(numCols == 3);

    // Read path points
    std::vector<ChVector<>> points;

    for (size_t i = 0; i < numPoints; i++) {
        double x, y, z;
        std::getline(ifile, line);
        std::istringstream jss(line);
        jss >> x >> y >> z;
        points.push_back(ChVector<>(x, y, z));
    }

    // Include point beyond SPH patch
    {
        auto np = points.size();
        points.push_back(2.0 * points[np - 1] - points[np - 2]);
    }

    // Raise all path points
    for (auto& p : points)
        p.z() += 0.1;

    ifile.close();

    return std::shared_ptr<ChBezierCurve>(new ChBezierCurve(points));
}

void CreateTrackBCEMarkers(std::shared_ptr<TrackedVehicle> vehicle, ChSystemFsi& sysFSI) {
    // GetCollision shapes for a track shoe (will use only collision boxes)
    auto geometry = vehicle->GetTrackShoe(VehicleSide::LEFT, 0)->GetGroundContactGeometry();

    // Consider only collision boxes that are large enough
    auto min_length = 2 * (sysFSI.GetNumBoundaryLayers() - 1) * sysFSI.GetInitialSpacing();
    std::vector<ChVehicleGeometry::BoxShape> coll_boxes;
    for (const auto& box : geometry.m_coll_boxes) {
        if (box.m_dims.x() > min_length && box.m_dims.y() > min_length && box.m_dims.z() < min_length)
            coll_boxes.push_back(box);
    }

    cout << "Consider " << coll_boxes.size() << " collision boxes out of " << geometry.m_coll_boxes.size() << endl;

    // Add an FSI body and associated BCE markers for each track shoe
    size_t num_track_BCE = 0;

    auto nshoes_left = vehicle->GetNumTrackShoes(VehicleSide::LEFT);
    for (size_t i = 0; i < nshoes_left; i++) {
        auto shoe_body = vehicle->GetTrackShoe(VehicleSide::LEFT, i)->GetShoeBody();
        sysFSI.AddFsiBody(shoe_body);
        for (const auto& box : coll_boxes) {
            num_track_BCE += sysFSI.AddBoxBCE(shoe_body, ChFrame<>(box.m_pos, box.m_rot), box.m_dims, true);
        }
    }

    auto nshoes_right = vehicle->GetNumTrackShoes(VehicleSide::RIGHT);
    for (size_t i = 0; i < nshoes_right; i++) {
        auto shoe_body = vehicle->GetTrackShoe(VehicleSide::RIGHT, i)->GetShoeBody();
        sysFSI.AddFsiBody(shoe_body);
        for (const auto& box : coll_boxes) {
            num_track_BCE += sysFSI.AddBoxBCE(shoe_body, ChFrame<>(box.m_pos, box.m_rot), box.m_dims, true);
        }
    }

    cout << "Added " << num_track_BCE << " BCE markers on track shoes" << endl;
}

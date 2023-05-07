// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2023 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Rainer Gericke
// =============================================================================
//
// Base class for the U401 vehicle models
//
// =============================================================================

#ifndef U401_VEHICLE_H
#define U401_VEHICLE_H

#include "chrono_vehicle/wheeled_vehicle/ChWheeledVehicle.h"

#include "chrono_models/ChApiModels.h"
#include "chrono_models/vehicle/ChVehicleModelDefs.h"

namespace chrono {
namespace vehicle {
namespace unimog {

/// @addtogroup vehicle_models_U401
/// @{

/// UAZ vehicle system.
class CH_MODELS_API U401_Vehicle : public ChWheeledVehicle {
   public:
    U401_Vehicle(const bool fixed,
                 BrakeType brake_type,
                 SteeringTypeWV steering_model,
                 ChContactMethod contact_method = ChContactMethod::NSC,
                 CollisionType chassis_collision_type = CollisionType::NONE);

    U401_Vehicle(ChSystem* system,
                 const bool fixed,
                 BrakeType brake_type,
                 SteeringTypeWV steering_model,
                 CollisionType chassis_collision_type = CollisionType::NONE);

    ~U401_Vehicle();

    virtual int GetNumberAxles() const override { return 2; }

    virtual double GetWheelbase() const override { return 2.3; }
    virtual double GetMinTurningRadius() const override { return 5.8; }
    virtual double GetMaxSteeringAngle() const override { return 27 * CH_C_DEG_TO_RAD; }

    void SetInitWheelAngVel(const std::vector<double>& omega) {
        assert(omega.size() == 4);
        m_omega = omega;
    }

    double GetSpringForce(int axle, VehicleSide side) const;
    double GetSpringLength(int axle, VehicleSide side) const;
    double GetSpringDeformation(int axle, VehicleSide side) const;

    double GetShockForce(int axle, VehicleSide side) const;
    double GetShockLength(int axle, VehicleSide side) const;
    double GetShockVelocity(int axle, VehicleSide side) const;

    virtual void Initialize(const ChCoordsys<>& chassisPos, double chassisFwdVel = 0) override;

    // Log debugging information
    void LogHardpointLocations();  /// suspension hardpoints at design
    void DebugLog(int what);       /// shock forces and lengths, constraints, etc.

   private:
    void Create(bool fixed, BrakeType brake_type, SteeringTypeWV steering_model, CollisionType chassis_collision_type);

    std::vector<double> m_omega;
};

/// @} vehicle_models_U401

}  // namespace unimog
}  // end namespace vehicle
}  // end namespace chrono

#endif

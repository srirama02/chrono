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
// Authors: Radu Serban
// =============================================================================
//
// Marder automatic transmission model based on ChShaft objects.
//
// =============================================================================

#ifndef MARDER_AUTOMATIC_TRANSMISSION_SHAFTS_H
#define MARDER_AUTOMATIC_TRANSMISSION_SHAFTS_H

#include "chrono_vehicle/ChVehicle.h"
#include "chrono_vehicle/powertrain/ChAutomaticTransmissionShafts.h"

#include "chrono_models/ChApiModels.h"

namespace chrono {
namespace vehicle {
namespace marder {

/// @addtogroup vehicle_models_Marder
/// @{

/// Shafts-based powertrain model for the Marder vehicle.
class CH_MODELS_API Marder_AutomaticTransmissionShafts : public ChAutomaticTransmissionShafts {
  public:
    Marder_AutomaticTransmissionShafts(const std::string& name);
    ~Marder_AutomaticTransmissionShafts() {}

    virtual void SetGearRatios(std::vector<double>& fwd, double& rev) override;

    virtual double GetTransmissionBlockInertia() const override { return m_transmissionblock_inertia; }
    virtual double GetIngearShaftInertia() const override { return m_ingear_shaft_inertia; }
    virtual double GetMotorshaftInertia() const override { return m_motorshaft_inertia; }
    virtual double GetDriveshaftInertia() const override { return m_driveshaft_inertia; }

    virtual double GetUpshiftRPM() const override { return m_upshift_RPM; }
    virtual double GetDownshiftRPM() const override { return m_downshift_RPM; }

    virtual void SetTorqueConverterCapacityFactorMap(std::shared_ptr<ChFunction_Recorder>& map) override;
    virtual void SetTorqeConverterTorqueRatioMap(std::shared_ptr<ChFunction_Recorder>& map) override;

  private:
    // Shaft inertias.
    static const double m_transmissionblock_inertia;
    static const double m_motorshaft_inertia;
    static const double m_driveshaft_inertia;
    static const double m_ingear_shaft_inertia;

    // Gear shifting characteristics
    static const double m_upshift_RPM;
    static const double m_downshift_RPM;
};

/// @} vehicle_models_Marder

}  // end namespace Marder
}  // end namespace vehicle
}  // end namespace chrono

#endif


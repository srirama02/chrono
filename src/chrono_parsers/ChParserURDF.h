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
// Parser utility class for URDF input files.
//
// =============================================================================

#ifndef CH_PARSER_URDF_H
#define CH_PARSER_URDF_H

#include "chrono_parsers/ChApiParsers.h"

#include "chrono/physics/ChSystem.h"
#include "chrono/physics/ChBodyAuxRef.h"
#include "chrono/physics/ChMaterialSurface.h"

#include <urdf_parser/urdf_parser.h>

namespace chrono {
namespace parsers {

/// @addtogroup parsers_module
/// @{

/// URDF input file parser.
class ChApiParsers ChParserURDF {
  public:
    /// Motor actuation types.
    enum class ActuationType {
        POSITION,  ///< position (linear motor) or angle (rotation motor)
        SPEED,     ///< linear speed (linear motor) or angular speed (rotation motor)
        FORCE      ///< force (linear motor) or torque (rotation motor)
    };

    /// Construct a Chrono parser for the specified URDF file.
    ChParserURDF(const std::string& filename);

    /// Get the URDF model name.
    const std::string& GetModelName() const { return m_model->getName(); }

    /// Get the URDF model tree.
    const urdf::ModelInterfaceSharedPtr& GetModelTree() const { return m_model; }

    /// Print the body tree from parsed URDF file.
    void PrintModelBodies();

    /// Print the list of joints from parsed URDF file.
    void PrintModelJoints();

    /// Set the initial pose of the model (root link).
    void SetRootInitPose(const ChFrame<>& init_pose);

    /// Set the specified joint as actuated, using the specified actuation type.
    /// By default, all URDF joints are translated into Chrono kinematic joints. Joints marked as actuated will be
    /// translated to Chrono motors (with same kinematics as the corresponding passive joint). This function has no
    /// effect for joints other than Revolute or Prismatic.
    void SetJointActuated(const std::string& joint_name, ActuationType actuation_type);

    /// Set all candidate joints in the URDF model as actuated, using the specified actuation type.
    /// This function has no effect for joints other than Revolute or Prismatic.
    void SetAllJointsActuated(ActuationType actuation_type);

    /// Set default contact material properties.
    /// All bodies for which SetBodyContactMaterial was not explicitly called will be constructed with this contact
    /// material for all their collision shapes.
    void SetDefaultContactMaterial(const ChContactMaterialData& mat_data);

    /// Set contact material properties for the specified body.
    /// Bodies for which this function is not explictly called are constructed with the default contact material.
    void SetBodyContactMaterial(const std::string& body_name, const ChContactMaterialData& mat_data);

    /// Create the Chrono model in the given system from the parsed URDF model.
    void PopulateSystem(ChSystem& sys);

    /// Get the root body of the Chrono model.
    /// This function must be called after PopulateSystem.
    std::shared_ptr<ChBodyAuxRef> GetRootBody() const;

    /// Set the actuation function for the specified Chrono motor.
    /// The return value of this function has different meaning, depending on the type of motor, and can represent a
    /// position, angle, linear speed, angular speed, force, or torque.
    /// No action is taken if a joint with the specified does not exist or if it was not marked as actuated.
    /// This function must be called after PopulateSystem.
    void SetMotorFunction(const std::string& motor_name, const std::shared_ptr<ChFunction> function);

  private:
    ChColor toChColor(const urdf::Color& color);
    ChVector<> toChVector(const urdf::Vector3& vec);
    ChQuaternion<> toChQuaternion(const urdf::Rotation& rot);
    ChFrame<> toChFrame(const urdf::Pose& pose);
    std::shared_ptr<ChVisualShape> toChVisualShape(const urdf::GeometrySharedPtr geometry);
    std::shared_ptr<ChBodyAuxRef> toChBody(urdf::LinkConstSharedPtr link);
    std::shared_ptr<ChLink> toChLink(urdf::JointSharedPtr& joint);

    /// Recursively create bodies and joints in the Chrono model.
    void createChildren(urdf::LinkConstSharedPtr parent, const ChFrame<>& parent_frame);

    std::string m_filename;                                   ///< URDF file name
    std::string m_filepath;                                   ///< path of URDF file
    urdf::ModelInterfaceSharedPtr m_model;                    ///< parsed URDF model
    ChSystem* m_sys;                                          ///< containing Chrono system
    ChFrame<> m_init_pose;                                    ///< root body initial pose
    std::shared_ptr<ChBodyAuxRef> m_root_body;                ///< model root body
    std::map<std::string, ActuationType> m_actuated_joints;   ///< actuated joints
    std::map<std::string, ChContactMaterialData> m_mat_data;  ///< body contact material data
    ChContactMaterialData m_default_mat_data;                 ///< default contact material data
};

/// @} parsers_module

}  // end namespace parsers
}  // end namespace chrono

#endif

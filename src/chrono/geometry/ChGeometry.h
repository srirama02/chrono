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
// Authors: Alessandro Tasora, Radu Serban
// =============================================================================

#ifndef CHC_GEOMETRY
#define CHC_GEOMETRY

#include <memory>
#include <limits>

#include "chrono/core/ChApiCE.h"
#include "chrono/core/ChMath.h"

namespace chrono {

/// Namespace for classes which represent basic geometric objects
namespace geometry {

/// @addtogroup chrono_geometry
/// @{

/// Enumeration for the 3 frame directions.
enum class ChAxis {
    X,  ///< x direction of a reference frame
    Y,  ///< y direction of a reference frame
    Z   ///< z direction of a reference frame
};

/// Base class for geometric objects used for collisions and visualization.
class ChApi ChGeometry {
  public:
    /// Enumeration of geometric object types.
    enum class Type {
        NONE,
        SPHERE,
        ELLIPSOID,
        BOX,
        CYLINDER,
        TRIANGLE,
        CAPSULE,
        CONE,
        LINE,
        LINE_ARC,
        LINE_BEZIER,
        LINE_CAM,
        LINE_PATH,
        LINE_POLY,
        LINE_SEGMENT,
        ROUNDED_BOX,
        ROUNDED_CYLINDER,
        TRIANGLEMESH,
        TRIANGLEMESH_CONNECTED,
        TRIANGLEMESH_SOUP
    };

    /// Axis-aligned bounding box.
    struct ChApi AABB {
        /// Default is an inverted bounding box.
        AABB();

        /// Construct an AABB with provided corners.
        AABB(const ChVector<>& aabb_min, const ChVector<>& aabb_max);

        /// Get AABB center.
        ChVector<> Center() const;

        /// Get AABB dimensions.
        ChVector<> Size() const;

        ChVector<> min;  ///< low AABB corner
        ChVector<> max;  ///< high AABB corner
    };

  public:
    ChGeometry() {}
    ChGeometry(const ChGeometry& source) {}
    virtual ~ChGeometry() {}

    /// "Virtual" copy constructor.
    virtual ChGeometry* Clone() const = 0;

    /// Get the class type as an enum.
    virtual Type GetClassType() const { return Type::NONE; }

    /// Compute bounding box along the directions defined by the given rotation matrix.
    /// The default implementation returns a bounding box with zeros dimensions.
    virtual AABB GetBoundingBox(const ChMatrix33<>& rot) const;

    /// Enlarge the given existing bounding box with the bounding box of this object.
    void InflateBoundingBox(AABB& bbox, const ChMatrix33<>& rot) const;

    /// Returns the radius of a bounding sphere for this geometry.
    /// The default implementation returns the radius of a sphere bounding the geometry bounding box, which is not
    /// always the tightest possible.
    virtual double GetBoundingSphereRadius() const;

    /// Compute center of mass.
    virtual ChVector<> Baricenter() const { return VNULL; }

    /// Returns the dimension of the geometry
    /// (0=point, 1=line, 2=surface, 3=solid)
    virtual int GetManifoldDimension() const { return 0; }

    /// Generic update of internal data.
    virtual void Update() {}

    /// Method to allow serialization of transient data to archives.
    virtual void ArchiveOUT(ChArchiveOut& marchive);

    /// Method to allow de serialization of transient data from archives.
    virtual void ArchiveIN(ChArchiveIn& marchive);
};

/// @} chrono_geometry

}  // end namespace geometry

CH_CLASS_VERSION(geometry::ChGeometry, 0)

}  // end namespace chrono

#endif

// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2022 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Radu Serban
// =============================================================================

#ifndef CH_VISUAL_SYSTEM_IRRLICHT_H
#define CH_VISUAL_SYSTEM_IRRLICHT_H

#include <string>
#include <unordered_set>

#include <irrlicht.h>

#include "chrono/assets/ChVisualSystem.h"
#include "chrono/assets/ChBoxShape.h"
#include "chrono/assets/ChCylinderShape.h"
#include "chrono/assets/ChSphereShape.h"
#include "chrono/assets/ChCapsuleShape.h"
#include "chrono/assets/ChObjShapeFile.h"
#include "chrono/assets/ChVisualShape.h"
#include "chrono/assets/ChTriangleMeshShape.h"
#include "chrono/assets/ChGlyphs.h"
#include "chrono/assets/ChPathShape.h"
#include "chrono/assets/ChLineShape.h"

#include "chrono_irrlicht/ChApiIrr.h"
#include "chrono_irrlicht/ChIrrNode.h"
#include "chrono_irrlicht/ChIrrEffects.h"
#include "chrono_irrlicht/ChIrrGUI.h"

namespace chrono {
namespace irrlicht {

/// Vertical direction
enum class CameraVerticalDir { Y, Z };

class ChApiIrr ChVisualSystemIrrlicht : public ChVisualSystem {
  public:
    ChVisualSystemIrrlicht(ChSystem& sys);
    ~ChVisualSystemIrrlicht();

    /// Enable/disable antialias (default true).
    /// Must be called before Initialize().
    void SetAntialias(bool val);

    /// Enable/disable full-screen mode (default false).
    /// Must be called before Initialize().
    void SetFullscreen(bool val);

    /// Enable/disable shadows (default false).
    /// Must be called before Initialize().
    void SetShadows(bool val);

    /// Set the device driver type (default irr::video::EDT_DIRECT3D9).
    /// Must be called before Initialize().
    void SetDriverType(irr::video::E_DRIVER_TYPE driver_type);

    /// Set the window size (default 640x480).
    /// Must be called before Initialize().
    void SetWindowSize(const irr::core::dimension2d<irr::u32>& win_size);
    void SetWindowSize(const ChVector2<int>& win_size);

    /// Set the windoiw title (default "").
    /// Must be called before Initialize().
    void SetWindowTitle(const std::string& win_title);

    /// Use Y-up camera rendering (default CameraVerticalDir::Y).
    /// Must be called before Initialize().
    void SetCameraVertical(CameraVerticalDir vert);

    /// Set the Irrlicht logging level (default irr::ELL_INFORMATION).
    /// Must be called before Initialize().
    void SetLogLevel(irr::ELOG_LEVEL log_level);

    /// Initialize the visualization system.
    /// This creates the Irrlicht device using the current values for the optional device parameters.
    void Initialize();

    /// Add a logo in a 3D scene.
    /// Has no effect, unles called after Initialize().
    void AddLogo(const std::string& logo_filename = GetChronoDataFile("logo_chronoengine_alpha.png"));

    /// Add a camera in an Irrlicht 3D scene.
    /// The camera rotation/pan is controlled by mouse left and right buttons, the zoom is controlled by mouse wheel or
    /// rmb+lmb+mouse, the position can be changed also with keyboard up/down/left/right arrows, the height can be
    /// changed with keyboard 'PgUp' and 'PgDn' keys. Optional parameters are position and target.
    /// Has no effect, unles called after Initialize().
    void AddCamera(irr::core::vector3df pos = irr::core::vector3df(0, 0, -8),
                   irr::core::vector3df targ = irr::core::vector3df(0, 0, 0));

    /// Add a sky box in a 3D scene.
    /// Note: it is assumed that the specified "texture_dir" directory contains the following three texture images:
    /// sky_lf.jpg, sky_up.jpg, sky_dn.jpg.
    /// Has no effect, unless called after Initialize().
    void AddSkyBox(const std::string& texture_dir = GetChronoDataFile("skybox/"));

    /// Add a point light to the scene.
    /// Has no effect, unles called after Initialize().
    irr::scene::ILightSceneNode* AddLight(irr::core::vector3df pos,
                                          double radius,
                                          irr::video::SColorf color = irr::video::SColorf(0.7f, 0.7f, 0.7f, 1.0f));

    /// Add a point light that cast shadow (using soft shadows/shadow maps)
    /// Note that the quality of the shadow strictly depends on how close 'near_value' and 'far_value' are to the
    /// bounding box of the scene. Use AddShadow() to enable shadows for an object or for the entire scene.
    /// Has no effect, unless called after Initialize().
    irr::scene::ILightSceneNode* AddLightWithShadow(irr::core::vector3df pos,
                                                    irr::core::vector3df aim,
                                                    double radius,
                                                    double near_value,
                                                    double far_value,
                                                    double angle,
                                                    irr::u32 resolution = 512,
                                                    irr::video::SColorf color = irr::video::SColorf(1.f, 1.f, 1.f, 1.f),
                                                    bool directional = false,
                                                    bool clipborder = true);

    /// Simple shortcut to set two point lights in the scene.
    /// For more precise control on lights, use AddLight() or just use Irrlicht directly.
    /// Has no effect, unless called after Initialize().
    void AddTypicalLights();

    /// Enable shadow maps for all visual models in a scene or only for a single physics item.
    /// A shadow-enabled light must be added to the scene with
    /// Shadow maps in Irrlicht may slow visualization a bit. Also, one must remember to add shadow-enabled light, using
    /// AddLightWithShadow().
    /// Has no effect, unless called after Initialize().
    void EnableShadows(std::shared_ptr<ChPhysicsItem> item = nullptr);

    /// Attach a custom event receiver to the application.
    /// Has no effect, unless called after Initialize().
    void AddUserEventReceiver(irr::IEventReceiver* receiver);

    irr::IrrlichtDevice* GetDevice() { return m_device; }
    irr::video::IVideoDriver* GetVideoDriver() { return m_device->getVideoDriver(); }
    irr::scene::ISceneManager* GetSceneManager() { return m_device->getSceneManager(); }
    irr::scene::ICameraSceneNode* GetActiveCamera() { return m_device->getSceneManager()->getActiveCamera(); }
    irr::gui::IGUIEnvironment* GetGUIEnvironment() { return m_device->getGUIEnvironment(); }

    /// Process all visual assets in the associated ChSystem.
    /// This function is called by default by Initialize(), but can also be called later if further modifications to
    /// visualization assets occur.
    virtual void BindAll() override;

    /// Process the visual assets for the spcified physics item.
    /// This function must be called if a new physics item is added to the system or if changes to its visual model
    /// occur after the call to Initialize().
    virtual void BindItem(std::shared_ptr<ChPhysicsItem> item) override;

    /// Clean the canvas at the beginning of each animation frame.
    virtual void BeginScene(bool backBuffer = true,
                            bool zBuffer = true,
                            irr::video::SColor color = irr::video::SColor(255, 0, 0, 0));

    /// Draw all 3D shapes and GUI elements at the current frame.
    /// This function is typically called inside a loop such as
    /// <pre>
    ///    while(vis->GetDevice()->run()) {...}
    /// </pre>
    virtual void DrawAll();

    /// End the scene draw at the end of each animation frame.
    virtual void EndScene();

    /// Create a snapshot of the last rendered frame and save it to the provided file.
    /// The file extension determines the image format.
    virtual void WriteImageToFile(const std::string& filename) override;

  private:
    /// Create the ChIrrNodes for all visual model instances in the specified assembly.
    void CreateIrrNodes(const ChAssembly* assembly, std::unordered_set<const ChAssembly*>& trace);

    /// Create the ChIrrNode for the visual model instance of the specified physics item.
    void CreateIrrNode(std::shared_ptr<ChPhysicsItem> item);

    /// Populate the ChIrrNode for the visual model instance of the specified physics item.
    void PopulateIrrNode(irr::scene::ISceneNode* node,
                         std::shared_ptr<ChVisualModel> model,
                         const ChFrame<>& parent_frame);

    /// Add shadow to an Irrlicht node.
    void AddShadowToIrrNode(irr::scene::ISceneNode* node);

    /// Update.
    virtual void Update() override;

    std::unordered_map<ChPhysicsItem*, std::shared_ptr<ChIrrNode>> m_nodes;

    bool m_yup;                                        ///< use Y-up if true, Z-up if false
    std::string m_win_title;                           ///< window title
    irr::SIrrlichtCreationParameters m_device_params;  ///< Irrlicht device parameters
    irr::IrrlichtDevice* m_device;                     ///< Irrlicht visualization device
    irr::scene::ISceneNode* m_container;               ///< Irrlicht scene container
    std::unique_ptr<ChIrrGUI> m_gui;                   ///< associated Irrlicht GUI and event receiver
    std::unique_ptr<EffectHandler> m_effect_handler;   ///< effect handler for shadow maps
    bool m_use_effects;                                ///< flag to enable/disable effects


    // shared meshes
    irr::scene::IAnimatedMesh* sphereMesh;
    irr::scene::IMesh* cubeMesh;
    irr::scene::IMesh* cylinderMesh;
    irr::scene::IMesh* capsuleMesh;
};

}  // namespace irrlicht
}  // namespace chrono

#endif

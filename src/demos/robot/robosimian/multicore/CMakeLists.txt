# Demo programs for multicore RoboSimian simulations
# ------------------------------------------------------------------------------

if(NOT ENABLE_MODULE_MULTICORE OR NOT ENABLE_MODULE_OPENGL)
  message(STATUS "  Warning: the multicore RoboSimian projects require Chrono::Multicore and Chrono::OpenGL!")
  return()
endif()

# ------------------------------------------------------------------------------
# Additional include paths and libraries
# ------------------------------------------------------------------------------

include_directories(${CH_MULTICORE_INCLUDES})
include_directories(${CH_OPENGL_INCLUDES})

# ------------------------------------------------------------------------------

set(DEMOS
    demo_ROBOT_RoboSimian_Granular
)

set(UTILITIES
    granular.h
    granular.cpp
)

# ------------------------------------------------------------------------------
# Add all executables
# ------------------------------------------------------------------------------

foreach(PROGRAM ${DEMOS})
    message(STATUS "...add ${PROGRAM}")
    add_executable(${PROGRAM} "${PROGRAM}.cpp" ${UTILITIES})
    source_group(""  FILES "${PROGRAM}.cpp" ${UTILITIES})

    set_target_properties(${PROGRAM} PROPERTIES
        FOLDER tests
        LINK_FLAGS "${CH_LINKERFLAG_EXE}")

	target_compile_options(${PROGRAM}
		PRIVATE ${CH_CXX_FLAGS}
		PRIVATE ${CH_MULTICORE_CXX_FLAGS})

    set_property(TARGET ${PROGRAM} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROGRAM}>")

    target_link_libraries(${PROGRAM}
                          ChronoEngine
                          ChronoEngine_multicore
                          ChronoEngine_vehicle
                          ChronoEngine_opengl
                          ChronoModels_robot)

    install(TARGETS ${PROGRAM} DESTINATION ${CH_INSTALL_DEMO})
endforeach()

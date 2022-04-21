#=============================================================================
# CMake configuration file for the TwoCars demo.
# This example program requires Irrlicht run-time visualization.
#=============================================================================

IF(NOT ENABLE_MODULE_IRRLICHT)
    RETURN()
ENDIF()

#--------------------------------------------------------------
# List all model files for this demo

SET(PROGRAM demo_VEH_TwoCars)

#--------------------------------------------------------------
# Additional include directories

INCLUDE_DIRECTORIES(${CH_IRRLICHTINC})

#--------------------------------------------------------------
# Create the executable

MESSAGE(STATUS "...add ${PROGRAM}")

ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cpp ${MODEL_FILES})
SOURCE_GROUP("" FILES ${PROGRAM}.cpp)

SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES 
                      LINK_FLAGS "${LINKERFLAG_EXE}")

TARGET_COMPILE_OPTIONS(${PROGRAM}
	PRIVATE ${CH_CXX_FLAGS}
	PRIVATE ${CH_IRRLICHT_CXX_FLAGS})

if(APPLE)
   set_target_properties(${PROGRAM} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST})
endif()
SET_PROPERTY(TARGET ${PROGRAM} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROGRAM}>")
TARGET_LINK_LIBRARIES(${PROGRAM}
                      ChronoEngine
                      ChronoEngine_irrlicht
                      ChronoEngine_vehicle
                      ChronoEngine_vehicle_irrlicht
                      ChronoModels_vehicle)

INSTALL(TARGETS ${PROGRAM} DESTINATION ${CH_INSTALL_DEMO})

#--------------------------------------------------------------
set(DISTRIBUTED_TESTS
    demo_DISTR_rotgrav
    demo_DISTR_scaling
	demo_DISTR_wavetank
)

set(LINKER_FLAGS "${CH_LINKERFLAG_EXE}")
set(LIBRARIES
    ChronoEngine
    ChronoEngine_multicore
    ChronoEngine_distributed)

include_directories(${CH_DISTRIBUTED_INCLUDES} ${CH_MULTICORE_INCLUDES})

message(STATUS "Demo programs for DISTRIBUTED module...")

#--------------------------------------------------------------

foreach(PROGRAM ${DISTRIBUTED_TESTS})
    message(STATUS "...add ${PROGRAM}")

    add_executable(${PROGRAM}  "${PROGRAM}.cpp")
    source_group(""  FILES  "${PROGRAM}.cpp")

    set_target_properties(${PROGRAM} PROPERTIES
        FOLDER tests
        LINK_FLAGS "${LINKER_FLAGS}")
	target_compile_options(${PROGRAM}
		PRIVATE ${CH_CXX_FLAGS}
		PRIVATE ${CH_DISTRIBUTED_CXX_FLAGS})
    set_property(TARGET ${PROGRAM} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROGRAM}>")
    target_link_libraries(${PROGRAM} ${LIBRARIES})
	add_dependencies(${PROGRAM} ${LIBRARIES})

	install(TARGETS ${PROGRAM} DESTINATION ${CH_INSTALL_DEMO})
endforeach()

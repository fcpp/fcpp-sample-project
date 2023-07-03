cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
option(FCPP_INTERNAL_TESTS "Build internal tests for FCPP." OFF)
add_subdirectory(./fcpp/src)
fcpp_setup()

project(
    sample_project
    VERSION 0.0.0
    DESCRIPTION "Sample project to help setup of FCPP-based projects."
)

fcpp_target(./run/apartment_walk.cpp                ON)
fcpp_target(./run/channel_broadcast.cpp             ON)
fcpp_target(./run/collection_compare.cpp            OFF)
fcpp_target(./run/message_dispatch.cpp              ON)
fcpp_target(./run/spreading_collection_batch.cpp    OFF)
fcpp_target(./run/spreading_collection_gui.cpp      ON)
fcpp_target(./run/spreading_collection_mpi.cpp      ON)
fcpp_target(./run/spreading_collection_run.cpp      OFF)

fcpp_test(./test/tester.cpp)

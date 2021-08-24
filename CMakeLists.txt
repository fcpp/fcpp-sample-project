cmake_minimum_required(VERSION 3.9)
option(FCPP_INTERNAL_TESTS "Build internal tests for FCPP." OFF)
add_subdirectory(./fcpp/src)
fcpp_setup()

project(
    sample_project
    VERSION 0.0.0
    DESCRIPTION "Sample project to help setup of FCPP-based projects."
)

fcpp_target(channel_broadcast           ./run/channel_broadcast.cpp             ON)
fcpp_target(collection_compare          ./run/collection_compare.cpp            OFF)
fcpp_target(spreading_collection_batch  ./run/spreading_collection_batch.cpp    OFF)
fcpp_target(spreading_collection_gui    ./run/spreading_collection_gui.cpp      ON)
fcpp_target(spreading_collection_run    ./run/spreading_collection_run.cpp      OFF)

fcpp_test(tester ./test/tester.cpp)

cmake_minimum_required(VERSION 3.9)
add_subdirectory(./fcpp/src)
fcpp_setup()

project(
    sample_project
    VERSION 0.0.0
    DESCRIPTION "Sample project to help setup of FCPP-based projects."
)

fcpp_target(spreading_collection ./run/spreading_collection.cpp ON)
fcpp_target(channel_broadcast    ./run/channel_broadcast.cpp    ON)
fcpp_target(collection_compare   ./run/collection_compare.cpp   OFF)

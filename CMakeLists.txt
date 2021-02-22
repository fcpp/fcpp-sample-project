# CMake 3.9 or newer
cmake_minimum_required(VERSION 3.9)

# Compiling options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
if(WIN32)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")
endif(WIN32)

# Adding fcpp
option(FCPP_BUILD_GL "Build OpenGL features." ON) # this tells CMake to compile fcpp's OpenGL features
add_subdirectory(./fcpp/src/)
target_compile_options(fcpp PUBLIC "-DFCPP_CONFIGURATION=FCPP_CONFIGURATION_DEPENDENT")
#target_compile_options(fcpp PUBLIC "-DPROFILER")

# SPREADING COLLECTION
project(spreading_collection
    VERSION 0.1.0
    DESCRIPTION "Simulation of combined spreading and collection blocks."
)
add_executable(spreading_collection ./run/spreading_collection.cpp)
set_target_properties(spreading_collection PROPERTIES VERSION &{PROJECT_VERSION})
target_link_libraries(spreading_collection PRIVATE fcpp)
target_include_directories(spreading_collection PRIVATE ./)
copyGraphicsFolders(spreading_collection $<TARGET_FILE_DIR:fcpp> $<TARGET_FILE_DIR:spreading_collection>)
copyTextureFolder(spreading_collection ${PROJECT_SOURCE_DIR} $<TARGET_FILE_DIR:spreading_collection>)

# SPREADING COLLECTION
project(channel_broadcast
    VERSION 0.1.0
    DESCRIPTION "Simulation of broadcast through a channel."
)
add_executable(channel_broadcast ./run/channel_broadcast.cpp)
set_target_properties(channel_broadcast PROPERTIES VERSION &{PROJECT_VERSION})
target_link_libraries(channel_broadcast PRIVATE fcpp)
target_include_directories(channel_broadcast PRIVATE ./)
copyGraphicsFolders(channel_broadcast $<TARGET_FILE_DIR:fcpp> $<TARGET_FILE_DIR:channel_broadcast>)
copyTextureFolder(channel_broadcast ${PROJECT_SOURCE_DIR} $<TARGET_FILE_DIR:channel_broadcast>)

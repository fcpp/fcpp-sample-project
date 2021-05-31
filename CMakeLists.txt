cmake_minimum_required(VERSION 3.9)
add_subdirectory(./fcpp/src)
fcpp_gui_setup()


project(
    spreading_collection
    VERSION 0.1.0
    DESCRIPTION "Simulation of combined spreading and collection blocks."
)
fcpp_gui_project(spreading_collection ./run/spreading_collection.cpp)


project(
    channel_broadcast
    VERSION 0.1.0
    DESCRIPTION "Simulation of broadcast through a channel."
)
fcpp_gui_project(channel_broadcast ./run/channel_broadcast.cpp)

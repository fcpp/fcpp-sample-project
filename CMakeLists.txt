cmake_minimum_required(VERSION 3.9)
add_subdirectory(./fcpp/src)
fcpp_setup()


project(
    collection_compare
    VERSION 0.0.0
    DESCRIPTION "Batch simulation comparing performances of collection algorithms."
)
fcpp_project(collection_compare ./run/collection_compare.cpp)


project(
    spreading_collection
    VERSION 0.0.0
    DESCRIPTION "Graphical simulation of combined spreading and collection blocks."
)
fcpp_project(spreading_collection ./run/spreading_collection.cpp)


project(
    channel_broadcast
    VERSION 0.0.0
    DESCRIPTION "Graphical simulation of broadcast through a channel."
)
fcpp_project(channel_broadcast ./run/channel_broadcast.cpp)

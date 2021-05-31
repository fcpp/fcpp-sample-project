// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection.hpp
 * @brief Simple composition of spreading and collection functions.
 */

#ifndef FCPP_SPREADING_COLLECTION_H_
#define FCPP_SPREADING_COLLECTION_H_

#include "lib/beautify.hpp"
#include "lib/coordination.hpp"
#include "lib/data.hpp"
#include "lib/simulation_setup.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {


namespace tags {
    //! @brief Distance of the current node.
    struct my_distance {};

    //! @brief Diameter of the network (in the source).
    struct source_diameter {};

    //! @brief Diameter of the network (in every node).
    struct diameter {};

    //! @brief Color representing the distance of the current node.
    struct distance_c {};

    //! @brief Color representing the diameter of the network (in the source).
    struct source_diameter_c {};

    //! @brief Color representing the diameter of the network (in every node).
    struct diameter_c {};

    //! @brief Size of the current node.
    struct size {};

    //! @brief Shape of the current node.
    struct node_shape {};
}


//! @brief Main function.
MAIN() {
    rectangle_walk(CALL, make_vec(0,0,0), make_vec(side,side,height), comm/3, 1);
    device_t source_id = ((int)node.current_time()) / 50;
    bool is_source = node.uid == source_id;
    node.storage(tags::size{}) = is_source ? 20 : 10;
    double dist = abf_distance(CALL, is_source);
    double sdiam = mp_collection(CALL, dist, dist, 0.0, [](double x, double y){
        return max(x, y);
    }, [](double x, int){
        return x;
    });
    double diam = broadcast(CALL, dist, sdiam);
    node.storage(tags::my_distance{}) = dist;
    node.storage(tags::source_diameter{}) = sdiam;
    node.storage(tags::diameter{}) = diam;
    node.storage(tags::distance_c{}) = color::hsva(dist*hue_scale, 1, 1);
    node.storage(tags::source_diameter_c{}) = color::hsva(sdiam*hue_scale, 1, 1);
    node.storage(tags::diameter_c{}) = color::hsva(diam*hue_scale, 1, 1);
    node.storage(tags::node_shape{}) = is_source ? shape::cube : shape::sphere;
}


}


}

#endif // FCPP_SPREADING_COLLECTION_H_

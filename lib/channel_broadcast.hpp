// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file channel_broadcast.hpp
 * @brief Broadcasting information through an elliptical channel.
 */

#ifndef FCPP_CHANNEL_BROADCAST_H_
#define FCPP_CHANNEL_BROADCAST_H_

#include "lib/beautify.hpp"
#include "lib/coordination.hpp"
#include "lib/data.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief Minimum number whose square is at least n.
constexpr size_t discrete_sqrt(size_t n) {
    size_t lo = 0, hi = n, mid = 0;
    while (lo < hi) {
        mid = (lo + hi)/2;
        if (mid*mid < n) lo = mid+1;
        else hi = mid;
    }
    return lo;
}

//! @brief Number of devices.
constexpr size_t devices = 1000;

//! @brief Communication radius.
constexpr size_t comm = 100;

//! @brief Side of the deployment area.
constexpr size_t side = discrete_sqrt(devices * 3000);

//! @brief Height of the deployment area.
constexpr size_t height = 100;

//! @brief Color hue scale.
constexpr float hue_scale = 360.0f/(side+height);


//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {


namespace tags {
    //! @brief Distance to the source node.
    struct source_distance {};

    //! @brief Distance to the destination node.
    struct dest_distance {};

    //! @brief Color representing the minimal distance of the current node.
    struct distance_c {};

    //! @brief Size of the current node.
    struct size {};

//! @brief Shape of the current node.
    struct node_shape {};
}


//! @brief Selects an elliptical channel of given width between a source and destination.
FUN bool channel(ARGS, bool source, bool dest, double width) { CODE
    double ds = bis_distance(CALL, source, 1, 100);
    double dd = bis_distance(CALL, dest, 1, 100);
    node.storage(tags::source_distance{}) = ds;
    node.storage(tags::dest_distance{}) = dd;
    bool c = ds + dd < broadcast(CALL, ds, dd) + width;
    c = c or source or dest;
    node.storage(tags::distance_c{}) = c ? color::hsva(min(ds,dd)*hue_scale, 1, 1) : color();
    node.storage(tags::node_shape{}) = source or dest ? shape::tetrahedron : c ? shape::icosahedron : shape::sphere;
    return c;
}

//! @brief Main function.
MAIN() {
    rectangle_walk(CALL, make_vec(0,0,0), make_vec(side,side,height), 10, 1);
    device_t src_id = 0;
    device_t dst_id = 1;
    bool is_src = node.uid == src_id;
    bool is_dst = node.uid == dst_id;
    channel(CALL, is_src, is_dst, 20);
    node.storage(tags::size{}) = is_src or is_dst ? 30 : 10;
}


}


}

#endif // FCPP_CHANNEL_BROADCAST_H_

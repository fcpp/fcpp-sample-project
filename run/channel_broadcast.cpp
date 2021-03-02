// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"
#include "lib/simulation/displayer.hpp"

#define EXAMPLE_3D

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

constexpr size_t devices = 1000;
constexpr size_t comm = 100;
constexpr size_t side = discrete_sqrt(devices * 3000);
constexpr size_t height = 100;

constexpr float hue_scale = 360.0f/(side+height);


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

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
}

FUN bool channel(ARGS, bool source, bool dest, double width) { CODE
    double ds = bis_distance(CALL, source, 1, 100);
    double dd = bis_distance(CALL, dest, 1, 100);
    node.storage(tags::source_distance{}) = ds;
    node.storage(tags::dest_distance{}) = dd;
    bool c = ds + dd < broadcast(CALL, ds, dd) + width;
    c = c or source or dest;
    node.storage(tags::distance_c{}) = c ? color::hsva(min(ds,dd)*hue_scale, 1, 1) : color();
    return c;
}

//! @brief Main function.
MAIN() {
#ifdef EXAMPLE_3D
    rectangle_walk(CALL, make_vec(0,0,0), make_vec(side,side,height), 10, 1);
#else
    rectangle_walk(CALL, make_vec(0,0), make_vec(side,side), 10, 1);
#endif
    device_t src_id = 0;
    device_t dst_id = 1;
    bool is_src = node.uid == src_id;
    bool is_dst = node.uid == dst_id;
    bool is_chn = channel(CALL, is_src, is_dst, 20);
    node.storage(tags::size{}) = is_src or is_dst ? 30 : 10;
}

}


//! @brief Namespace for all FCPP components.
namespace component {

/**
 * @brief Combination of components for interactive simulations.
 *
 * It can be instantiated as `interactive_simulator<options...>::net`.
 */
DECLARE_COMBINE(interactive_simulator, displayer, calculus, simulated_connector, simulated_positioner, timer, scheduler, logger, storage, spawner, identifier, randomizer);

}

}

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,
    distribution::weibull_n<times_t, 10, 1, 10>
>;

#ifdef EXAMPLE_3D
using rectangle_d = distribution::rect_n<1, 0, 0, 0, side, side, height>;
constexpr size_t dim = 3;
#else
using rectangle_d = distribution::rect_n<1, 0, 0, side, side>;
constexpr size_t dim = 2;
#endif

DECLARE_OPTIONS(opt,
    parallel<true>,
    synchronised<false>,
    program<coordination::main>,
    round_schedule<round_s>,
    dimension<dim>,
    exports<vec<dim>, double, tuple<double,double>>,
    log_schedule<sequence::periodic_n<1, 0, 1>>,
    tuple_store<
        source_distance,    double,
        dest_distance,      double,
        distance_c,         color,
        size,               double
    >,
    spawn_schedule<sequence::multiple_n<devices, 0>>,
    init<x, rectangle_d>,
    connector<connect::fixed<comm, 1, dim>>,
    size_tag<size>,
    color_tag<distance_c>,
    shape_val<(size_t)shape::sphere>
);

int main() {
    component::interactive_simulator<opt>::net network{common::make_tagged_tuple<name,epsilon,texture>("Broadcast through an Elliptic Channel", 0.1, "land.jpg")};
    network.run();
    return 0;
}

// Copyright © 2023 Gianmarco Rampulla. All Rights Reserved.

/**
 * @file apartment_walk.cpp
 * @brief Minimal experiment for the navigator component.
 */

// [INTRODUCTION]
//! Importing the FCPP library.
#include "lib/fcpp.hpp"

/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

//! @brief Dummy ordering between positions (allows positions to be used as secondary keys in ordered tuples).
template <size_t n>
bool operator<(vec<n> const& a, vec<n> const& b) {
    for(int i = 0; i < n; i++)
        if (a[i] >= b[i]) return  false;
    return true;
}

    //! @brief Dimensionality of the space.
constexpr size_t dim = 3;
//! @brief Side of the deployment area.
constexpr size_t width = 850;
//! @brief Height of the deployment area.
constexpr size_t height = 500;
//! @brief Tallness of the deployment area.
constexpr size_t tall = 50;

//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {

//! @brief Tags used in the node storage.
namespace tags {
    //! @brief Color of the current node.
    struct node_color {};
    //! @brief Size of the current node.
    struct node_size {};
    //! @brief Shape of the current node.
    struct node_shape {};
    //! @brief Speed of the current node
    struct speed {};
    //! @brief Coordinates of nearest obstacle
    struct nearest_obstacle {};
    //! @brief Distance from nearest obstacle
    struct distance_from_obstacle {};
    //! @brief Delta X from nearest obstacle
    struct obstacle_delta_x {};
    //! @brief Delta Y from nearest obstacle
    struct obstacle_delta_y {};
    //! @brief Distance from closest neighbour
    struct distance_min_nbr {};
}


//! @brief Main function.
MAIN() {

    node.storage(tags::node_size{}) = 10;
    node.storage(tags::node_color{}) = color(TAN);
    node.storage(tags::node_shape{}) = shape::sphere;

    // used to set position of out of bound nodes at the start
    if (coordination::counter(CALL) == 1) {
        if (node.net.is_obstacle(node.position())) {
            auto p2 = node.net.closest_space(node.position());
            int deltaX, deltaY, size = node.storage(tags::node_size{});
            if((p2 - node.position())[0] > 0) deltaX = +size; else deltaX = -size;
            if((p2 - node.position())[1] > 0) deltaY = +size; else deltaY = -size;
            node.position() = make_vec(p2[0] + deltaX, p2[1] + deltaY, tall);
        }
    }

    auto closest = node.net.closest_obstacle(node.position());
    real_t dist1 = distance(closest, node.position());
    real_t min_neighbor_dist = min_hood(CALL, node.nbr_dist(),std::numeric_limits<real_t>::max());

    node.storage(tags::nearest_obstacle{}) = closest;
    node.storage(tags::distance_from_obstacle{}) = dist1;
    node.storage(tags::distance_min_nbr{}) = min_neighbor_dist;

    if (dist1 <= 30) {
        node.velocity() = make_vec(0,0,0);
        node.propulsion() = make_vec(0,0,0);
        node.propulsion() += -coordination::point_elastic_force(CALL,closest,1,0.10);
        if (min_neighbor_dist <= 25) {
            node.velocity() = make_vec(0,0,0);
            node.propulsion() += -coordination::neighbour_elastic_force(CALL, 0.05, 0.05);
        }
    }
    else {
        if (min_neighbor_dist <= 25) {
            node.propulsion() = make_vec(0,0,0);
            node.velocity() = make_vec(0,0,0);
            node.propulsion() += -coordination::neighbour_elastic_force(CALL, 0.05, 0.05);
        }
        else {
            node.propulsion() = make_vec(0,0,0);
            rectangle_walk(CALL, make_vec(0, 0, tall), make_vec(width, height, tall), node.storage(tags::speed{}), 1);
        }
    }



}
//! @brief Export types used by the main function (update it when expanding the program).
FUN_EXPORT main_t = common::export_list<double, int, rectangle_walk_t<dim>>;

} // namespace coordination

// [SYSTEM SETUP]

//! @brief Namespace for component options.
namespace option {

//! @brief Import tags to be used for component options.
using namespace component::tags;
//! @brief Import tags used by aggregate functions.
using namespace coordination::tags;

using fcpp::dim;
using fcpp::width;
using fcpp::height;

//! @brief Number of people in the area.
constexpr int node_num = 10;

//! @brief Description of the round schedule.
using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,    // uniform time in the [0,1] interval for start
    distribution::weibull_n<times_t, 10, 1, 10> // weibull-distributed time for interval (10/10=1 mean, 1/10=0.1 deviation)
>;
//! @brief The sequence of network snapshots (one every simulated second).
using log_s = sequence::periodic_n<1, 0, 1>;
//! @brief The sequence of node generation events (node_num devices all generated at time 0).
using spawn_s = sequence::multiple_n<node_num, 0>;
//! @brief The distribution of initial node positions (random in a 850x500 square).
using rectangle_d = distribution::rect_n<1, 0, 0, tall, width, height, tall>;
//! @brief The distribution of node speeds (all equal to a fixed value).
using speed_d = distribution::constant_i<double, speed>;
//! @brief The contents of the node storage as tags and associated types.
using store_t = tuple_store<
    nearest_obstacle,           vec<dim>,
    distance_from_obstacle,     real_t,
    obstacle_delta_x,           real_t,
    obstacle_delta_y,           real_t,
    distance_min_nbr,           real_t,
    speed,                      double,
    node_color,                 color,
    node_size,                  double,
    node_shape,                 shape
>;
//! @brief The tags and corresponding aggregators to be logged (change as needed).
using aggregator_t = aggregators<
    node_size,                  aggregator::mean<double>
>;

//! @brief The general simulation options.
DECLARE_OPTIONS(list,
    parallel<true>,      // multithreading enabled on node rounds
    synchronised<false>, // optimise for asynchronous networks
    program<coordination::main>,   // program to be run (refers to MAIN above)
    exports<coordination::main_t>, // export type list (types used in messages)
    retain<metric::retain<2,1>>,   // messages are kept for 2 seconds before expiring
    round_schedule<round_s>, // the sequence generator for round events on nodes
    log_schedule<log_s>,     // the sequence generator for log events on the network
    spawn_schedule<spawn_s>, // the sequence generator of node creation events on the network
    store_t,       // the contents of the node storage
    aggregator_t,  // the tags and corresponding aggregators to be logged
    init<
        x,     rectangle_d, // initialise position randomly in a rectangle for new nodes
        speed, speed_d
    >,
    dimension<dim>, // dimensionality of the space
    connector<connect::fixed<100, 1, dim>>, // connection allowed within a fixed comm range
    shape_tag<node_shape>, // the shape of a node is read from this tag in the store
    size_tag<node_size>,   // the size  of a node is read from this tag in the store
    color_tag<node_color>,  // the color of a node is read from this tag in the store
    area<0,0,width,height>
);

} // namespace option

} // namespace fcpp

//! @brief The main function.
int main() {
    using namespace fcpp;
    //! @brief The network object type (interactive simulator with given options).
    using net_t = component::interactive_simulator<option::list>::net;
    //! @brief The initialisation values (simulation name).
    auto init_v = common::make_tagged_tuple<option::name, option::texture, option::obstacles, option::speed, option::obstacles_color_threshold>("Simulated map test", "apartment.jpg", "apartment.jpg", 3, 0.8);
    //! @brief Construct the network object.
    net_t network{init_v};
    //! @brief Run the simulation until exit.
    network.run();
    return 0;
}

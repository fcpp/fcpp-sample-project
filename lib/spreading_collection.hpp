// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection.hpp
 * @brief Simple composition of spreading and collection functions.
 *
 * This header file is designed to work under multiple execution paradigms.
 */

#ifndef FCPP_SPREADING_COLLECTION_H_
#define FCPP_SPREADING_COLLECTION_H_

#include "lib/fcpp.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief The final simulation time.
constexpr size_t end_time = 300;
//! @brief Communication radius.
constexpr size_t comm = 100;
//! @brief Dimensionality of the space.
constexpr size_t dim = 3;
//! @brief Height of the deployment area.
constexpr size_t height = comm;


//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {


//! @brief Tags used in the node storage.
namespace tags {
    //! @brief The variance of round timing in the network.
    struct tvar {};
    //! @brief The number of hops in the network.
    struct hops {};
    //! @brief The density of devices.
    struct dens {};
    //! @brief The movement speed of devices.
    struct speed {};
    //! @brief The number of devices.
    struct devices {};
    //! @brief The side of deployment area.
    struct side {};
    //! @brief The factor producing hues from distances.
    struct hue_scale {};

    //! @brief True distance of the current node from the source.
    struct true_distance {};
    //! @brief Computed distance of the current node from the source.
    struct calc_distance {};
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
    struct node_size {};
    //! @brief Shape of the current node.
    struct node_shape {};
}


//! @brief Function selecting a source based on the current time.
FUN bool select_source(ARGS, int step) { CODE
    // the source ID increases by 1 every "step" seconds
    device_t source_id = ((int)node.current_time()) / step;
    bool is_source = node.uid == source_id;
    // retrieves from the net object the current true position of the source
    vec<3> source_pos = node.position();
    if (node.net.node_count(source_id))
        source_pos = node.net.node_at(source_id).position(node.current_time());
    // store relevant values in the node storage
    node.storage(tags::true_distance{})     = distance(node.position(), source_pos);
    node.storage(tags::node_size{})         = is_source ? 20 : 10;
    node.storage(tags::node_shape{})        = is_source ? shape::star : shape::sphere;
    return is_source;
}
//! @brief Export types used by the select_source function (none).
FUN_EXPORT select_source_t = common::export_list<>;


//! @brief Main function.
MAIN() {
    // access stored constants
    double const& side      = node.storage(tags::side{});
    double const& speed     = node.storage(tags::speed{});
    double const& hue_scale = node.storage(tags::hue_scale{});
    // random walk into a given rectangle with given speed
    rectangle_walk(CALL, make_vec(0,0,0), make_vec(side,side,height), speed, 1);
    // selects a different source every 50 simulated seconds
    bool is_source = select_source(CALL, 50);
    // calculate distances from the source
    double dist = abf_distance(CALL, is_source);
    // collect the maximum finite distance (diameter) back towards the source
    double sdiam = mp_collection(CALL, dist, dist, 0.0, [](double x, double y){
        x = isfinite(x) ? x : 0;
        y = isfinite(y) ? y : 0;
        return max(x, y);
    }, [](double x, int){
        return x;
    });
    // broadcast the diameter computed in the source to the whole network
    double diam = broadcast(CALL, dist, sdiam);
    // store relevant values in the node storage
    node.storage(tags::calc_distance{})     = dist;
    node.storage(tags::source_diameter{})   = sdiam;
    node.storage(tags::diameter{})          = diam;
    // store colors, using the values to regulate hue (with full saturation and value)
    node.storage(tags::distance_c{})        = color::hsva(dist *hue_scale, 1, 1);
    node.storage(tags::source_diameter_c{}) = color::hsva(sdiam*hue_scale, 1, 1);
    node.storage(tags::diameter_c{})        = color::hsva(diam *hue_scale, 1, 1);
}
//! @brief Export types used by the main function.
FUN_EXPORT main_t = common::export_list<rectangle_walk_t<3>, select_source_t, abf_distance_t, mp_collection_t<double, double>, broadcast_t<double, double>>;


} // namespace coordination


//! @brief Namespace for component options.
namespace option {


//! @brief Import tags to be used for component options.
using namespace component::tags;
//! @brief Import tags used by aggregate functions.
using namespace coordination::tags;


//! @brief The randomised sequence of rounds for every node (about one every second, with 10% variance).
using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>, // uniform time in the [0,1] interval for start
    distribution::weibull< // weibull-distributed time for interval (mean 1, deviation equal to tvar divided by 100)
        distribution::constant_n<double, 1>,
        functor::div<distribution::constant_i<double, tvar>, distribution::constant_n<double, 100>>
    >,
    distribution::constant_n<times_t, end_time+2>  // the constant end_time+2 number for end
>;
//! @brief The sequence of network snapshots (one every simulated second).
using log_s = sequence::periodic_n<1, 0, 1, end_time>;
//! @brief The sequence of node generation events (multiple devices all generated at time 0).
using spawn_s = sequence::multiple<
    distribution::constant_i<size_t, devices>,
    distribution::constant_n<double, 0>
>;
//! @brief The distribution of initial node positions (random in a given rectangle).
using rectangle_d = distribution::rect<
    distribution::constant_n<double, 0>,
    distribution::constant_n<double, 0>,
    distribution::constant_n<double, 0>,
    distribution::constant_i<double, side>,
    distribution::constant_i<double, side>,
    distribution::constant_n<double, height>
>;
//! @brief The distribution of sides (all equal to a fixed value).
using side_d = distribution::constant_i<double, side>;
//! @brief The distribution of hue scale (all equal to a fixed value).
using hue_d  = functor::div<
    distribution::constant_n<double, 360>,
    functor::add<distribution::constant_i<double, side>, distribution::constant_n<double, height>>
>;
//! @brief The distribution of node speeds (all equal to a fixed value).
using speed_d = functor::mul<
    distribution::constant_i<double, speed>,
    distribution::constant_n<double, comm, 100>
>;
//! @brief The contents of the node storage as tags and associated types.
using store_t = tuple_store<
    side,               double,
    hue_scale,          double,
    speed,              double,
    true_distance,      double,
    calc_distance,      double,
    source_diameter,    double,
    diameter,           double,
    distance_c,         color,
    source_diameter_c,  color,
    diameter_c,         color,
    node_shape,         shape,
    node_size,          double
>;
//! @brief The tags and corresponding aggregators to be logged.
using aggregator_t = aggregators<
    true_distance,      aggregator::max<double>,
    diameter,           aggregator::combine<
                            aggregator::min<double>,
                            aggregator::mean<double>,
                            aggregator::max<double>
                        >
>;
//! @brief The aggregator to be used on logging rows for plotting.
using row_aggregator_t = common::type_sequence<aggregator::mean<double>>;
//! @brief The logged values to be shown in plots as lines (true_distance, diameter).
using points_t = plot::values<aggregator_t, row_aggregator_t, true_distance, diameter>;
//! @brief A plot of the logged values by time for tvar,dens,hops,speed = 10 (default values).
using time_plot_t = plot::split<plot::time, plot::filter<tvar, filter::equal<10>, dens, filter::equal<10>, hops, filter::equal<10>, speed, filter::equal<10>, points_t>>;
//! @brief A plot of the logged values by tvar for times >= 50 (after the first source switch).
using tvar_plot_t = plot::split<tvar, plot::filter<plot::time, filter::above<50>, dens, filter::equal<10>, hops, filter::equal<10>, speed, filter::equal<10>, points_t>>;
//! @brief A plot of the logged values by dens for times >= 50 (after the first source switch).
using dens_plot_t = plot::split<dens, plot::filter<plot::time, filter::above<50>, tvar, filter::equal<10>, hops, filter::equal<10>, speed, filter::equal<10>, points_t>>;
//! @brief A plot of the logged values by hops for times >= 50 (after the first source switch).
using hops_plot_t = plot::split<hops, plot::filter<plot::time, filter::above<50>, tvar, filter::equal<10>, dens, filter::equal<10>, speed, filter::equal<10>, points_t>>;
//! @brief A plot of the logged values by speed for times >= 50 (after the first source switch).
using speed_plot_t = plot::split<speed, plot::filter<plot::time, filter::above<50>, tvar, filter::equal<10>, dens, filter::equal<10>, hops, filter::equal<10>, points_t>>;
//! @brief Combining the plots into a single row.
using plot_t = plot::join<time_plot_t, tvar_plot_t, dens_plot_t, hops_plot_t, speed_plot_t>;


//! @brief The general simulation options.
DECLARE_OPTIONS(list,
    parallel<false>,     // no multithreading on node rounds
    synchronised<false>, // optimise for asynchronous networks
    program<coordination::main>,   // program to be run (refers to MAIN above)
    exports<coordination::main_t>, // export type list (types used in messages)
    round_schedule<round_s>, // the sequence generator for round events on nodes
    log_schedule<log_s>,     // the sequence generator for log events on the network
    spawn_schedule<spawn_s>, // the sequence generator of node creation events on the network
    store_t,       // the contents of the node storage
    aggregator_t,  // the tags and corresponding aggregators to be logged
    init<
        x,          rectangle_d, // initialise position randomly in a rectangle for new nodes
        side,       side_d,      // initialise side with the globally provided simulation area side
        hue_scale,  hue_d,       // initialise hue_scale based on globally provided area side
        speed,      speed_d      // initialise speed with the globally provided speed for new nodes
    >,
    // general parameters to use for plotting
    extra_info<
        tvar,   double,
        dens,   double,
        hops,   double,
        speed,  double
    >,
    plot_type<plot_t>, // the plot description to be used
    dimension<dim>, // dimensionality of the space
    connector<connect::fixed<comm, 1, dim>>, // connection allowed within a fixed comm range
    shape_tag<node_shape>, // the shape of a node is read from this tag in the store
    size_tag<node_size>,   // the size of a node is read from this tag in the store
    color_tag<distance_c, source_diameter_c, diameter_c> // colors of a node are read from these
);


} // namespace option


} // namespace fcpp


#endif // FCPP_SPREADING_COLLECTION_H_

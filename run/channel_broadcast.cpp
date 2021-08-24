// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"
#include "lib/channel_broadcast.hpp"

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

constexpr size_t dim = 3;

using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,
    distribution::weibull_n<times_t, 10, 1, 10>
>;

using rectangle_d = distribution::rect_n<1, 0, 0, 0, side, side, height>;

using aggregator_t = aggregators<in_channel, aggregator::mean<double>>;

using plot_t = plot::split<plot::time, plot::values<aggregator_t, common::type_sequence<>, in_channel>>;

DECLARE_OPTIONS(opt,
    parallel<true>,
    synchronised<false>,
    program<coordination::main>,
    exports<coordination::main_t>,
    round_schedule<round_s>,
    log_schedule<sequence::periodic_n<1, 0, 1>>,
    spawn_schedule<sequence::multiple_n<devices, 0>>,
    tuple_store<
        in_channel,         bool,
        source_distance,    double,
        dest_distance,      double,
        distance_c,         color,
        size,               double,
        node_shape,         shape
    >,
    aggregator_t,
    init<
        x,                  rectangle_d
    >,
    plot_type<plot_t>,
    dimension<dim>,
    connector<connect::fixed<comm, 1, dim>>,
    shape_tag<node_shape>,
    size_tag<size>,
    color_tag<distance_c>
);

int main() {
    plot_t p;
    std::cout << "/*\n";
    {
        using net_t = component::interactive_simulator<opt>::net;
        auto init_v = common::make_tagged_tuple<name, epsilon, texture, plotter>(
            "Broadcast through an Elliptic Channel",
            0.1,
            "land.jpg",
            &p
        );
        net_t network{init_v};
        network.run();
    }
    std::cout << "*/\n";
    std::cout << plot::file("channel_broadcast", p.build());
    return 0;
}

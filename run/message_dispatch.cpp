// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"
#include "lib/message_dispatch.hpp"

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

constexpr size_t dim = 3;
constexpr size_t end = 150;

using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,
    distribution::weibull_n<times_t, 10, 1, 10>,
    distribution::constant_n<times_t, end+2>
>;

using rectangle_d = distribution::rect_n<1, 0, 0, 0, side, side, height>;

using aggregator_t = aggregators<
    max_msg,        aggregator::max<size_t>,
    tot_msg,        aggregator::sum<size_t>,
    max_proc,       aggregator::max<size_t>,
    tot_proc,       aggregator::sum<size_t>,
    first_delivery, aggregator::sum<double>,
    sent_count,     aggregator::sum<size_t>,
    delivery_count, aggregator::sum<size_t>,
    repeat_count,   aggregator::sum<size_t>
>;

template <typename... Ts>
using lines_t = plot::join<plot::values<aggregator_t, common::type_sequence<>, Ts>...>;
using plot_t = plot::split<plot::time, lines_t<max_msg, tot_msg, max_proc, tot_proc, first_delivery, sent_count, delivery_count, repeat_count>>;

DECLARE_OPTIONS(opt,
    parallel<false>,
    synchronised<false>,
    program<coordination::main>,
    exports<coordination::main_t>,
    round_schedule<round_s>,
    log_schedule<sequence::periodic_n<1, 0, 1, end>>,
    spawn_schedule<sequence::multiple_n<devices, 0>>,
    tuple_store<
        speed,              double,
        max_msg,            size_t,
        tot_msg,            size_t,
        max_proc,           size_t,
        tot_proc,           size_t,
        first_delivery,     times_t,
        sent_count,         size_t,
        delivery_count,     size_t,
        repeat_count,       size_t,
        center_dist,        double,
        node_color,         color,
        left_color,         color,
        right_color,        color,
        node_size,          double,
        node_shape,         shape
    >,
    aggregator_t,
    init<
        x,                  rectangle_d,
        speed,              distribution::constant_n<double, 5>
    >,
    plot_type<plot_t>,
    dimension<dim>,
    connector<connect::fixed<comm, 1, dim>>,
    message_size<true>,
    shape_tag<node_shape>,
    size_tag<node_size>,
    color_tag<node_color, left_color, right_color>
);

int main() {
    plot_t p;
    std::cout << "/*\n";
    {
        using net_t = component::interactive_simulator<opt>::net;
        auto init_v = common::make_tagged_tuple<name, epsilon, plotter>(
            "Dispatch of Peer-to-peer Messages",
            0.1,
            &p
        );
        net_t network{init_v};
        network.run();
    }
    std::cout << "*/\n";
    std::cout << plot::file("message_dispatch", p.build());
    return 0;
}

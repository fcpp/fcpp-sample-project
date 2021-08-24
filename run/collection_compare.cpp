// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"
#include "lib/collection_compare.hpp"

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

constexpr size_t algo       = 1;
constexpr size_t device_num = 1000;
constexpr size_t end_time   = 500;
constexpr size_t maxX       = 2000;
constexpr size_t maxY       = 200;

using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,
    distribution::weibull_n<times_t, 100, 25, 100>,
    distribution::constant_n<times_t, end_time+2>
>;

using log_s = sequence::periodic_n<1, 0, 10, end_time>;

using spawn_s = sequence::multiple_n<device_num, 0>;

using rectangle_d = distribution::rect_n<1, 0, 0, maxX, maxY>;

DECLARE_OPTIONS(opt,
    parallel<true>,
    synchronised<false>,
    program<coordination::main>,
    exports<coordination::main_t>,
    round_schedule<round_s>,
    log_schedule<log_s>,
    spawn_schedule<spawn_s>,
    tuple_store<
        algorithm,  int,
        spc_sum,    double,
        mpc_sum,    double,
        wmpc_sum,   double,
        ideal_sum,  double,
        spc_max,    double,
        mpc_max,    double,
        wmpc_max,   double,
        ideal_max,  double
    >,
    aggregators<
        spc_sum,    aggregator::sum<double>,
        mpc_sum,    aggregator::sum<double>,
        wmpc_sum,   aggregator::sum<double>,
        ideal_sum,  aggregator::sum<double>,
        spc_max,    aggregator::max<double>,
        mpc_max,    aggregator::max<double>,
        wmpc_max,   aggregator::max<double>,
        ideal_max,  aggregator::max<double>
    >,
    init<
        x,          rectangle_d,
        algorithm,  distribution::constant_n<int, algo>
    >,
    connector<connect::fixed<100>>
);

int main() {
    using net_t = component::batch_simulator<opt>::net;
    auto init_v = common::make_tagged_tuple<epsilon>(0.1);
    net_t network{init_v};
    network.run();
    return 0;
}

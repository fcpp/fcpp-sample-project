// Copyright © 2020 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"

#include "lib/collection_compare.hpp"

using namespace fcpp;
using namespace component::tags;
using namespace coordination::tags;

#define ALGO        1
#define DEVICE_NUM  1000
#define END_TIME    500
#define MAXX        2000
#define MAXY        200

using spawn_s = sequence::multiple<distribution::constant<times_t, 0>, DEVICE_NUM>;

using round_s = sequence::periodic<distribution::interval_t<times_t, 0, 1>, distribution::weibull_t<times_t, 100, 25, 100>, distribution::constant<times_t, END_TIME>>;

using export_s = sequence::periodic<distribution::constant<times_t, 0>, distribution::constant<times_t, 10>, distribution::constant<times_t, END_TIME>>;

using rectangle_d = distribution::vec<distribution::interval_t<double, 0, MAXX>, distribution::interval_t<double, 0, MAXY>>;

DECLARE_OPTIONS(opt,
    program<main>,
    round_schedule<round_s>,
    exports<
        device_t, double, field<double>, vec<2>,
        tuple<double,device_t>, tuple<double,int>, tuple<double,double>
    >,
    log_schedule<export_s>,
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
    spawn_schedule<spawn_s>,
    init<
        x,          rectangle_d,
        algorithm,  distribution::constant<int, ALGO>
    >,
    connector<connect::fixed<100>>
);

int main() {
    batch::run(component::batch_simulator<opt>{});
    return 0;
}

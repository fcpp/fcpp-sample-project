// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include <ctime>
#include <iomanip>
#include <sstream>

#include "lib/spreading_collection.hpp"

using namespace fcpp;

//! @brief Object accumulating the CPU elapsed during its lifetime in a given counter.
class profiler {
  public:
    //! @brief Constructor given a counter name.
    profiler() : start{std::clock()} {}

    //! @brief Computes elapsed time.
    operator double() {
        return (std::clock() - start) * 1.0 / CLOCKS_PER_SEC;
    }

  private:
    //! @brief Stores the clock during construction.
    std::clock_t start;
};


int main(int argc, char** argv) {
    // Construct the plotter object.
    option::plot_t p;
    // The component type (batch simulator with given options).
    using comp_t = component::batch_simulator<option::list>;
    // The list of initialisation values to be used for simulations.
    auto init_list = batch::make_tagged_tuple_sequence(
        batch::arithmetic<option::seed >(0, 99, 1),     // 100 different random seeds
        batch::arithmetic<option::speed>(0, 48, 2, 10), // 25 different speeds
        batch::arithmetic<option::dens >(5, 29, 1, 10), // 25 different densities
        batch::arithmetic<option::hops >(1, 25, 1, 10), // 25 different hop sizes
        batch::arithmetic<option::tvar >(0, 48, 2, 10), // 25 different time variances
        // generate output file name for the run
        batch::stringify<option::output>("output/spreading_collection_mpi", "txt"),
        batch::formula<option::side, size_t>([](auto const& x) {
            double h = common::get<option::hops>(x);
            return h * comm / sqrt(2.0) + 0.5;
        }),
        // computes device number from dens and side
        batch::formula<option::devices, size_t>([](auto const& x) {
            double d = common::get<option::dens>(x);
            double s = common::get<option::side>(x);
            return d*s*s/(3.141592653589793*comm*comm) + 0.5;
        }),
        batch::constant<option::plotter>(&p) // reference to the plotter object
    );
    // Runs the given simulations.
    profiler t;
    batch::mpi_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    std::cerr << double(t) << std::endl;
    // Builds the resulting plots.
    std::cout << plot::file("distributed_batch", p.build());
    // Checks correctness.
    if (p != option::plot_t{}) {
        option::plot_t q;
        swap(p, q);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        std::stringstream sp, sq;
        sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
        sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
        if (sp.str() == sq.str()) {
            std::cerr << "Test succeeded!" << std::endl;
        } else {
            std::cerr << "Test failed!" << std::endl;
            std::cout << sp.str();
        }
    }
    return 0;
}

// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include <iomanip>
#include <sstream>

#include "lib/spreading_collection.hpp"

using namespace fcpp;


int main(int argc, char** argv) {
    //! @brief Construct the plotter object.
    option::plot_t p;
    //! @brief The component type (batch simulator with given options).
    using comp_t = component::batch_simulator<option::list>;
    //! @brief The list of initialisation values to be used for simulations.
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
    //! @brief Runs the given simulations.
    int provided, initialized, rank;
    MPI_Initialized(&initialized);
    if (not initialized) {
        MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
        assert(provided == MPI_THREAD_MULTIPLE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    if (not initialized) MPI_Finalize();
    //! @brief Builds the resulting plots.
    if (p != option::plot_t{}) {
        option::plot_t q;
        swap(p, q);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        std::stringstream sp, sq;
        sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
        sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
        std::cerr << (sp.str() == sq.str() ? "Test succeeded!" : "Test failed!") << std::endl;
        std::cout << sp.str();
        std::cout << sq.str();
    }
    return 0;
}

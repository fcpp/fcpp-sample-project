// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include "lib/spreading_collection.hpp"
#include <mpi.h>

using namespace fcpp;


void validate_result(option::plot_t& p) {
    int n_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    option::plot_t q;
        //! @brief The component type (batch simulator with given options).
		using comp_t = component::batch_simulator<option::list>;
        //! @brief The list of initialisation values to be used for simulations.

        auto init_list = batch::make_tagged_tuple_sequence(
            batch::arithmetic<option::seed>(0, 9, 1),                   // 10 different random seeds
            batch::arithmetic<option::speed>(size_t(0), comm/2, comm/20), // 11 different speeds
            // generate output file name for the run
            batch::stringify<option::output>("output/spreading_collection_batch", "txt"),
            batch::constant<option::plotter>(&q)                        // reference to the plotter object
        );
        //! @brief Runs the given simulations.
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    
    if (p == q)
        std::cerr << "Test succeed!" << std::endl;
    else
        std::cerr << "Test failed!" << std::endl; 
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    //! @brief Construct the plotter object.
    option::plot_t p;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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
    batch::mpi_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    //! @brief Builds the resulting plots.
    if (rank == 0) {
        validate_result(p);
        std::cout << plot::file("distributed_batch", p.build());
    }
    MPI_Finalize();
    return 0;
}

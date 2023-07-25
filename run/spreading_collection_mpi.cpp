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

auto init_lister(option::plot_t& p, int max_seed) {
    return batch::make_tagged_tuple_sequence(
        batch::arithmetic<option::seed >(0, max_seed-1, 1), // max_seed different random seeds
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
}

#ifndef FCPP_MPI
#define MPI_Init_thread(...)
#define MPI_Comm_rank(...)
#define MPI_Comm_size(...)
#define MPI_Barrier(...)
#define MPI_Finalize()
#endif

int main(int argc, char** argv) {
    // Sets up MPI.
    int provided, rank, n_procs;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    //assert(provided == MPI_THREAD_MULTIPLE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    // The component type (batch simulator with given options).
    using comp_t = component::batch_simulator<option::list>;
    // Compute a reference plot.
    option::plot_t q;
    if (rank == 0) {
        q = {};
        auto init_list = init_lister(q, 100);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    }
    // The vector storing recorded execution times.
    std::vector<double> t_static, t_dynamic;
    // Construct the plotter object.
    option::plot_t p;
    for (int i=0; i<10; ++i) {
        p = {};
        MPI_Barrier(MPI_COMM_WORLD);
        profiler t;
        // The list of initialisation values to be used for simulations.
        auto init_list = init_lister(p, 100);
        // Runs the given simulations.
        batch::mpi_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        if (rank == 0) {
            t_static.push_back(t);
            std::stringstream sp, sq;
            sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
            sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
            if (sp.str() != sq.str()) {
                std::cerr << "MPI static run " << i << ", check failed!" << std::endl;
                std::cout << sp.str();
                std::cout << sq.str();
            }
        }
    }
    for (int i=0; i<10; ++i) {
        p = {};
        MPI_Barrier(MPI_COMM_WORLD);
        profiler t;
        // The list of initialisation values to be used for simulations.
        auto init_list = init_lister(p, 100);
        // Runs the given simulations.
        batch::mpi_dynamic_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        if (rank == 0) {
            t_dynamic.push_back(t);
            std::stringstream sp, sq;
            sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
            sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
            if (sp.str() != sq.str()) {
                std::cerr << "MPI dynamic run " << i << ", check failed!" << std::endl;
                std::cout << sp.str();
                std::cout << sq.str();
            }
        }
    }
    if (rank == 0) {
        q = {};
        auto init_list = init_lister(q, 10*n_procs);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
    }
    for (int i=0; i<10; ++i) {
        p = {};
        MPI_Barrier(MPI_COMM_WORLD);
        profiler t;
        // The list of initialisation values to be used for simulations.
        auto init_list = init_lister(p, 10*n_procs);
        // Runs the given simulations.
        batch::mpi_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        if (rank == 0) {
            t_static.push_back(t);
            std::stringstream sp, sq;
            sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
            sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
            if (sp.str() != sq.str()) {
                std::cerr << "MPI static run " << i << ", check failed!" << std::endl;
                std::cout << sp.str();
                std::cout << sq.str();
            }
        }
    }
    for (int i=0; i<10; ++i) {
        p = {};
        MPI_Barrier(MPI_COMM_WORLD);
        profiler t;
        // The list of initialisation values to be used for simulations.
        auto init_list = init_lister(p, 10*n_procs);
        // Runs the given simulations.
        batch::mpi_dynamic_run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        if (rank == 0) {
            t_dynamic.push_back(t);
            std::stringstream sp, sq;
            sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
            sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
            if (sp.str() != sq.str()) {
                std::cerr << "MPI dynamic run " << i << ", check failed!" << std::endl;
                std::cout << sp.str();
                std::cout << sq.str();
            }
        }
    }
    MPI_Finalize();
    return 0;
}

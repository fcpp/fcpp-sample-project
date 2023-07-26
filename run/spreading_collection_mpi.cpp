// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include <chrono>
#include <iomanip>
#include <sstream>

#include "lib/spreading_collection.hpp"

using namespace fcpp;

//! @brief Object accumulating the CPU elapsed during its lifetime in a given counter.
class profiler {
  public:
    //! @brief Constructor given a counter name.
    profiler() = default;

    //! @brief Computes elapsed time.
    operator double() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001;
    }

  private:
    //! @brief Stores the clock during construction.
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
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

void plot_check(std::string name, int i, option::plot_t& p, option::plot_t& q) {
    std::stringstream sp, sq;
    sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
    sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
    if (sp.str() != sq.str()) {
        std::cerr << "MPI " << name << " run " << i << ", check failed!" << std::endl;
        std::cout << sp.str();
        std::cout << sq.str();
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <procs per node>" << std::endl;
        return 0;
    }
    // Sets up MPI.
    constexpr int rank_master = 0;
    int rank, n_procs, n_nodes, procs_per_node = atoi(argv[1]);
    size_t threads_per_proc;
    batch::mpi_init(rank, n_procs);
    n_nodes = n_procs / procs_per_node;
    threads_per_proc = std::thread::hardware_concurrency() / procs_per_node;
    // The component type (batch simulator with given options).
    using comp_t = component::batch_simulator<option::list>;

    // STRONG SCALING

    // Compute a reference plot, to check correctness.
    option::plot_t q;
    if (rank == rank_master) {
        q = {};
        profiler t;
        auto init_list = init_lister(q, 100);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        std::cerr << "Strong scaling reference plot computed in " << double(t) << "s" << std::endl;
    }
    // The vectors storing recorded execution times.
    std::vector<double> t_static, t_dynamic;
    // Construct the plotter object.
    option::plot_t p;
    // MPI static division.
    for (int i=0; i<10; ++i) {
        p = {};
        batch::mpi_barrier();
        profiler t;
        auto init_list = init_lister(p, 100);
        batch::mpi_run(comp_t{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
        if (rank == rank_master) {
            t_static.push_back(t);
            plot_check("static", i, p, q);
        }
    }
    // MPI dynamic division.
    for (int i=0; i<10; ++i) {
        p = {};
        batch::mpi_barrier();
        profiler t;
        auto init_list = init_lister(p, 100);
        batch::mpi_dynamic_run(comp_t{}, 4*threads_per_proc, 4, common::tags::dynamic_execution{threads_per_proc}, init_list);
        if (rank == 0) {
            t_dynamic.push_back(t);
            plot_check("dynamic", i, p, q);
        }
    }
    if (rank == rank_master) {
        // Report times and reset.
        std::cerr << "STRONG SCALING:" << std::endl;
        for (double x : t_static) std::cerr << " " << x;
        for (double x : t_dynamic) std::cerr << " " << x;
        std::cerr << std::endl;
        t_static = {};
        t_dynamic = {};
        q = {};

        // WEAK SCALING

        // Compute a reference plot, to check correctness.
        profiler t;
        auto init_list = init_lister(q, 10*n_nodes);
        batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
        std::cerr << "Weak scaling reference plot computed in " << double(t) << "s" << std::endl;
    }
    // MPI static division.
    for (int i=0; i<10; ++i) {
        p = {};
        batch::mpi_barrier();
        profiler t;
        auto init_list = init_lister(p, 10*n_nodes);
        batch::mpi_run(comp_t{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
        if (rank == rank_master) {
            t_static.push_back(t);
            plot_check("static", i, p, q);
        }
    }
    // MPI dynamic division.
    for (int i=0; i<10; ++i) {
        p = {};
        batch::mpi_barrier();
        profiler t;
        auto init_list = init_lister(p, 10*n_nodes);
        batch::mpi_dynamic_run(comp_t{}, 4*threads_per_proc, 4, common::tags::dynamic_execution{threads_per_proc}, init_list);
        if (rank == rank_master) {
            t_dynamic.push_back(t);
            plot_check("dynamic", i, p, q);
        }
    }
    batch::mpi_finalize();
    return 0;
}

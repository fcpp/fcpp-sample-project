// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_mpi.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots, across multiple nodes with MPI.
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

//! @brief Does not return an arithmetic sequence of seeds.
inline auto maybe_seeds(int max_seed, common::number_sequence<false>) {
    return batch::constant<>();
}

//! @brief Returns an arithmetic sequence of seeds.
inline auto maybe_seeds(int max_seed, common::number_sequence<true>) {
    return batch::arithmetic<option::seed>(0, max_seed-1, 1);
}

//! @brief Creates an init sequence given a plotter object, the number of seeds and whether to vary them first.
template <bool seeds_first>
auto init_lister(option::plot_t& p, int max_seed) {
    return batch::make_tagged_tuple_sequence(
        maybe_seeds(max_seed, common::number_sequence<seeds_first>{}),     // max_seed different random seeds
        batch::arithmetic<option::speed>(0, 48, 2, 10), // 25 different speeds
        batch::arithmetic<option::dens >(5, 29, 1, 10), // 25 different densities
        batch::arithmetic<option::hops >(1, 25, 1, 10), // 25 different hop sizes
        batch::arithmetic<option::tvar >(0, 48, 2, 10), // 25 different time variances
        maybe_seeds(max_seed, common::number_sequence<not seeds_first>{}), // max_seed different random seeds
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
        batch::constant<option::plotter,option::output>(&p,nullptr) // reference to the plotter object
    );
}

//! @brief Checks whether two computed plots are practically identical, printing corresponding output.
void plot_check(option::plot_t& p, option::plot_t& q) {
    std::stringstream sp, sq;
    sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
    sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
    if (sp.str() != sq.str()) {
        std::cerr << "Plot check failed!" << std::endl;
        std::cerr << "=======================================" << std::endl;
        std::cerr << sp.str();
        std::cerr << "=======================================" << std::endl;
        std::cerr << sq.str();
        std::cerr << "=======================================" << std::endl;
    }
}

//! @brief The component type (batch simulator with given options).
using comp_type = component::batch_simulator<option::list>;

//! @brief The number of runs to average times.
constexpr int runs = 10;

//! @brief The rank of the master process.
constexpr int rank_master = 0;

//! @brief Runs a series of executions, storing times and checking correctness.
template <bool seeds_first, typename F, typename... As>
void runner(int rank, int max_seed, option::plot_t& q, std::string s, F&& f) {
    if (rank == rank_master) std::cerr << "MPI " << s << ", starting " << runs << "runs." << std::endl;
    std::vector<double> v;
    for (int i=0; i<runs; ++i) {
        batch::mpi_barrier();
        profiler t;
        option::plot_t p;
        auto init_list = init_lister<seeds_first>(p, max_seed);
        f(init_list);
        if (rank == rank_master) {
            v.push_back(t);
            std::cerr << "MPI " << s << " run " << i << " completed in " << double(t) << "s." << std::endl;
            plot_check(p, q);
        }
    }
    if (rank == rank_master) {
        std::cout << std::endl << s << ": ";
        for (double x : v) std::cout << " " << x;
        std::cout << std::endl << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <procs per node>" << std::endl;
        return 0;
    }
    // Sets up MPI.
    int rank, n_procs, n_nodes, procs_per_node = atoi(argv[1]);
    batch::mpi_init(rank, n_procs);
    n_nodes = n_procs / procs_per_node;
    size_t threads_per_proc = std::thread::hardware_concurrency() / procs_per_node;
    if (rank == rank_master)
        std::cerr << "Running on " << n_nodes << " nodes, with " << procs_per_node << " MPI processes each, and " << threads_per_proc << " threads for each process." << std::endl;

    std::vector<std::string> scaling_name = {"STRONG", "WEAK"};
    std::vector<int> scaling_seeds = {100, 10*n_nodes};

    for (int s = 0; s < 2; ++s) {
        // Compute a reference plot, to check correctness.
        option::plot_t q;
        if (rank == rank_master) {
            profiler t;
            auto init_list = init_lister<true>(q, scaling_seeds[s]);
            batch::run(comp_type{}, common::tags::dynamic_execution{}, init_list);
            std::cerr << scaling_name[s] << " scaling reference plot computed in " << double(t) << "s" << std::endl;
        }
        // Baselines with 1 CPU
        if (n_nodes == 1) {
            std::cerr << std::endl << scaling_name[s] << " SCALING" << std::endl << std::endl;
            runner<true >(rank, scaling_seeds[s], q, "baseline seeds-first", [=](auto init_list){
                batch::run(comp_type{}, common::tags::dynamic_execution{threads_per_proc,1}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, "baseline seeds-last", [=](auto init_list){
                batch::run(comp_type{}, common::tags::dynamic_execution{threads_per_proc,1}, init_list);
            });
        } else {
            // Construct the plotter object.
            option::plot_t p;
            // MPI static seeds-first division.
            runner<true >(rank, scaling_seeds[s], q, "static seeds-first", [=](auto init_list){
                batch::mpi_run(comp_type{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, "static seeds-last",  [=](auto init_list){
                batch::mpi_run(comp_type{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<true >(rank, scaling_seeds[s], q, "static seeds-shuffle", [=](auto init_list){
                batch::mpi_run(comp_type{}, common::tags::dynamic_execution{threads_per_proc}, init_list, true);
            });
            runner<true >(rank, scaling_seeds[s], q, "dynamic seeds-first", [=](auto init_list){
                batch::mpi_better_dynamic_run(comp_type{}, 8, 8, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, "dynamic seeds-last", [=](auto init_list){
                batch::mpi_better_dynamic_run(comp_type{}, 8, 8, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<true >(rank, scaling_seeds[s], q, "dynamic seeds-shuffle", [=](auto init_list){
                batch::mpi_better_dynamic_run(comp_type{}, 8, 8, common::tags::dynamic_execution{threads_per_proc}, init_list, true);
            });
        }
    }
    batch::mpi_finalize();
    return 0;
}

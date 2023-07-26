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

//! @brief Checks whether two computed plots are practically identical, printing corresponding output.
void plot_check(std::string name, int i, option::plot_t& p, option::plot_t& q) {
    std::cerr << "MPI " << name << " run " << i << " completed." << std::endl;
    std::stringstream sp, sq;
    sp << std::setprecision(3) << plot::file("distributed_batch", p.build());
    sq << std::setprecision(3) << plot::file("distributed_batch", q.build());
    if (sp.str() != sq.str()) {
        std::cerr << "Plot check failed!" << std::endl;
        std::cerr << "=======================================" << std::endl;
        std::cout << sp.str();
        std::cerr << "=======================================" << std::endl;
        std::cout << sq.str();
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
void runner(int rank, int max_seed, option::plot_t& q, std::vector<double>& v, std::string s, F&& f) {
    for (int i=0; i<runs; ++i) {
        batch::mpi_barrier();
        profiler t;
        option::plot_t p;
        auto init_list = init_lister<seeds_first>(p, max_seed);
        f(init_list);
        if (rank == rank_master) {
            v.push_back(t);
            plot_check(s, i, p, q);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <procs per node>" << std::endl;
        return 0;
    }
    // Sets up MPI.
    int rank, n_procs, n_nodes, procs_per_node = atoi(argv[1]);
    size_t threads_per_proc;
    batch::mpi_init(rank, n_procs);
    n_nodes = n_procs / procs_per_node;
    threads_per_proc = std::thread::hardware_concurrency() / procs_per_node;

    std::vector<std::string> scaling_name = {"STRONG", "WEAK"};
    std::vector<int> scaling_seeds = {100, 10*n_nodes};

    for (int s = 0; s < 2; ++s) {
        // Compute a reference plot, to check correctness.
        option::plot_t q;
        if (rank == rank_master) {
            profiler t;
            auto init_list = init_lister<true>(q, scaling_seeds[s]);
            batch::run(comp_type{}, common::tags::dynamic_execution{}, init_list);
            std::cerr << scaling_name[s] << "scaling reference plot computed in " << double(t) << "s" << std::endl;
        }
        // Baselines with 1 CPU
        if (n_nodes == 1) {
            std::vector<double> t_sfirst[5], t_slast[5];
            runner<true >(rank, scaling_seeds[s], q, t_sfirst[0], "static seeds-first", [=](auto init_list){
                batch::run(comp_type{}, common::tags::parallel_execution{threads_per_proc}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, t_slast[0],  "static seeds-last",  [=](auto init_list){
                batch::run(comp_type{}, common::tags::parallel_execution{threads_per_proc}, init_list);
            });
            for (size_t i = 1; i < 5; ++i) {
                runner<true >(rank, scaling_seeds[s], q, t_sfirst[i], "dynamic-"" seeds-first", [=](auto init_list){
                    batch::run(comp_type{}, common::tags::dynamic_execution{threads_per_proc,i}, init_list);
                });
                runner<false>(rank, scaling_seeds[s], q, t_slast[i],  "dynamic-"" seeds-last", [=](auto init_list){
                    batch::run(comp_type{}, common::tags::dynamic_execution{threads_per_proc,i}, init_list);
                });
            }
            std::cerr << std::endl << scaling_name[s] << " SCALING" << std::endl;
            for (int i = 0; i < 5; ++i) {
                std::cerr << std::endl << (i == 0 ? "static" : "dynamic-" + std::to_string(i)) + " seeds-first:";
                for (double x : t_sfirst[i]) std::cerr << " " << x;
                std::cerr << std::endl << (i == 0 ? "static" : "dynamic-" + std::to_string(i)) + " seeds-last:";
                for (double x : t_slast[i]) std::cerr << " " << x;
            }
        } else {
            // The vectors storing recorded execution times.
            std::vector<double> t_static_sfirst, t_static_slast, t_dynamic_sfirst, t_dynamic_slast;
            // Construct the plotter object.
            option::plot_t p;
            // MPI static seeds-first division.
            runner<true >(rank, scaling_seeds[s], q, t_static_sfirst,  "static seeds-first", [=](auto init_list){
                batch::mpi_run(comp_type{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, t_static_slast,   "static seeds-last",  [=](auto init_list){
                batch::mpi_run(comp_type{}, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<true >(rank, scaling_seeds[s], q, t_dynamic_sfirst, "dynamic seeds-first", [=](auto init_list){
                batch::mpi_dynamic_run(comp_type{}, 4*threads_per_proc, 4, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            runner<false>(rank, scaling_seeds[s], q, t_dynamic_slast,  "dynamic seeds-last", [=](auto init_list){
                batch::mpi_dynamic_run(comp_type{}, 4*threads_per_proc, 4, common::tags::dynamic_execution{threads_per_proc}, init_list);
            });
            if (rank == rank_master) {
                // Report times and reset.
                std::cerr << std::endl << scaling_name[s] << " SCALING" << std::endl;
                std::cerr << std::endl << "static seeds-first:";
                for (double x : t_static_sfirst) std::cerr << " " << x;
                std::cerr << std::endl << "static seeds-last:";
                for (double x : t_static_slast) std::cerr << " " << x;
                std::cerr << std::endl << "dynamic seeds-first:";
                for (double x : t_dynamic_sfirst) std::cerr << " " << x;
                std::cerr << std::endl << "dynamic seeds-last:";
                for (double x : t_dynamic_slast) std::cerr << " " << x;
                std::cerr << std::endl;
            }
        }
    }
    batch::mpi_finalize();
    return 0;
}

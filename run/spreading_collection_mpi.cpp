// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include <limits>

#include "lib/spreading_collection.hpp"
#include <mpi.h>

using namespace fcpp;

void run_simulations(option::plot_t& p) {
    //! @brief The component type (batch simulator with given options).
    using comp_t = component::batch_simulator<option::list>;
    //! @brief The list of initialisation values to be used for simulations.
    auto init_list = batch::make_tagged_tuple_sequence(
        batch::arithmetic<option::seed>(0, 9, 1),                   // 10 different random seeds
        batch::arithmetic<option::speed>(size_t(0), comm/2, comm/20), // 11 different speeds
        // generate output file name for the run
        batch::stringify<option::output>("output/spreading_collection_batch", "txt"),
        batch::constant<option::plotter>(&p)                        // reference to the plotter object
    );
    //! @brief Runs the given simulations.
    batch::run(comp_t{}, init_list);
    //! @brief Builds the resulting plots.
}

void validate_result(option::plot_t& p) {
    int n_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    option::plot_t q;
    for (int i = 0; i < n_procs; ++i) {
        option::plot_t t;
        //! @brief The component type (batch simulator with given options).
		run_simulations(t);
        q+=t;
    }
    if (p == q)
        std::cerr << "Test succeed!" << std::endl;
    else
        std::cerr << "Test failed!" << std::endl; 
}

//! @brief Master process that aggregates all the plots
void master(option::plot_t& p) {
	int n_procs, size;
	int max_size = 1024 * 1024 * 1024;
	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
	char* buf = new char[max_size];
	MPI_Status status;
	for (int i = 1; i < n_procs; ++i) {
		MPI_Recv(buf, max_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &size);
		option::plot_t q;
		common::isstream is({buf, buf+size});
		is >> q;
		p += q;
	}
	delete [] buf;
	std::cout << plot::file("distributed_batch", p.build());
}

//! @brief Worker process that sends its plot to the master
void worker(const option::plot_t& p, int rank_master) {
	common::osstream os;
	os << p;
	std::vector<char>& v = os.data();
	char* data = v.data();
	std::size_t size = v.size(); //TODO: size_t -> int
	assert(size > std::numeric_limits<int>::max());
	MPI_Send(data, size, MPI_CHAR, rank_master, 0, MPI_COMM_WORLD);
}

/**
 * @brief Aggregates all plots using a centralized communication.
 * This is for demonstration only. It's not very efficient
 * because all the plots are sent to a single process.
 * The complexity is O(n).
 * Since the aggregation of plots is commutative and 
 * associative, a more efficient solution is to implement a
 * collective reduce communication with complexity O(log n)
 */

void aggregate_plots(option::plot_t& p, bool validate) {
	int rank, rank_master = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == rank_master) {
		master(p);
        if (validate)
            validate_result(p);
    }
	else
		worker(p, rank_master);
}

bool validation_requested(int argc, char** argv) {
    return argc == 2 && (strcmp(argv[1], "validate") == 0);
}


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

    bool validate = validation_requested(argc, argv);
    //! @brief Construct the plotter object.
    option::plot_t p;
	run_simulations(p);
    //std::cout << plot::file("batch", p.build());
	//! @brief Aggregate all plots
	aggregate_plots(p, validate);

	MPI_Finalize();
    return 0;
}


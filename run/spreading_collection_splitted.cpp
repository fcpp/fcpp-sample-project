// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include "lib/spreading_collection.hpp"

using namespace fcpp;

void run_splitted_simulation(option::plot_t& q, int n) {
    for (int i = 0; i < n; ++i) {
        //! @brief Construct the plotter object.
        option::plot_t p;
        //! @brief The component type (batch simulator with given options).
        using comp_t = component::batch_simulator<option::list>;
        //! @brief The list of initialisation values to be used for simulations.
        auto init_list = batch::make_tagged_tuple_sequence(
            batch::arithmetic<option::seed>(0, 9, 1),                   // 10 different random seeds
            batch::arithmetic<option::speed>(size_t(0), comm/2, comm/20), // 11 different speeds
            // generate output file name for the run
            batch::stringify<option::output>("output/spreading_collection_splitted", "txt"),
            batch::constant<option::plotter>(&p)                        // reference to the plotter object
        );
        //! @brief Runs the given simulations.
        batch::splitted_run(comp_t{}, common::tags::dynamic_execution{}, n, i, init_list);
        q += p;
    }
}

void run_complete_simulation(option::plot_t& q) {
    //! @brief Construct the plotter object.
    option::plot_t p;
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
    batch::run(comp_t{}, common::tags::dynamic_execution{}, init_list);
}

int main() {
    option::plot_t splitted_plot;
    option::plot_t complete_plot;
    run_splitted_simulation(splitted_plot, 4);
    run_complete_simulation(complete_plot);
    if (splitted_plot == complete_plot)
        std::cerr << "Test succeed!" << std::endl;
    else
        std::cerr << "Test failed!" << std::endl;
    //! @brief Builds the resulting plots.
    std::cout << plot::file("splitted", splitted_plot.build());
    std::cout << plot::file("completed", complete_plot.build());
    return 0;
}

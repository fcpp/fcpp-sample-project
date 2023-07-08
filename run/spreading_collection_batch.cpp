// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_batch.cpp
 * @brief Runs multiple executions of the spreading collection case study non-interactively from the command line, producing overall plots.
 */

#include "lib/spreading_collection.hpp"

using namespace fcpp;

int main() {
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
    batch::run(comp_t{}, init_list);
    //! @brief Builds the resulting plots.
    std::cout << plot::file("batch", p.build());
    return 0;
}

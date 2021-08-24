// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file spreading_collection_run.cpp
 * @brief Runs a single execution of the spreading collection case study non-interactively from the command line.
 */

#include "lib/spreading_collection.hpp"

using namespace fcpp;

int main() {
    //! @brief The network object type (batch simulator with given options).
    using net_t = component::batch_simulator<option::list>::net;
    //! @brief The initialisation values (node movement speed).
    auto init_v = common::make_tagged_tuple<option::speed>(comm/4);
    //! @brief Construct the network object.
    net_t network{init_v};
    //! @brief Run the simulation until exit.
    network.run();
    return 0;
}

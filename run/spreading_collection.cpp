// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

#include "lib/spreading_collection.hpp"

using namespace fcpp;

int main() {
    component::interactive_simulator<opt>::net network{common::make_tagged_tuple<name,epsilon,texture,speed>("Spreading-Collection Composition", 0.1, "fcpp.png", comm/3)};
    network.run();
    return 0;
}

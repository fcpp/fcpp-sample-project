// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

/**
 * @file simulation_setup.hpp
 * @brief Setup of the basic simulation setup details.
 */

#ifndef FCPP_SIMULATION_SETUP_H_
#define FCPP_SIMULATION_SETUP_H_

#include <cstddef>


//! @brief Minimum number whose square is at least n.
constexpr size_t discrete_sqrt(size_t n) {
    size_t lo = 0, hi = n, mid = 0;
    while (lo < hi) {
        mid = (lo + hi)/2;
        if (mid*mid < n) lo = mid+1;
        else hi = mid;
    }
    return lo;
}

//! @brief Number of devices.
constexpr size_t devices = 1000;

//! @brief Communication radius.
constexpr size_t comm = 100;

//! @brief Side of the deployment area.
constexpr size_t side = discrete_sqrt(devices * 3000);

//! @brief Height of the deployment area.
constexpr size_t height = 100;

//! @brief Color hue scale.
constexpr float hue_scale = 360.0f/(side+height);


#endif // FCPP_SIMULATION_SETUP_H_

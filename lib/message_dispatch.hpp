// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

/**
 * @file message_dispatch.hpp
 * @brief Aggregate process dispatching point-to-point messages, avoiding to flood the network.
 */

#ifndef FCPP_MESSAGE_DISPATCH_H_
#define FCPP_MESSAGE_DISPATCH_H_

#include "lib/beautify.hpp"
#include "lib/coordination.hpp"
#include "lib/data.hpp"


//! @brief Struct representing a message.
struct message {
    //! @brief Sender UID.
    fcpp::device_t from;
    //! @brief Receiver UID.
    fcpp::device_t to;
    //! @brief Creation timestamp.
    fcpp::times_t time;

    //! @brief Empty constructor.
    message() = default;

    //! @brief Member constructor.
    message(fcpp::device_t from, fcpp::device_t to, fcpp::times_t time) : from(from), to(to), time(time) {}

    //! @brief Equality operator.
    bool operator==(message const& m) const {
        return from == m.from and to == m.to and time == m.time;
    }

    //! @brief Hash computation.
    size_t hash() const {
        constexpr size_t offs = sizeof(size_t)*CHAR_BIT/3;
        return (size_t(time) << (2*offs)) | (size_t(from) << (offs)) | size_t(to);
    }

    //! @brief Serialises the content from/to a given input/output stream.
    template <typename S>
    S& serialize(S& s) {
        return s & from & to & time;
    }

    //! @brief Serialises the content from/to a given input/output stream (const overload).
    template <typename S>
    S& serialize(S& s) const {
        return s << from << to << time;
    }
};


namespace std {
    //! @brief Hasher object for the message struct.
    template <>
    struct hash<message> {
        //! @brief Produces an hash for a message, combining to and from into a size_t.
        size_t operator()(message const& m) const {
            return m.hash();
        }
    };
}


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


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


//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {


namespace tags {
    //! @brief The movement speed of devices.
    struct speed {};

    //! @brief The maximum message size ever exchanged by the node.
    struct max_msg {};

    //! @brief The total message size ever exchanged by the node.
    struct tot_msg {};

    //! @brief The maximum number of processes ever run by the node.
    struct max_proc {};

    //! @brief The total number of processes ever run by the node.
    struct tot_proc {};

    //! @brief Total time of first delivery.
    struct first_delivery {};

    //! @brief Total number of sent messages.
    struct sent_count {};

    //! @brief Total number of first deliveries.
    struct delivery_count {};

    //! @brief Total number of repeated deliveries.
    struct repeat_count {};

    //! @brief Distance to the central node.
    struct center_dist {};

    //! @brief Color of the current node.
    struct node_color {};

    //! @brief Left color of the current node.
    struct left_color {};

    //! @brief Right color of the current node.
    struct right_color {};

    //! @brief Size of the current node.
    struct node_size {};

    //! @brief Shape of the current node.
    struct node_shape {};
}


using set_t = std::unordered_set<device_t>;
using map_t = std::unordered_map<message, times_t>;

//! @brief Main function.
MAIN() {
    // import tags for convenience
    using namespace tags;
    // random walk
    rectangle_walk(CALL, make_vec(0,0,0), make_vec(side,side,height), node.storage(speed{}), 1);
    device_t src_id = 0;
    // distance estimation
    bool is_src = node.uid == src_id;
    double ds = bis_distance(CALL, is_src, 1, 100);
    // basic node rendering
    node.storage(center_dist{}) = ds;
    node.storage(node_color{}) = color::hsva(ds*hue_scale, 1, 1);
    node.storage(node_shape{}) = is_src ? shape::cube : shape::icosahedron;
    node.storage(node_size{}) = is_src ? 20 : 10;
    // spanning tree definition
    device_t parent = get<1>(min_hood(CALL, make_tuple(nbr(CALL, ds), node.nbr_uid())));
    // routing sets along the tree
    set_t below = sp_collection(CALL, ds, set_t{node.uid}, set_t{}, [](set_t x, set_t const& y){
        x.insert(y.begin(), y.end());
        return x;
    });
    // random message with 1% probability
    common::option<message> m;
    if (node.next_real() < 0.01) {
        m.emplace(node.uid, (device_t)node.next_int(devices-1), node.current_time());
        node.storage(sent_count{}) += 1;
    }
    // deploys messages
    std::vector<color> procs{BLACK};
    map_t r = spawn(CALL, [&](message const& m){
        procs.push_back(color::hsva(m.to*360.0/devices, 1, 1));
        bool inpath = below.count(m.from) + below.count(m.to) > 0;
        status s = node.uid == m.to ? status::terminated_output :
                   inpath ? status::internal : status::external;
        return make_tuple(node.current_time(), s);
    }, m);
    // process and msg stats
    node.storage(max_proc{}) = max(node.storage(max_proc{}), procs.size() - 1);
    node.storage(tot_proc{}) += procs.size() - 1;
    node.storage(max_msg{}) = max(node.storage(max_msg{}), node.msg_size());
    node.storage(tot_msg{}) += node.msg_size();
    // additional node rendering
    node.storage(left_color{})  = procs[min(int(procs.size()), 2)-1];
    node.storage(right_color{}) = procs[min(int(procs.size()), 3)-1];
    // persist received messages and delivery stats
    r = old(CALL, map_t{}, [&](map_t m){
        for (auto const& x : r) {
            if (m.count(x.first)) node.storage(repeat_count{}) += 1;
            else {
                node.storage(first_delivery{}) += x.second - x.first.time;
                node.storage(delivery_count{}) += 1;
                m[x.first] = x.second;
            }
        }
        return m;
    });
}
//! @brief Exports for the main function.
FUN_EXPORT main_t = export_list<rectangle_walk_t<3>, bis_distance_t, sp_collection_t<double, set_t>, device_t, spawn_t<message, status>, map_t>;


}


}

#endif // FCPP_MESSAGE_DISPATCH_H_

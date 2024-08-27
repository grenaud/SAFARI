#ifndef HANDLEGRAPH_ALGORITHMS_FIND_SHORTEST_PATHS_HPP_INCLUDED
#define HANDLEGRAPH_ALGORITHMS_FIND_SHORTEST_PATHS_HPP_INCLUDED

/**
 * \file find_shortest_paths.hpp
 *
 * Definitions for the find_shortest_paths algorithm.
 */

#include <unordered_map>

#include "handlegraph/handle_graph.hpp"

namespace handlegraph {
namespace algorithms {
    
/// Finds the length of the shortest oriented path from the given handle
/// in a given direction to all reachable oriented nodes on a directed walk.
/// Uses Dijkstra's Algorithm. Distances are measured between the outgoing
/// side of the start node and the incoming side of the target.
std::unordered_map<handle_t, size_t>  find_shortest_paths(const HandleGraph* g, handle_t start,
                                                          bool traverse_leftward = false);
                                                      
}
}

#endif

#ifndef HANDLEGRAPH_ALGORITHMS_WEAKLY_CONNECTED_COMPONENTS_HPP_INCLUDED
#define HANDLEGRAPH_ALGORITHMS_WEAKLY_CONNECTED_COMPONENTS_HPP_INCLUDED

/**
 * \file weakly_connected_components.hpp
 *
 * Defines an algorithm for finding weakly connected components in a graph.
 */

#include "handlegraph/handle_graph.hpp"

#include <unordered_set>
#include <vector>

namespace handlegraph {
namespace algorithms {


/// Returns sets of IDs defining components that are connected by any series
/// of nodes and edges, even if it is not a valid bidirected walk. TODO: It
/// might make sense to have a handle-returning version, but the consumers of
/// weakly connected components right now want IDs, and membership in a weakly
/// connected component is orientation-independent.
std::vector<std::unordered_set<nid_t>> weakly_connected_components(const HandleGraph* graph);

/// Return pairs of weakly connected component ID sets and the handles that are
/// their tips, oriented inward. If a node is both a head and a tail, it will
/// appear in tips in both orientations.
std::vector<std::pair<std::unordered_set<nid_t>, std::vector<handle_t>>> weakly_connected_components_with_tips(const HandleGraph* graph);

/// Returns true if graph is a single weakly connected component. Graphs with
/// no nodes are considered connected.
bool is_weakly_connected(const HandleGraph* graph);

}
}

#endif

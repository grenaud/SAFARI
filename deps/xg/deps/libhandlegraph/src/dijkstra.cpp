/**
 * \file dijkstra.cpp
 *
 * Implementation of Dijkstra's Algorithm over the bidirected graph.
 */

#include "handlegraph/algorithms/dijkstra.hpp"

#include <queue>

namespace handlegraph {
namespace algorithms {

using namespace std;

bool dijkstra(const HandleGraph* g, handle_t start,
              function<bool(const handle_t&, size_t)> reached_callback,
              bool traverse_leftward) {
              
    unordered_set<handle_t> starts;
    starts.insert(start);          
    
    // Implement single-start search in terms of multi-start search
    return dijkstra(g, starts, reached_callback, traverse_leftward);
              
}

bool dijkstra(const HandleGraph* g, const unordered_set<handle_t>& starts,
              function<bool(const handle_t&, size_t)> reached_callback,
              bool traverse_leftward) {

    // We keep a priority queue so we can visit the handle with the shortest
    // distance next. We put handles in here whenever we see them with shorter
    // distances (since STL priority queue can't update), so we also need to
    // make sure nodes coming out haven't been visited already.
    using Record = pair<size_t, handle_t>;
    
    // We filter out handles that have already been visited.
    unordered_set<handle_t> visited;
    
    // We need a custom ordering for the queue
    struct IsFirstGreater {
        IsFirstGreater() = default;
        ~IsFirstGreater() = default;
        inline bool operator()(const Record& a, const Record& b) {
            return a.first > b.first;
        }
    };
    
    priority_queue<Record, vector<Record>, IsFirstGreater> queue;
    unordered_set<handle_t> dequeued;
    
    // We keep a current handle
    handle_t current;
    size_t distance = 0;
    for (auto& start : starts) {
        queue.push(make_pair(distance, start));
    }
    
    while (!queue.empty()) {
        // While there are things in the queue, get the first.
        tie(distance, current) = queue.top();
        queue.pop();
        if (dequeued.count(current)) {
            continue;
        }
        else {
            dequeued.insert(current);
        }
        
#ifdef debug_vg_algorithms
        cerr << "Visit " << g->get_id(current) << " " << g->get_is_reverse(current) << " at distance " << distance << endl;
#endif    


        // Emit the handle as being at the given distance
        if (!reached_callback(current, distance)) {
            // The user told us to stop. Return that we stopped early.
            
#ifdef debug_vg_algorithms
            cerr << "\tAbort search" << endl;
#endif  
            
            return false;
        }
        
        // Remember that we made it here.
        visited.emplace(current);
        
        if (!starts.count(current)) {
            // Up the distance with the node's length. We don't do this for the
            // start handles because we want to count distance from the *end* of
            // the start handles unless directed otherwise.
            distance += g->get_length(current);
        }
            
        g->follow_edges(current, traverse_leftward, [&](const handle_t& next) {
            // For each handle to the right of here
            
            if (!visited.count(next)) {
                // New shortest distance. Will never happen after the handle comes out of the queue because of Dijkstra.
                queue.push(make_pair(distance, next));
                
#ifdef debug_vg_algorithms
                cerr << "\tNew best path to " << g->get_id(next) << " " << g->get_is_reverse(next)
                    << " at distance " << distance << endl;
#endif
                
            } else {
#ifdef debug_vg_algorithms
                cerr << "\tDisregard path to " << g->get_id(next) << " " << g->get_is_reverse(next)
                    << " at distance " << distance << endl;
#endif
            }
        });
    }

    // If we made it here, we finished the entire graph.
    return true;

}

    
}
}

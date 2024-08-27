// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


//  union_find.hpp
//
// Contains an implementation of a custom union-find with some additional capabilities
//

#ifndef structures_union_find_hpp
#define structures_union_find_hpp

#include <vector>
#include <unordered_set>
#include <cstdint>
#include <algorithm>
#include <cassert>

namespace structures {

using namespace std;

/**
 * A custom Union-Find data structure that supports merging a set of indices in
 * disjoint sets in amortized nearly linear time. This implementation also supports
 * querying the size of the group containing an index in constant time and querying
 * the members of the group containing an index in linear time in the size of the group.
 */
class UnionFind {
public:
    /// Construct UnionFind for this many indices
    //group() cannot be used if include_children is false
    UnionFind(size_t size, bool include_children = true);
    
    /// Destructor
    ~UnionFind();
    
    /// Returns the number of indices in the UnionFind
    size_t size();

    /// Resize by adding additional items, each in its own set. Does nothing if size is less than the current size
    void resize(size_t size);
    
    /// Returns the group ID that index i belongs to (can change after calling union)
    size_t find_group(size_t i);
    
    /// Merges the group containing index i with the group containing index j
    size_t union_groups(size_t i, size_t j);
    
    /// Returns the size of the group containing index i
    size_t group_size(size_t i);
    
    /// Returns a vector of the indices in the same group as index i
    vector<size_t> group(size_t i);
    
    /// Returns all of the groups, each in a separate vector
    vector<vector<size_t>> all_groups();
    
private:
    
    struct UFNode;
    vector<UFNode> uf_nodes;
    bool include_children;
};

    

struct UnionFind::UFNode {
    UFNode(size_t index) : rank(0), size(1), head(index) {}
    ~UFNode() {}
    
    size_t rank;
    size_t size;
    size_t head;
    unordered_set<size_t> children;
};
    
}

#endif /* structures_union_find_hpp */

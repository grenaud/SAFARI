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


#include "structures/suffix_tree.hpp"

namespace structures {

using namespace std;

// Ukkonen's construction algorithm, similar to implementation in
// http://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english
SuffixTree::SuffixTree(string::const_iterator begin, string::const_iterator end) :
begin(begin), end(end)
{
    
    unordered_map<unordered_map<char, STNode*>*, unordered_map<char, STNode*>*> suffix_links;
    
    // active "edge" in usual parlance
    STNode* active_node = nullptr;
    // active "node" in usual parlance
    unordered_map<char, STNode*>* active_branch_point = &root;
    
    // the next index to attempt to match on the current node (and possibly insert a new one)
    int active_length = 0;
    
    int remaining = 0;
    int64_t str_len = end - begin;
    for (int64_t i = 0; i <= str_len; i++) {
        
        remaining++;
        
        // is there an implicit match?
        if (active_node) {
            // the active position is along the sequence of the node
            if (get_char(active_node->first + active_length) == get_char(i)) {
                // it matches this position of the string
                if (active_length == active_node->final_index(i)) {
                    // advance the active position to the next branch point
                    active_branch_point = &(active_node->children);
                    active_node = nullptr;
                    active_length = 0;
                }
                else {
                    // advance the active position down the node
                    active_length++;
                }
                
                continue;
            }
        }
        else if (active_branch_point->count(get_char(i))) {
            // the active position is at a branch point and it has an edge that
            // matches
            active_node = active_branch_point->at(get_char(i));
            if (active_node->length(i) == 1) {
                active_branch_point = &(active_node->children);
                active_node = nullptr;
            }
            else {
                active_length = 1;
            }
            
            continue;
        }
        
        
        // there are no more implicit matches to work through, add the
        // remaining suffixes we've accumulated
        
        unordered_map<char, STNode*>* prev_internal_branch_point = nullptr;
        while (remaining > 0) {
            
            // traverse downward if necessary
            if (active_node) {
                int64_t node_len = active_node->length(i);
                while (active_length >= node_len) {
                    
                    active_branch_point = &(active_node->children);
                    active_length -= node_len;
                    
                    // we land right on a branch point, which should not have a
                    // corresponding edge since we already checked
                    if (active_length == 0) {
                        active_node = nullptr;
                        break;
                    }
                    
                    active_node = active_branch_point->at(get_char(i - active_length));
                    node_len = active_node->length(i);
                }
            }
            
            if (!active_node) {
                if (active_branch_point->count(get_char(i))) {
                    // there is a node from the branch point that starts with the right
                    // sequence, identify it as the active node and then continue the iteration
                    // so that it gets identified as an implicit match
                    active_node = active_branch_point->at(get_char(i));
                }
                else {
                    // there is no node from this branch point starting with the right
                    // sequence so create one and add it
                    nodes.emplace_back(i, -1);
                    STNode* new_node = &nodes.back();
                    (*active_branch_point)[get_char(i)] = new_node;
                    
                    if (prev_internal_branch_point && active_branch_point != &root) {
                        suffix_links[prev_internal_branch_point] = active_branch_point;
                    }
                    
                    if (active_branch_point != &root) {
                        prev_internal_branch_point = active_branch_point;
                    }
                    else {
                        prev_internal_branch_point = nullptr;
                    }
                    
                    if (suffix_links.count(active_branch_point)) {
                        active_branch_point = suffix_links[active_branch_point];
                    }
                    else {
                        active_branch_point = &root;
                    }
                }
            }
            
            if (active_node) {
                
                // check to make sure we haven't traversed to somewhere with an implicit match
                if (get_char(active_node->first + active_length) == get_char(i)) {
                    // take care of any suffix links before advancing to next phase
                    if (prev_internal_branch_point && active_branch_point != &root) {
                        suffix_links[prev_internal_branch_point] = active_branch_point;
                    }
                    
                    // it matches this position of the string
                    if (active_length == active_node->final_index(i)) {
                        // advance the active position to the next branch point
                        active_branch_point = &(active_node->children);
                        active_node = nullptr;
                        active_length = 0;
                    }
                    else {
                        // advance the active position down the node
                        active_length++;
                    }
                    
                    prev_internal_branch_point = nullptr;
                    
                    break;
                }
                
                // create a new node for the first part of active node
                nodes.emplace_back(active_node->first, active_node->first + active_length - 1);
                STNode* new_beginning_node = &nodes.back();
                // rewire the edge from the parent branch point
                (*active_branch_point)[get_char(active_node->first)] = new_beginning_node;
                
                // create a new node for the new leaf
                nodes.emplace_back(i, -1);
                STNode* new_end_node = &nodes.back();
                // add an edge from the new node for the first segment
                new_beginning_node->children[get_char(new_end_node->first)] = new_end_node;
                
                // move the current node down to the end of the new
                active_node->first += active_length;
                // add in the next
                new_beginning_node->children[get_char(active_node->first)] = active_node;
                
                // move our pointer to the new beginning node
                active_node = new_beginning_node;
                
                // add in a suffix link
                if (prev_internal_branch_point) {
                    
                    suffix_links[prev_internal_branch_point] = &(active_node->children);
                    
                }
                
                // set up the next suffix link
                prev_internal_branch_point = &(active_node->children);
                
                if (active_branch_point == &root) {
                    // we are already at the root, so walk the beginning of the suffix ahead
                    // and update the active point accordingly
                    active_length--;
                    char active_node_begin;
                    if (active_node->length(i) > 1) {
                        active_node_begin = get_char(active_node->first + 1);
                    }
                    else {
                        active_node_begin = get_char(new_end_node->first);
                    }
                    if (active_branch_point->count(active_node_begin)) {
                        active_node = active_branch_point->at(active_node_begin);
                    }
                    else {
                        active_node = nullptr;
                    }
                }
                else if (suffix_links.count(active_branch_point)) {
                    // there is a suffix link, so we can walk to the next suffix using that
                    active_branch_point = suffix_links[active_branch_point];
                    if (active_branch_point->count(get_char(active_node->first))) {
                        active_node = active_branch_point->at(get_char(active_node->first));
                    }
                    else {
                        active_node = nullptr;
                    }
                }
                else {
                    // we are newly moving to the root, which is the same as taking a suffix
                    // link to it
                    active_branch_point = &root;
                    if (active_branch_point->count(get_char(active_node->first))) {
                        active_node = active_branch_point->at(get_char(active_node->first));
                    }
                    else {
                        active_node = nullptr;
                    }
                }
            }
            
            // decrement the number of leaves remaining
            remaining--;
        }
    }
    
    // initialize DFS stack
    list<STNode*> stack;
    for (const auto& edge : root) {
        stack.push_back(edge.second);
    }
    
    // navigate through tree and replace the implicit end index with an explicit one
    while (!stack.empty()) {
        STNode* node = stack.back();
        stack.pop_back();
        
        if (node->last < 0) {
            node->last = str_len;
        }
        
        for (const auto& edge : node->children) {
            stack.push_back(edge.second);
        }
    }
    
}

size_t SuffixTree::longest_overlap(const string& str) {
    return longest_overlap(str.begin(), str.end());
}

size_t SuffixTree::longest_overlap(string::const_iterator begin, string::const_iterator end) {
    
    size_t overlap = 0;
    
    STNode* node = nullptr;
    unordered_map<char, STNode*>* branch_point = &root;
    
    size_t node_idx = 0;
    size_t str_idx = 0;
    
    for (auto iter = begin; iter <= end; iter++, str_idx++) {
        if (branch_point) {
            // check if the prefix thus far is a suffix
            if (branch_point->count('\0')) {
                overlap = str_idx;
            }
            // check for match on the first position on a node using the edges
            if (branch_point->count(*iter)) {
                node = branch_point->at(*iter);
                branch_point = nullptr;
                node_idx = 1;
            }
            else {
                break;
            }
        }
        else {
            // check for match along a node sequence
            if (iter == end) {
                // we've run out of string to find matches for but this could be
                // an overlap if we're at the end of of the suffix tree string too
                if (get_char(node->first + node_idx) == '\0') {
                    overlap = str_idx;
                }
            }
            else {
                if (*iter == get_char(node->first + node_idx)) {
                    // we match here
                    node_idx++;
                }
                else if (get_char(node->first + node_idx) == '\0') {
                    // we've matched the entire string, this is a leaf
                    overlap = str_idx;
                    break;
                }
                else {
                    // we mismatch, don't keep looking for longer matches
                    break;
                }
            }
        }
        
        // mark if we need to use edges next iteration
        if (node_idx > node->last - node->first) {
            branch_point = &(node->children);
        }
    }
    
    return overlap;
}

vector<size_t> SuffixTree::substring_locations(const string& str) {
    return substring_locations(str.begin(), str.end());
}

vector<size_t> SuffixTree::substring_locations(string::const_iterator begin, string::const_iterator end) {
    
    vector<size_t> locations;
    
    
    size_t str_len = this->end - this->begin;
    size_t substr_len = end - begin;
    
    // ensure that we will never have an index beyond end of the suffix tree string
    // or try to match empty string (else it matches everywhere)
    if (substr_len > str_len || end <= begin) {
        return locations;
    }
    
    STNode* node = nullptr;
    unordered_map<char, STNode*>* branch_point = &root;
    size_t node_idx = 0;
    
    bool found_match = true;
    
    // look for a match of the entirety of the query string
    for (auto iter = begin; iter != end; iter++) {
        if (branch_point) {
            // check for match on the first position on a node using the edges
            if (branch_point->count(*iter)) {
                node = branch_point->at(*iter);
                branch_point = nullptr;
                node_idx = 1;
            }
            else {
                found_match = false;
                break;
            }
        }
        else {
            // check for match on the current position along a node
            if (*iter == get_char(node->first + node_idx)) {
                node_idx++;
            }
            else {
                found_match = false;
                break;
            }
        }
        
        // mark if we need to use edges next iteration
        if (node_idx > node->last - node->first) {
            branch_point = &(node->children);
        }
    }
    
    if (found_match) {
        // we matched the entire query string, now traverse down the tree to find locations
        
        
        // edge case: there is only one location, so we are already on a leaf node
        if (node->last == str_len) {
            locations.push_back(node->first + node_idx - substr_len);
            return locations;
        }
        
        // mark the distance to the past-the-last index of this node
        int64_t depth_to_node_end = node->last - node->first - node_idx;
        
        // initialize a stack with the next nodes down
        list<pair<STNode*, int64_t>> stack;
        for (const pair<char, STNode*>& edge : node->children) {
            stack.push_back(pair<STNode*, int64_t>(edge.second, depth_to_node_end));
        }
        
        // use DFS to find leaves, keeping track of depth
        while (!stack.empty()) {
            pair<STNode*, int64_t> path_head = stack.back();
            stack.pop_back();
            
            if (path_head.first->last == str_len) {
                // we are at a leaf, use it and the depth to figure out where the
                // match is at the lead of this suffix
                
                STNode* head_node = path_head.first;
                int64_t depth = path_head.second;
                
                locations.push_back(str_len - (head_node->last - head_node->first + 1 + depth + substr_len));
            }
            else {
                // we are at an internal node, update the depth and continue
                
                STNode* head_node = path_head.first;
                int64_t next_depth = path_head.second + head_node->last - head_node->first + 1;
                
                for (const pair<char, STNode*>& edge : path_head.first->children) {
                    stack.push_back(pair<STNode*, int64_t>(edge.second, next_depth));
                }
            }
        }
    }
    
    return locations;
}

SuffixTree::STNode::STNode(int64_t first, int64_t last) : first(first), last(last) {
    // nothing to do
}

}

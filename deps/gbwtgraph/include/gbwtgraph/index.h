#ifndef GBWTGRAPH_CONSTRUCTION_H
#define GBWTGRAPH_CONSTRUCTION_H

#include <cstdlib>
#include <functional>

#include <omp.h>

#include <gbwtgraph/gbwtgraph.h>
#include <gbwtgraph/minimizer.h>

#define RYMER

/*
  index.h: Minimizer index construction from GBWTGraph.
*/

namespace gbwtgraph
{


//------------------------------------------------------------------------------

/*
  Index the haplotypes in the graph. Insert the minimizers into the provided index.
  Function argument get_payload is used to generate the payload for each position
  stored in the index.
  The number of threads can be set through OMP.
*/
template<class KeyType>
void
index_haplotypes(const GBWTGraph& graph, bool rymer, MinimizerIndex<KeyType>& index,
                 const std::function<payload_type(const pos_t&)>& get_payload, unsigned int k=21)
{
  typedef typename MinimizerIndex<KeyType>::minimizer_type minimizer_type;

  int threads = omp_get_max_threads();

  // Minimizer caching. We only generate the payloads after we have removed duplicate positions.
  std::vector<std::vector<std::pair<minimizer_type, pos_t>>> cache(threads);
  constexpr size_t MINIMIZER_CACHE_SIZE = 1024;

      // Introduce a shared counter to track the number of indexed minimizers.
  std::atomic<size_t> minimizer_count(0);

  auto flush_cache = [&](int thread_id)
  {

    std::vector<std::pair<minimizer_type, pos_t>>& current_cache = cache[thread_id];
    gbwt::removeDuplicates(current_cache, false);
    std::vector<payload_type> payload;
    payload.reserve(current_cache.size());
    for(size_t i = 0; i < current_cache.size(); i++) { payload.push_back(get_payload(current_cache[i].second)); }
    #pragma omp critical (minimizer_index)
    {
      for(size_t i = 0; i < current_cache.size(); i++)
      {
        minimizer_count++;
        if(!rymer){
            index.insert(current_cache[i].first, current_cache[i].second, payload[i]);
                  }
        else{
            auto rymer_key = index.kmer2rymer(current_cache[i].first.key, k);
            current_cache[i].first.key=rymer_key;
            current_cache[i].first.hash=rymer_key.hash();
            index.insert(current_cache[i].first, current_cache[i].second, {current_cache[i].first.original_kmer_key.get_key(), 42});
            current_cache[i].first.original_kmer_key = current_cache[i].first.key;
            current_cache[i].first.original_kmer_hash = current_cache[i].first.key.hash();
                  }
            }
      }
    cache[thread_id].clear();
  };

  // Minimizer finding.
  auto find_minimizers = [&](const std::vector<handle_t>& traversal, const std::string& seq)
  {

    std::vector<minimizer_type> minimizers;

    if(!rymer){
    minimizers = index.minimizers(seq, false); // Calls syncmers() when appropriate.
              }

    else {
    minimizers = index.minimizers(seq, true);
         }

    auto iter = traversal.begin();
    size_t node_start = 0;
    int thread_id = omp_get_thread_num();

    for(minimizer_type& minimizer : minimizers)
    {
       if(minimizer.empty()) { continue; }

      // Find the node covering minimizer starting position.
      size_t node_length = graph.get_length(*iter);
      while(node_start + node_length <= minimizer.offset)
      {
        node_start += node_length;
        ++iter;
        node_length = graph.get_length(*iter);
      }
      pos_t pos { graph.get_id(*iter), graph.get_is_reverse(*iter), minimizer.offset - node_start };
      if(minimizer.is_reverse) { pos = reverse_base_pos(pos, node_length); }
      if(!Position::valid_offset(pos))
      {
        #pragma omp critical (cerr)
        {
          std::cerr << "index_haplotypes(): Node offset " << offset(pos) << " is too large" << std::endl;
        }
        std::exit(EXIT_FAILURE);
      }
      cache[thread_id].emplace_back(minimizer, pos);
    }

    if(cache[thread_id].size() >= MINIMIZER_CACHE_SIZE) { flush_cache(thread_id); }
  };

  /*
    Index the minimizers.
    We do a lot of redundant work by traversing both orientations and finding almost the same minimizers
    in each orientation. If we consider only the windows starting in forward (reverse) orientation,
    we may skip windows that cross from a reverse node to a forward node (from a forward node to a
    reverse node).
  */

  for_each_haplotype_window(graph, index.window_bp(), find_minimizers, (threads > 1), false);
  for(int thread_id = 0; thread_id < threads; thread_id++) { flush_cache(thread_id); }

  // Output the total minimizer count here
std::cerr << "Total count: " << minimizer_count << std::endl;

  if(rymer){
  //std::cerr << "DUMPING RYMER HASH" << std::endl;
  //index.dump_hash_table_rymer();
           }

  else{
  //std::cerr << "DUMPING MINIMIZER HASH" << std::endl;
  //index.dump_hash_table();
      }

}

#endif // GBWTGRAPH_CONSTRUCTION_H

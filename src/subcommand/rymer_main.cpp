/** \file rymer_main.cpp
 *
 * Defines the "vg rymer" subcommand, which builds the minimizer index.
 *
 * The index contains the lexicographically smallest kmer in a window of w
 * successive kmers and their reverse complements. If the kmer contains
 * characters other than A, C, G, and T, it will not be indexed.
 *
 * The index contains either all or haplotype-consistent minimizers. Indexing all
 * minimizers from complex graph regions can take a long time (e.g. tens of hours
 * vs 5-10 minutes for 1000GP), because many windows have the same minimizer.
 * As the total number of minimizers is manageable (e.g. 1.5x more for 1000GP)
 * it should be possible to develop a better algorithm for finding the minimizers.
 *
 * A quick idea for indexing the entire graph:
 * - For each node v, extract the subgraph for the windows starting in v.
 * - Extract all k'-mers from the subgraph and use them to determine where the
 *   minimizers can start.
 */

#include "subcommand.hpp"

#include <vg/io/vpkg.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

#include <getopt.h>
#include <omp.h>

#include "../gbwtgraph_helper.hpp"
#include "../gbwt_helper.hpp"
#include "../index_registry.hpp"
#include "../utility.hpp"
#include "../handle.hpp"
#include "../snarl_distance_index.hpp"

#include <gbwtgraph/index.h>

using namespace vg;

// Using too many threads just wastes CPU time without speeding up the construction.
constexpr int DEFAULT_MAX_THREADS = 16;

int get_default_threads_rymer() {
    return std::min(omp_get_max_threads(), DEFAULT_MAX_THREADS);
}

void help_rymer(char** argv) {
    std::cerr << "usage: " << argv[0] << " rymer [options] graph" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Builds a (w, k)-RYmer index of the threads in the GBWT" << std::endl;
    std::cerr << "index. The graph can be any HandleGraph, which will be transformed into a GBWTGraph." << std::endl;
    std::cerr << "The transformation can be avoided by providing a GBWTGraph or a GBZ graph." << std::endl;
    std::cerr << std::endl;
    std::cerr << "Required options:" << std::endl;
    std::cerr << "    -o, --output-name X     store the index to file X" << std::endl;
    std::cerr << std::endl;
    std::cerr << "RYmer options:" << std::endl;
    std::cerr << "    -k, --kmer-length N     length of the kmers in the index (default " << IndexingParameters::rymer_k << ", max " << gbwtgraph::DefaultMinimizerIndex::key_type::KMER_MAX_LENGTH << ")" << std::endl;
    std::cerr << "    -w, --window-length N   choose the RYmer from a window of N kmers (default " << IndexingParameters::rymer_w << ")" << std::endl;
    std::cerr << "    -s, --smer-length N     use smers of length N in closed syncmers (default " << IndexingParameters::minimizer_s << ")" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Other options:" << std::endl;
    std::cerr << "    -d, --distance-index X  annotate the hits with positions in this distance index" << std::endl;
    std::cerr << "    -l, --load-index X      load the index from file X and insert the new RYmers into it" << std::endl;
    std::cerr << "                            (overrides RYmer options)" << std::endl;
    std::cerr << "    -g, --gbwt-name X       use the GBWT index in file X (required with a non-GBZ graph)" << std::endl;
    std::cerr << "    -p, --progress          show progress information" << std::endl;
    std::cerr << "    -t, --threads N         use N threads for index construction (default " << get_default_threads_rymer() << ")" << std::endl;
    std::cerr << "                            (using more than " << DEFAULT_MAX_THREADS << " threads rarely helps)" << std::endl;
    std::cerr << std::endl;
}

int main_rymer(int argc, char** argv) {

    if (argc <= 5) {
        help_rymer(argv);
        return 1;
    }

    // Command-line options.
    std::string output_name, distance_name, load_index, gbwt_name, graph_name;
    bool use_syncmers = false;
    bool progress = false;
    int threads = get_default_threads_rymer();

    int c;
    optind = 2; // force optind past command positional argument
    while (true) {
        static struct option long_options[] =
        {
            { "gbwt-name", required_argument, 0, 'g' },
            { "output-name", required_argument, 0, 'o' },
            { "index-name", required_argument, 0, 'i' }, // deprecated
            { "kmer-length", required_argument, 0, 'k' },
            { "window-length", required_argument, 0, 'w' },
            { "bounded-syncmers", no_argument, 0, 'b' }, // deprecated
            { "closed-syncmers", no_argument, 0, 'c' },
            { "smer-length", required_argument, 0, 's' },
            { "distance-index", required_argument, 0, 'd' },
            { "load-index", required_argument, 0, 'l' },
            { "gbwt-graph", no_argument, 0, 'G' }, // deprecated
            { "progress", no_argument, 0, 'p' },
            { "threads", required_argument, 0, 't' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "g:o:i:k:w:bcs:d:l:Gpt:h", long_options, &option_index);
        if (c == -1) { break; } // End of options.

        switch (c)
        {
        case 'g':
            gbwt_name = optarg;
            break;
        case 'o':
            output_name = optarg;
            break;
        case 'i':
            std::cerr << "[vg rymer] warning: --index-name is deprecated, use --output-name instead" << std::endl;
            output_name = optarg;
            break;
        case 'k':
            IndexingParameters::rymer_k = parse<size_t>(optarg);
            break;
        case 'w':
            IndexingParameters::rymer_w = parse<size_t>(optarg);
            break;
        case 'b':
            std::cerr << "[vg rymer] warning: --bounded-syncmers is deprecated, use --closed-syncmers instead" << std::endl;
            use_syncmers = true;
            break;
        case 'c':
            use_syncmers = true;
            break;
        case 's':
            IndexingParameters::minimizer_s = parse<size_t>(optarg);
            break;
        case 'd':
            distance_name = optarg;
            break;
        case 'l':
            load_index = optarg;
            break;
        case 'G':
            std::cerr << "[vg rymer] warning: --gbwt-graph is deprecated, graph format is now autodetected" << std::endl;
            break;
        case 'p':
            progress = true;
            break;
        case 't':
            threads = parse<int>(optarg);
            threads = std::min(threads, omp_get_max_threads());
            threads = std::max(threads, 1);
            break;

        case 'h':
        case '?':
            help_rymer(argv);
            return 1;
        default:
            std::abort();
        }
    }
    if (output_name.empty()) {
        std::cerr << "[vg rymer] error: option --output-name is required" << std::endl;
        return 1;
    }
    if (optind + 1 != argc) {
        help_rymer(argv);
        return 1;
    }
    graph_name = argv[optind];
    omp_set_num_threads(threads);

    double start = gbwt::readTimer();
   
    // We use GBWT and GBWTGraph in this GBZ wrapper.
    unique_ptr<gbwtgraph::GBZ> gbz;
   
    // Load whatever the graph argument is
    if (progress) {
        std::cerr << "Loading input graph from " << graph_name << std::endl;
    }
    auto input = vg::io::VPKG::try_load_first<gbwtgraph::GBZ, gbwtgraph::GBWTGraph, HandleGraph>(graph_name);
    if (get<0>(input)) {
        // We loaded a GBZ directly
        gbz = std::move(get<0>(input));
    } else if (get<1>(input)) {
        // We loaded a GBWTGraph and need to pair it with a GBWT
        gbz.reset(new gbwtgraph::GBZ());
        gbz->graph = std::move(*get<1>(input));
        
        if (gbwt_name.empty()) {
            std::cerr << "[vg rymer] error: option --gbwt-name is required when using a GBWTGraph" << std::endl;
            return 1;
        }
        
        // Go get the GBWT
        load_gbwt(gbz->index, gbwt_name, progress);
        // And attach them together
        gbz->graph.set_gbwt(gbz->index);
    } else if (get<2>(input)) {
        // We got a normal HandleGraph
        
        if (gbwt_name.empty()) {
            std::cerr << "[vg rymer] error: option --gbwt-name is required when using a HandleGraph" << std::endl;
            return 1;
        }
        
        if (progress) {
            std::cerr << "Loading GBWT from " << gbwt_name << std::endl;
        }
        std::unique_ptr<gbwt::GBWT> gbwt_index(vg::io::VPKG::load_one<gbwt::GBWT>(gbwt_name));
        if (progress) {
            std::cerr << "Building GBWTGraph" << std::endl;
        }
        gbz.reset(new gbwtgraph::GBZ(gbwt_index, *get<2>(input)));
    } else {
        std::cerr << "[vg rymer] error: input graph is not a GBZ, GBWTGraph, or HandleGraph." << std::endl;
        return 1;
    }

if (IndexingParameters::rymer_k < 10){throw runtime_error("[vg rymer] k must be no less than 10 for the RYmer index");}

    // Minimizer index.
    std::unique_ptr<gbwtgraph::DefaultMinimizerIndex> rymer_index;
    if (load_index.empty()) {

        rymer_index = std::make_unique<gbwtgraph::DefaultMinimizerIndex>(IndexingParameters::rymer_k,
            (use_syncmers ? IndexingParameters::minimizer_s : IndexingParameters::rymer_w),
            use_syncmers);

    } else {
        if (progress) {
            std::cerr << "Loading MinimizerIndex from " << load_index << std::endl;
        }
        rymer_index = vg::io::VPKG::load_one<gbwtgraph::DefaultMinimizerIndex>(load_index);
    }

    // Distance index.
    std::unique_ptr<SnarlDistanceIndex> distance_index;
    if (!distance_name.empty()) {
        // new distance index
        if (progress) {
            std::cerr << "Loading SnarlDistanceIndex from " << distance_name << std::endl;
        }
        distance_index = vg::io::VPKG::load_one<SnarlDistanceIndex>(distance_name);
    }

    // Build the index.
    if (progress) {
        std::cerr << "Building RYmerIndex with k = " << rymer_index->k();
        if (rymer_index->uses_syncmers()) {
            std::cerr << ", s = " << rymer_index->s();
        } else {
            std::cerr << ", w = " << rymer_index->w();
        }
        std::cerr << std::endl;
    }


  if (distance_name.empty()) {
    gbwtgraph::index_haplotypes(gbz->graph, true, *rymer_index, [](const pos_t&) -> gbwtgraph::payload_type {
        return MIPayload::NO_CODE;
    }, IndexingParameters::rymer_k);
} else {
    gbwtgraph::index_haplotypes(gbz->graph, true, *rymer_index, [&](const pos_t& pos) -> gbwtgraph::payload_type {
        return MIPayload::encode(get_minimizer_distances(*distance_index,pos));
    }, IndexingParameters::rymer_k);
}

    // Index statistics.
    if (progress) {
        std::cerr << rymer_index->size() << " keys (" << rymer_index->unique_keys() << " unique)" << std::endl;
        std::cerr << "RYmer occurrences: " << rymer_index->values() << std::endl;
        std::cerr << "Load factor: " << rymer_index->load_factor() << std::endl;
        double seconds = gbwt::readTimer() - start;
        std::cerr << "Construction so far: " << seconds << " seconds" << std::endl;
        std::cerr << "Hash table size: " << rymer_index->capacity() << std::endl;
    }

    std::string output_name_minimizer = output_name; // Copy of the original

    std::size_t pos = output_name_minimizer.rfind(".ry");
    if (pos != std::string::npos) {
        output_name_minimizer.replace(pos, 3, ".min"); // Replace .min with .ry in the copy
    }

    // Serialize the index.
    save_minimizer(*rymer_index, output_name);

    if (progress) {
        double seconds = gbwt::readTimer() - start;
        std::cerr << "Time usage: " << seconds << " seconds" << std::endl;
        std::cerr << "Memory usage: " << gbwt::inGigabytes(gbwt::memoryUsage()) << " GiB" << std::endl;
    }

    return 0;
}

// Register subcommand
static vg::subcommand::Subcommand vg_rymer("rymer", "build a minimizer index or a syncmer index", vg::subcommand::TOOLKIT, main_rymer);

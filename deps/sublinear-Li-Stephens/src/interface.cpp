#include "haplotype_manager.hpp"
#include "reference.hpp"
#include "probability.hpp"
#include "reference_sequence.hpp"
#include "input_haplotype.hpp"
#include "interface.h"
#include <iostream>
#include <cmath>
#include <string>
#include <cstring>

using namespace std;

size_t haplotypeManager_get_num_shared_sites(haplotypeManager* hap_manager) {
  return hap_manager->shared_sites();
}

int haplotypeManager_read_index_is_shared(haplotypeManager* hap_manager, size_t read_site_index) {
  bool to_return = hap_manager->read_index_is_shared(read_site_index);
  return (int)to_return;
}

haplotypeManager* haplotypeManager_build_abs_bound(
            char* reference_sequence,
            size_t ref_seq_length,
            size_t number_of_ref_sites,
            size_t* positions_of_ref_sites,
            size_t number_of_haplotypes,
            char* alleles_by_site_and_haplotype,
            double mutation_penalty, 
            double recombination_penalty,
            size_t read_DP_ref_start,
            size_t read_DP_site_count,
            size_t* read_DP_site_offsets,
            char* read_DP_sequence, 
            double threshold) {
  penaltySet* penalties = new penaltySet(recombination_penalty, 
                                         mutation_penalty, 
                                         number_of_haplotypes);
  vector<alleleValue> ref_site_allele_vector;
  for(size_t i = 0; i < number_of_ref_sites; i++) {
    ref_site_allele_vector.push_back(allele::from_char(reference_sequence[positions_of_ref_sites[i]]));
  }
  vector<size_t> ref_site_position_vector = 
            vector<size_t>(positions_of_ref_sites, positions_of_ref_sites + number_of_ref_sites);
  
  siteIndex* reference = new siteIndex(ref_site_position_vector, ref_seq_length);  
    
  vector<vector<alleleValue> > haplotypes = 
            vector<vector<alleleValue> >(number_of_haplotypes,
                                         vector<alleleValue>(number_of_ref_sites, A));
                                         
  for(size_t i = 0; i < number_of_haplotypes; i++) {
    for(size_t j = 0; j < number_of_ref_sites; j++) {
      haplotypes[i][j] = allele::from_char(alleles_by_site_and_haplotype[i*number_of_haplotypes + j]);
    }
  }
  
  haplotypeCohort* cohort = 
            new haplotypeCohort(haplotypes, reference);  
  
  vector<size_t> read_sites(read_DP_site_offsets,
                            read_DP_site_offsets + read_DP_site_count);
  haplotypeManager* hap_manager = 
              new haplotypeManager(reference, 
                                   cohort, 
                                   penalties, 
                                   reference_sequence,
                                   read_sites,
                                   read_DP_sequence, 
                                   read_DP_ref_start);
  hap_manager->build_entire_tree(threshold);
  return hap_manager;
}

haplotypeManager* haplotypeManager_build_interval_bound(
            char* reference_sequence,
            size_t ref_seq_length,
            size_t number_of_ref_sites,
            size_t* positions_of_ref_sites,
            size_t number_of_haplotypes,
            char* alleles_by_site_and_haplotype,
            double mutation_penalty, 
            double recombination_penalty,
            size_t read_DP_ref_start,
            size_t read_DP_site_count,
            size_t* read_DP_site_offsets,
            char* read_DP_sequence, 
            double threshold) {
  penaltySet* penalties = new penaltySet(recombination_penalty, 
                                         mutation_penalty, 
                                         number_of_haplotypes);
  vector<alleleValue> ref_site_allele_vector;
  for(size_t i = 0; i < number_of_ref_sites; i++) {
    ref_site_allele_vector.push_back(allele::from_char(reference_sequence[positions_of_ref_sites[i]]));
  }
  vector<size_t> ref_site_position_vector = 
            vector<size_t>(positions_of_ref_sites, positions_of_ref_sites + number_of_ref_sites);
  
  siteIndex* reference =
            new siteIndex(ref_site_position_vector, 
                                         ref_seq_length);  
    
  vector<vector<alleleValue> > haplotypes = 
            vector<vector<alleleValue> >(number_of_haplotypes,
                                         vector<alleleValue>(number_of_ref_sites, A));
                                         
  for(size_t i = 0; i < number_of_haplotypes; i++) {
    for(size_t j = 0; j < number_of_ref_sites; j++) {
      haplotypes[i][j] = allele::from_char(alleles_by_site_and_haplotype[i*number_of_haplotypes + j]);
    }
  }
  
  haplotypeCohort* cohort = 
            new haplotypeCohort(haplotypes, reference);  
  
  vector<size_t> read_sites(read_DP_site_offsets,
                            read_DP_site_offsets + read_DP_site_count);
  haplotypeManager* hap_manager = 
              new haplotypeManager(reference, 
                                   cohort, 
                                   penalties, 
                                   reference_sequence,
                                   read_sites,
                                   read_DP_sequence, 
                                   read_DP_ref_start);
  hap_manager->build_entire_tree_interval(threshold);
  return hap_manager;
}

haplotypeManager* haplotypeManager_build_from_idx(
            char* reference_sequence,
            size_t ref_seq_length,
            siteIndex* reference,
            haplotypeCohort* cohort,
            double mutation_penalty, 
            double recombination_penalty,
            size_t read_DP_ref_start,
            size_t read_DP_site_count,
            size_t* read_DP_site_offsets,
            char* read_DP_sequence) {
  penaltySet* penalties = new penaltySet(recombination_penalty, 
                                         mutation_penalty, 
                                         cohort->get_n_haplotypes());
  vector<size_t> read_sites(read_DP_site_offsets,
                            read_DP_site_offsets + read_DP_site_count);

  haplotypeManager* hap_manager = 
              new haplotypeManager(reference, 
                                   cohort, 
                                   penalties, 
                                   reference_sequence,
                                   read_sites,
                                   read_DP_sequence, 
                                   read_DP_ref_start);

  return hap_manager;
}

void haplotypeManager_build_tree_interval(haplotypeManager* hap_manager,
                                          double threshold) {
  hap_manager->build_entire_tree_interval(threshold);
}

void haplotypeStateNode_get_next_options(
            haplotypeStateNode* n, 
            haplotypeStateNode** option_array) {
  size_t number_of_children = n->number_of_children();
  vector<haplotypeStateNode*> children = n->get_unordered_children();
  for(size_t i = 0; i < number_of_children; i++) {
    option_array[i] = children[i];
  }
  for(size_t i = number_of_children; i < 5; i++) {
    option_array[i] = nullptr;
  }
}

haplotypeStateNode* haplotypeStateNode_get_child(
            haplotypeStateNode* parent,
            char allele) {
  return parent->get_child(allele::from_char(allele));
}

size_t haplotypeStateNode_number_of_children(haplotypeStateNode* n) {
  return n->number_of_children();
}

double haplotypeStateNode_local_probability(
            haplotypeStateNode* n, 
            haplotypeManager* hap_manager) {
  const penaltySet* penalties = hap_manager->get_penalties();
  return n->prefix_likelihood() - n->max_prefix_likelihood(penalties);
}

double haplotypeStateNode_total_probability(haplotypeStateNode* n) {
  return n->prefix_likelihood();
}

char haplotypeStateNode_allele(haplotypeStateNode* n) {
  return allele::to_char(n->get_allele());
}

void haplotypeManager_delete(haplotypeManager* to_delete) {
  delete to_delete->get_reference();
  delete to_delete->get_cohort();
  delete to_delete->get_penalties();
  delete to_delete;
}

haplotypeStateNode* haplotypeManager_get_root_node(
            haplotypeManager* hap_manager) {
  return hap_manager->get_tree()->root;
}

haplotypeStateNode* haplotypeStateNode_get_parent(haplotypeStateNode* n) {
  return n->get_parent();
}

void haplotypeManager_print_transition_likelihoods(haplotypeManager* hap_manager) {
  hap_manager->print_tree_transitions();
}

void haplotypeManager_print_prefix_likelihoods(haplotypeManager* hap_manager) {
  hap_manager->print_tree();
}

void haplotypeManager_print_terminal_nodes(haplotypeManager* hap_manager) {
  hap_manager->print_terminal_nodes();
}

haplotypeCohort* haplotypeCohort_init_empty(size_t number_of_haplotypes, siteIndex* ref) {
  return new haplotypeCohort(number_of_haplotypes, ref);
}

size_t haplotypeCohort_sum_information_content(haplotypeCohort* cohort, inputHaplotype* query) {
  return cohort->sum_information_content(query->get_alleles(), query->get_start_site());
}

size_t haplotypeCohort_sum_total_information(haplotypeCohort* cohort) {
  size_t to_return = 0;
  for(size_t i = 0; i < cohort->get_n_sites(); i++) {
    to_return += cohort->get_total_information(i);
  }
  return to_return;
}

void haplotypeCohort_print_deltas(haplotypeCohort* cohort, inputHaplotype* query) {
  for(size_t i = 0; i < query->get_alleles().size(); i++) {
    cout << cohort->get_information_content(i, query->get_alleles()[i]) << "\t";
  }
  cout << endl;
}

size_t haplotypeCohort_n_sites(haplotypeCohort* cohort) {
  return cohort->get_n_sites();
}

siteIndex* siteIndex_init_empty(size_t global_offset) {
  return new siteIndex(global_offset);
}

int64_t siteIndex_add_site(siteIndex* reference, size_t position) {
  reference->add_site(position);
}

void haplotypeCohort_add_record(haplotypeCohort* cohort) {
  cohort->add_record();
}

void haplotypeCohort_set_sample_allele(
            haplotypeCohort* cohort, size_t site, size_t sample, char allele) {
  cohort->set_sample_allele(site, sample, allele::from_char(allele));
}

void siteIndex_calc_spans(siteIndex* reference, size_t length) {
  reference->calculate_final_span_length(length);
}

void haplotypeCohort_populate_counts(haplotypeCohort* cohort) {
  cohort->populate_allele_counts();
}

void haplotypeCohort_delete(haplotypeCohort* cohort) {
  delete cohort;
}

void siteIndex_set_initial_span(siteIndex* ref, size_t length) {
  ref->set_initial_span(length);
}

size_t haplotypeCohort_n_haplotypes(haplotypeCohort* cohort) {
  return cohort->get_n_haplotypes();
}

size_t siteIndex_n_sites(siteIndex* reference) {
  return reference->number_of_sites();
}

void siteIndex_delete(siteIndex* reference) {
  delete reference;
}

int haplotypeManager_is_shared_site(haplotypeManager* hap_manager, size_t position) {
  return (int)(hap_manager->get_reference()->is_site(position));
}

void haplotypeManager_init_opt_idx(haplotypeManager* hap_manager,
                  								 char* r_alleles_1,
                  								 char* r_alleles_2) {
  char* ss_values_1 = (char*)malloc(hap_manager->shared_sites());
  char* ss_values_2 = (char*)malloc(hap_manager->shared_sites());
  for(size_t i = 0; i < hap_manager->shared_sites(); i++) {
    ss_values_1[i] = r_alleles_1[hap_manager->shared_index_to_read_index(i)];
    ss_values_2[i] = r_alleles_2[hap_manager->shared_index_to_read_index(i)];
  }
  hap_manager->set_option_index(ss_values_1, ss_values_2);
  free(ss_values_1);
  free(ss_values_2);
}

inputHaplotype* inputHaplotype_build(const char* ref_seq, 
                          const char* observed_haplotype, 
                          siteIndex* ref_struct,
                          size_t start_position) {
  inputHaplotype* to_return = new inputHaplotype(ref_seq, observed_haplotype, 
                                                 ref_struct, start_position, 
                                                 ref_struct->length_in_bp());
  to_return->validate();
  return to_return;
}

size_t inputHaplotype_n_sites(inputHaplotype* input_haplotype) {
  return input_haplotype->number_of_sites();
}

size_t inputHaplotype_start_site(inputHaplotype* input_haplotype) {
  return input_haplotype->get_start_site();
}

void inputHaplotype_delete(inputHaplotype* in_hap) {
  delete in_hap;
}

fastFwdAlgState* fastFwdAlgState_initialize(siteIndex* reference,
                                            penaltySet* penalties,
                                            haplotypeCohort* cohort) {
  fastFwdAlgState* to_return = new fastFwdAlgState(reference, penalties, cohort);
  return to_return;
}

double fastFwdAlgState_score(fastFwdAlgState* hap_matrix, inputHaplotype* observed_haplotype) {
  double to_return = hap_matrix->calculate_probability(observed_haplotype);
  return to_return;
}

void fastFwdAlgState_delete(fastFwdAlgState* hap_matrix) {
  delete hap_matrix;
}

penaltySet* penaltySet_build(double recombination_penalty,
                             double mutation_penalty,
                             size_t number_of_haplotypes) {
  penaltySet* to_return = new penaltySet(recombination_penalty, 
                                         mutation_penalty, 
                                         number_of_haplotypes);
  return to_return;
}

void penaltySet_delete(penaltySet* penalty_set) {
  delete penalty_set;  
}

slowFwdSolver* slowFwd_initialize(siteIndex* reference, penaltySet* penalties, haplotypeCohort* cohort) {
  return new slowFwdSolver(reference, penalties, cohort);
}

double slowFwd_solve_quadratic(slowFwdSolver* solver, inputHaplotype* observed_haplotype) {
  return solver->calculate_probability_quadratic(observed_haplotype->get_alleles(), observed_haplotype->get_start_site());
}

double slowFwd_solve_linear(slowFwdSolver* solver, inputHaplotype* observed_haplotype) {
  return solver->calculate_probability_linear(observed_haplotype->get_alleles(), observed_haplotype->get_start_site());
}

inputHaplotype* haplotypeCohort_random_haplo(haplotypeCohort* cohort, siteIndex* reference, size_t generations, penaltySet* penalties, size_t length) {
  size_t start_site = 0;
  if(start_site < cohort->get_n_sites()) {
    size_t end_site = cohort->get_n_sites() - 1;
    vector<alleleValue> random_haplo = cohort->rand_desc_haplo(generations, penalties->rho, penalties->mu, start_site, end_site);
    inputHaplotype* to_return = new inputHaplotype(random_haplo, vector<size_t>(random_haplo.size(), 0), reference, cohort->get_reference()->get_position(start_site), cohort->get_reference()->get_position(end_site) - cohort->get_reference()->get_position(start_site));
    to_return->validate();
    return to_return;
  } else {
    return new inputHaplotype(reference);
  }
}

siteIndex* haplotypeCohort_get_reference(haplotypeCohort* cohort) {
  return cohort->get_reference();
}

haplotypeCohort* random_cohort(size_t n_bp, size_t k, haplotypeCohort* cohort, siteIndex* reference) {
  size_t interval_start = reference->rand_interval_start(n_bp);
  size_t interval_end = interval_start + n_bp;
  size_t site_start = reference->find_site_above(interval_start);
  size_t site_end = reference->find_site_below(interval_end);
  return cohort->subset(site_start, site_end, k);
}

void remove_homogeneous_sites(haplotypeCohort* cohort) {
  cohort->remove_homogeneous_sites();
}

void slowFwdSolver_delete(slowFwdSolver* to_delete) {
  delete to_delete;
}

void n_random_uints(size_t* to_return, size_t N, size_t max_value) {
  vector<size_t> temp = haploRandom::n_unique_uints(N, max_value);
  memcpy(to_return, &temp[0], sizeof(size_t)*N);
}

// void slowFwdSolver_u_values_stats(slowFwdSolver* solver, inputHaplotype* observed_haplotype) {
//   pair<vector<double>, vector<size_t> > temp = solver->sequence_statistics(observed_haplotype->get_alleles(), observed_haplotype->get_start_site());
//   cout << "unique values\t" << temp.first[0] << "\t" << temp.second[0] << "\t";
//   cout << "unique active values\t" << temp.first[1] << "\t" << temp.second[1] << "\t";
//   cout << "active rows\t" << temp.first[2] << "\t" << temp.second[2] << "\t";
//   cout << "history length\t" << temp.first[3] << "\t" << temp.second[3] << "\t";
//   cout << "active history length\t" << temp.first[4] << "\t" << temp.second[4] << "\t";
//   cout << "map classes\t" << temp.first[5] << "\t" << temp.second[5] << "\t";
//   cout << "active map classes\t" << temp.first[6] << "\t" << temp.second[6] << "\t";
//   cout << "times haplotype active\t" << temp.first[7] << "\t" << temp.second[7] << "\t";
//   cout << endl;
// }
// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <inttypes.h>
#include <climits>
#include <stack>
#include <unordered_map>
#include <string>
#include <array>
#include <memory>
#include <atomic>
#include <string_view>
#include <thread>
#include "v_array.h"
#include "array_parameters.h"
#include "loss_functions.h"
#include "example.h"
#include "config.h"
#include "learner.h"
#include <time.h>
#include "hash.h"
#include "crossplat_compat.h"
#include "error_reporting.h"
#include "constant.h"
#include "rand48.h"
#include "hashstring.h"
#include "decision_scores.h"
#include "feature_group.h"
#include "rand_state.h"

#include "options.h"
#include "version.h"
#include "kskip_ngram_transformer.h"

typedef float weight;

typedef std::unordered_map<std::string, std::unique_ptr<features>> feature_dict;
typedef vw::LEARNER::base_learner* (*reduction_setup_fn)(vw::config::options_i&, workspace&);

using options_deleter_type = void (*)(vw::config::options_i*);

struct shared_data;

struct dictionary_info
{
  std::string name;
  uint64_t file_hash;
  std::shared_ptr<feature_dict> dict;
};

struct vw_logger
{
  bool quiet;

  vw_logger() : quiet(false) {}

  vw_logger(const vw_logger& other) = delete;
  vw_logger& operator=(const vw_logger& other) = delete;
};

#ifdef BUILD_EXTERNAL_PARSER
// forward declarations
namespace vw
{
namespace external
{
class parser;
struct parser_options;
}  // namespace external
}  // namespace vw
#endif

namespace vw
{
namespace parsers
{
namespace flatbuffer
{
class parser;
}
}  // namespace parsers
}  // namespace vw

struct trace_message_wrapper
{
  void* _inner_context;
  trace_message_t _trace_message;

  trace_message_wrapper(void* context, trace_message_t trace_message)
      : _inner_context(context), _trace_message(trace_message)
  {
  }
  ~trace_message_wrapper() = default;
};

struct workspace
{
private:
  std::shared_ptr<rand_state> _random_state_sp = std::make_shared<rand_state>();  // per instance random_state

public:
  shared_data* sd;

  parser* example_parser;
  std::thread parse_thread;

  bool chain_hash_json = false;

  vw::LEARNER::base_learner* l;         // the top level learner
  vw::LEARNER::single_learner* scorer;  // a scoring function
  vw::LEARNER::base_learner*
      cost_sensitive;  // a cost sensitive learning algorithm.  can be single or multi line learner

  void learn(example&);
  void learn(multi_ex&);
  void predict(example&);
  void predict(multi_ex&);
  void finish_example(example&);
  void finish_example(multi_ex&);

  void (*set_minmax)(shared_data* sd, float label);

  uint64_t current_pass;

  uint32_t num_bits;  // log_2 of the number of features.
  bool default_bits;

  uint32_t hash_seed;

#ifdef BUILD_FLATBUFFERS
  std::unique_ptr<vw::parsers::flatbuffer::parser> flat_converter;
#endif

#ifdef BUILD_EXTERNAL_PARSER
  std::unique_ptr<vw::external::parser> external_parser;
#endif
  std::string data_filename;

  bool save_per_pass;
  float initial_weight;
  float initial_constant;

  bool bfgs;
  bool hessian_on;

  bool save_resume;
  bool preserve_performance_counters;
  std::string id;

  vw::version_struct model_file_ver;
  double normalized_sum_norm_x;
  bool vw_is_main = false;  // true if vw is executable; false in library mode

  // error reporting
  std::shared_ptr<trace_message_wrapper> trace_message_wrapper_context;
  std::unique_ptr<std::ostream> trace_message;

  std::unique_ptr<vw::config::options_i, options_deleter_type> options;

  uint32_t wpp;

  std::unique_ptr<vw::io::writer> stdout_adapter;

  std::vector<std::string> initial_regressors;

  std::string feature_mask;

  std::string per_feature_regularizer_input;
  std::string per_feature_regularizer_output;
  std::string per_feature_regularizer_text;

  float l1_lambda;  // the level of l_1 regularization to impose.
  float l2_lambda;  // the level of l_2 regularization to impose.
  bool no_bias;     // no bias in regularization
  float power_t;    // the power on learning rate decay.
  int reg_mode;

  size_t pass_length;
  size_t numpasses;
  size_t passes_complete;
  uint64_t parse_mask;  // 1 << num_bits -1
  bool permutations;    // if true - permutations of features generated instead of simple combinations. false by default

  // Referenced by examples as their set of interactions. Can be overriden by reductions.
  std::vector<std::vector<namespace_index>> interactions;
  bool ignore_some;
  std::array<bool, NUM_NAMESPACES> ignore;  // a set of namespaces to ignore
  bool ignore_some_linear;

  bool redefine_some;                                  // --redefine param was used
  std::array<unsigned char, NUM_NAMESPACES> redefine;  // keeps new chars for namespaces
  std::unique_ptr<vw::kskip_ngram_transformer> skip_gram_transformer;
  std::vector<std::string> limit_strings;      // descriptor of feature limits
  std::array<uint32_t, NUM_NAMESPACES> limit;  // count to limit features by
  std::array<uint64_t, NUM_NAMESPACES>
      affix_features;  // affixes to generate (up to 16 per namespace - 4 bits per affix)
  std::array<bool, NUM_NAMESPACES> spelling_features;  // generate spelling features for which namespace
  std::vector<std::string> dictionary_path;            // where to look for dictionaries

  // feature_dict can be created in either loaded_dictionaries or namespace_dictionaries.
  // use shared pointers to avoid the question of ownership
  std::vector<dictionary_info> loaded_dictionaries;  // which dictionaries have we loaded from a file to memory?
  // This array is required to be value initialized so that the std::vectors are constructed.
  std::array<std::vector<std::shared_ptr<feature_dict>>, NUM_NAMESPACES>
      namespace_dictionaries{};  // each namespace has a list of dictionaries attached to it

  VW_DEPRECATED("delete_prediction has been deprecated")
  void (*delete_prediction)(void*);

  vw_logger logger;
  bool audit;     // should I print lots of debugging information?
  bool training;  // Should I train if lable data is available?
  bool invariant_updates;  // Should we use importance aware/safe updates
  uint64_t random_seed;
  bool random_weights;
  bool random_positive_weights;  // for initialize_regressor w/ new_mf
  bool normal_weights;
  bool tnormal_weights;
  bool add_constant;
  bool nonormalize;
  bool do_reset_source;
  bool holdout_set_off;
  bool early_terminate;
  uint32_t holdout_period;
  uint32_t holdout_after;
  size_t check_holdout_every_n_passes;  // default: 1, but search might want to set it higher if you spend multiple
                                        // passes learning a single policy

  size_t normalized_idx;  // offset idx where the norm is stored (1 or 2 depending on whether adaptive is true)

  uint32_t lda;

  std::string text_regressor_name;
  std::string inv_hash_regressor_name;

  size_t length() { return (static_cast<size_t>(1)) << num_bits; };

  std::vector<std::tuple<std::string, reduction_setup_fn>> reduction_stack;
  std::vector<std::string> enabled_reductions;

  // Prediction output
  std::vector<std::unique_ptr<vw::io::writer>> final_prediction_sink;  // set to send global predictions to.
  std::unique_ptr<vw::io::writer> raw_prediction;                      // file descriptors for text output.

  VW_DEPRECATED("print has been deprecated, use print_by_ref")
  void (*print)(vw::io::writer*, float, float, v_array<char>);
  void (*print_by_ref)(vw::io::writer*, float, float, const v_array<char>&);
  VW_DEPRECATED("print_text has been deprecated, use print_text_by_ref")
  void (*print_text)(vw::io::writer*, std::string, v_array<char>);
  void (*print_text_by_ref)(vw::io::writer*, const std::string&, const v_array<char>&);
  std::unique_ptr<loss_function> loss;

  VW_DEPRECATED("This is unused and will be removed")
  char* program_name;

  bool stdin_off;

  // runtime accounting variables.
  float initial_t;
  float eta;  // learning rate control.
  float eta_decay_rate;
  time_t init_time;

  std::string final_regressor_name;

  parameters weights;

  size_t max_examples;  // for TLC

  bool hash_inv;
  bool print_invert;

  // Set by --progress <arg>
  bool progress_add;   // additive (rather than multiplicative) progress dumps
  float progress_arg;  // next update progress dump multiplier

  std::map<uint64_t, std::string> index_name_map;

  // hack to support cb model loading into ccb reduction
  bool is_ccb_input_model = false;

  workspace();
  ~workspace();
  std::shared_ptr<rand_state> get_random_state() { return _random_state_sp; }

  workspace(const workspace&) = delete;
  workspace& operator=(const workspace&) = delete;

  // vw object cannot be moved as many objects hold a pointer to it.
  // That pointer would be invalidated if it were to be moved.
  workspace(const workspace&&) = delete;
  workspace& operator=(const workspace&&) = delete;

  std::string get_setupfn_name(reduction_setup_fn setup);
  void build_setupfn_name_dict();

private:
  std::unordered_map<reduction_setup_fn, std::string> _setup_name_map;
};

VW_DEPRECATED("Use print_result_by_ref instead")
void print_result(vw::io::writer* f, float res, float weight, v_array<char> tag);
void print_result_by_ref(vw::io::writer* f, float res, float weight, const v_array<char>& tag);

void noop_mm(shared_data*, float label);
void compile_gram(
    std::vector<std::string> grams, std::array<uint32_t, NUM_NAMESPACES>& dest, char* descriptor, bool quiet);
void compile_limits(std::vector<std::string> limits, std::array<uint32_t, NUM_NAMESPACES>& dest, bool quiet);

VW_DEPRECATED("Use print_tag_by_ref instead")
int print_tag(std::stringstream& ss, v_array<char> tag);
int print_tag_by_ref(std::stringstream& ss, const v_array<char>& tag);

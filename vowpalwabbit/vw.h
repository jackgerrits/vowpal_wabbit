// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

/*! \mainpage
 *
 * For the primary interface see:
 * - \link vw vw namespace documentation \endlink
 *
 * For other docs see:
 * - [Project website](https://vowpalwabbit.org)
 * - [Wiki](https://github.com/VowpalWabbit/vowpal_wabbit/wiki)
 * - C++ build instructions:
 *     - [Install dependencies](https://github.com/VowpalWabbit/vowpal_wabbit/wiki/Dependencies)
 *     - [Build](https://github.com/VowpalWabbit/vowpal_wabbit/wiki/Building)
 *     - [Install](https://github.com/VowpalWabbit/vowpal_wabbit/wiki/Installing)
 * - [Install other languages](https://vowpalwabbit.org/start.html)
 * - [Tutorials](https://github.com/VowpalWabbit/vowpal_wabbit/wiki/Tutorial)
 */

#ifdef _WIN32
#  ifdef LEAKCHECK
// Visual Leak Detector for memory leak detection on Windows
#    include <vld.h>
#  endif
#endif

#include "global_data.h"
#include "example.h"
#include "hash.h"
#include "simple_label.h"
#include "parser.h"
#include "parse_example.h"

#include "options.h"

namespace vw
{
/*    Caveats:
    (1) Some commandline parameters do not make sense as a library.
    (2) The code is not yet reentrant.
   */
workspace* initialize(std::unique_ptr<config::options_i, options_deleter_type> options, io_buf* model = nullptr,
    bool skipModelLoad = false, trace_message_t trace_listener = nullptr, void* trace_context = nullptr);
workspace* initialize(config::options_i& options, io_buf* model = nullptr, bool skipModelLoad = false,
    trace_message_t trace_listener = nullptr, void* trace_context = nullptr);
workspace* initialize(std::string s, io_buf* model = nullptr, bool skipModelLoad = false,
    trace_message_t trace_listener = nullptr, void* trace_context = nullptr);
workspace* initialize(int argc, char* argv[], io_buf* model = nullptr, bool skipModelLoad = false,
    trace_message_t trace_listener = nullptr, void* trace_context = nullptr);
workspace* seed_vw_model(workspace* vw_model, std::string extra_args, trace_message_t trace_listener = nullptr,
    void* trace_context = nullptr);
// Allows the input command line string to have spaces escaped by '\'
workspace* initialize_escaped(std::string const& s, io_buf* model = nullptr, bool skipModelLoad = false,
    trace_message_t trace_listener = nullptr, void* trace_context = nullptr);

void cmd_string_replace_value(std::stringstream*& ss, std::string flag_to_replace, std::string new_value);

// The argv array from both of these functions must be freed.
char** to_argv(std::string const& s, int& argc);
char** to_argv_escaped(std::string const& s, int& argc);
void free_args(int argc, char* argv[]);

const char* are_features_compatible(workspace& vw1, workspace& vw2);

/*
  Call finish() after you are done with the vw instance.  This cleans up memory usage.
 */
void finish(workspace& all, bool delete_all = true);

void start_parser(workspace& all);
void end_parser(workspace& all);
bool is_ring_example(workspace& all, example* ae);

struct primitive_feature_space  // just a helper definition.
{
  unsigned char name;
  feature* fs;
  size_t len;
};

// The next commands deal with creating examples.  Caution: vw does not all allow creation of many examples at once by
// default.  You can adjust the exact number by tweaking ring_size.

/* The simplest of two ways to create an example.  An example_line is the literal line in a vw-format datafile.
 */
example* read_example(workspace& all, char* example_line);
example* read_example(workspace& all, std::string example_line);

// The more complex way to create an example.

// after you create and fill feature_spaces, get an example with everything filled in.
example* import_example(workspace& all, const std::string& label, primitive_feature_space* features, size_t len);

// callers must free memory using dealloc_examples
// this interface must be used with care as finish_example is a no-op for these examples.
// thus any delay introduced when freeing examples must be at least as long as the one
// introduced by all.l->finish_example implementations.
// e.g. multiline examples as used by cb_adf must not be released before the finishing newline example.
example* alloc_examples(size_t count);
void dealloc_examples(example* example_ptr, size_t count);

void parse_example_label(workspace& all, example& ec, std::string label);
void setup_examples(workspace& all, v_array<example*>& examples);
void setup_example(workspace& all, example* ae);
example* new_unused_example(workspace& all);
example* get_example(parser* pf);
float get_topic_prediction(example* ec, size_t i);  // i=0 to max topic -1
float get_label(example* ec);
float get_importance(example* ec);
float get_initial(example* ec);
float get_cost_sensitive_prediction(example* ec);
v_array<float>& get_cost_sensitive_prediction_confidence_scores(example* ec);
float get_action_score(example* ec, size_t i);
size_t get_action_score_length(example* ec);
size_t get_tag_length(example* ec);
const char* get_tag(example* ec);
size_t get_feature_number(example* ec);
float get_confidence(example* ec);
feature* get_features(workspace& all, example* ec, size_t& feature_number);
void return_features(feature* f);

void add_constant_feature(workspace& all, example* ec);
void add_label(example* ec, float label, float weight = 1, float base = 0);

// notify vw that you are done with the example.
void finish_example(workspace& all, example& ec);
void finish_example(workspace& all, multi_ex& ec);
void empty_example(workspace& all, example& ec);

void move_feature_namespace(example* dst, example* src, namespace_index c);

void copy_example_metadata(example*, const example*);
void copy_example_data(example*, const example*);  // metadata + features, don't copy the label
void copy_example_data_with_label(example* dst, const example* src);

// after export_example, must call releaseFeatureSpace to free native memory
primitive_feature_space* export_example(workspace& all, example* e, size_t& len);
void releaseFeatureSpace(primitive_feature_space* features, size_t len);

void save_predictor(workspace& all, std::string reg_name);
void save_predictor(workspace& all, io_buf& buf);

// inlines

// First create the hash of a namespace.
inline uint64_t hash_space(workspace& all, const std::string& s)
{
  return all.example_parser->hasher(s.data(), s.length(), all.hash_seed);
}
inline uint64_t hash_space_static(const std::string& s, const std::string& hash)
{
  return getHasher(hash)(s.data(), s.length(), 0);
}
inline uint64_t hash_space_cstr(workspace& all, const char* fstr)
{
  return all.example_parser->hasher(fstr, strlen(fstr), all.hash_seed);
}
// Then use it as the seed for hashing features.
inline uint64_t hash_feature(workspace& all, const std::string& s, uint64_t u)
{
  return all.example_parser->hasher(s.data(), s.length(), u) & all.parse_mask;
}
inline uint64_t hash_feature_static(const std::string& s, uint64_t u, const std::string& h, uint32_t num_bits)
{
  size_t parse_mark = (1 << num_bits) - 1;
  return getHasher(h)(s.data(), s.length(), u) & parse_mark;
}

inline uint64_t hash_feature_cstr(workspace& all, char* fstr, uint64_t u)
{
  return all.example_parser->hasher(fstr, strlen(fstr), u) & all.parse_mask;
}

inline uint64_t chain_hash(workspace& all, const std::string& name, const std::string& value, uint64_t u)
{
  // chain hash is hash(feature_value, hash(feature_name, namespace_hash)) & parse_mask
  return all.example_parser->hasher(
             value.data(), value.length(), all.example_parser->hasher(name.data(), name.length(), u)) &
      all.parse_mask;
}

inline float get_weight(workspace& all, uint32_t index, uint32_t offset)
{
  return (&all.weights[((uint64_t)index) << all.weights.stride_shift()])[offset];
}

inline void set_weight(workspace& all, uint32_t index, uint32_t offset, float value)
{
  (&all.weights[((uint64_t)index) << all.weights.stride_shift()])[offset] = value;
}

inline uint32_t num_weights(workspace& all) { return (uint32_t)all.length(); }

inline uint32_t get_stride(workspace& all) { return all.weights.stride(); }

inline void init_features(primitive_feature_space& fs, size_t features_count)
{
  fs.fs = new feature[features_count];
  fs.len = features_count;
}

inline void set_feature(primitive_feature_space& fs, size_t index, uint64_t feature_hash, float value)
{
  fs.fs[index].weight_index = feature_hash;
  fs.fs[index].x = value;
}
}  // namespace vw

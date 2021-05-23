// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include <string_view>

#include <vector>

struct parser;
struct shared_data;
struct polylabel;
class io_buf;
class reduction_features;

enum class label_type_t
{
  simple,
  cb,       // contextual-bandit
  cb_eval,  // contextual-bandit evaluation
  cs,       // cost-sensitive
  multilabel,
  multiclass,
  ccb,  // conditional contextual-bandit
  slates,
  nolabel,
  continuous  // continuous actions
};

struct example;
struct label_parser
{
  void (*default_label)(polylabel*);
  void (*parse_label)(polylabel*, const std::vector<std::string_view>&, reduction_features&);
  void (*cache_label)(polylabel*, reduction_features&, io_buf& cache);
  size_t (*read_cached_label)(polylabel*, reduction_features&, io_buf& cache);
  float (*get_weight)(const polylabel*, const reduction_features&);
  bool (*test_label)(const polylabel*);
  label_type_t label_type;
};

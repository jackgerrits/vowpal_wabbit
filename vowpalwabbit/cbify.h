// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once
#include "reductions_fwd.h"

class parameters;
struct example;
struct namespace_interactions;

struct cbify_adf_data
{
  multi_ex ecs;
  size_t num_actions;
  size_t increment;
  uint64_t custom_index_mask;

  void init_adf_data(
      const std::size_t num_actions, size_t increment, std::vector<std::vector<namespace_index>>& interactions);
  void copy_example_to_adf(parameters& weights, example& ec);

  ~cbify_adf_data();
};

vw::LEARNER::base_learner* cbify_setup(vw::config::options_i& options, workspace& all);
vw::LEARNER::base_learner* cbifyldf_setup(vw::config::options_i& options, workspace& all);

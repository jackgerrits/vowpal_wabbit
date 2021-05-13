// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once
#include "learner.h"
#include "options.h"

namespace vw
{
namespace continuous_action
{
namespace cats
{
LEARNER::base_learner* setup(config::options_i& options, workspace& all);
struct cats
{
  uint32_t num_actions;
  float bandwidth;
  float min_value;
  float max_value;

  cats(LEARNER::single_learner* p_base);

  void learn(example& ec);
  void predict(example& ec);
  float get_loss(const vw::cb_continuous::continuous_label& cb_cont_costs, float predicted_action) const;

private:
  LEARNER::single_learner* _base = nullptr;
};
}  // namespace cats
}  // namespace continuous_action
}  // namespace vw

// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "cb_explore_adf_common.h"
#include "reductions_fwd.h"
#include <vector>

namespace vw
{
namespace cb_explore_adf
{
namespace first
{
vw::LEARNER::base_learner* setup(vw::config::options_i& options, workspace& all);
}  // namespace first
}  // namespace cb_explore_adf
}  // namespace vw

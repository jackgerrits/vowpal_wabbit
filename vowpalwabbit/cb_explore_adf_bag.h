// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "reductions_fwd.h"

#include <vector>
#include <memory>

namespace vw
{
namespace cb_explore_adf
{
namespace bag
{
vw::LEARNER::base_learner* setup(vw::config::options_i& options, workspace& all);
}  // namespace bag
}  // namespace cb_explore_adf
}  // namespace vw

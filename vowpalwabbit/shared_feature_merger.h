// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#pragma once
#include "reductions_fwd.h"

struct vw;

namespace vw
{
namespace shared_feature_merger
{
vw::LEARNER::base_learner* shared_feature_merger_setup(config::options_i& options, workspace& all);

}  // namespace shared_feature_merger
}  // namespace vw

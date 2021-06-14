#pragma once
// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "reductions_fwd.h"

namespace vw
{
namespace binary
{
vw::LEARNER::base_learner* binary_setup(vw::config::options_i& options, workspace& all);
}
}  // namespace vw

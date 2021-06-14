// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once
#include "learner.h"

namespace vw
{
namespace continuous_action
{
// Setup reduction in stack
LEARNER::base_learner* cb_explore_pdf_setup(config::options_i& options, workspace& all);
}  // namespace continuous_action
}  // namespace vw

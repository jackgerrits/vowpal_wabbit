// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once
#include "learner.h"

namespace vw
{
namespace continuous_action
{
namespace cats_pdf
{
// Setup reduction in stack
LEARNER::base_learner* setup(config::options_i& options, workspace& all);

}  // namespace cats_pdf
}  // namespace continuous_action
}  // namespace vw

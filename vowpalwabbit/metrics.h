#pragma once
// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "reductions_fwd.h"

namespace vw
{
namespace metrics
{
vw::LEARNER::base_learner* metrics_setup(vw::config::options_i& options, workspace& all);
void output_metrics(workspace& all);
}  // namespace metrics
}  // namespace vw

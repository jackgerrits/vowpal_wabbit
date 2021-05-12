// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include "reductions_fwd.h"

namespace CSOAA
{
vw::LEARNER::base_learner* csoaa_setup(vw::config::options_i& options, workspace& all);

vw::LEARNER::base_learner* csldf_setup(vw::config::options_i& options, workspace& all);
struct csoaa;
void finish_example(workspace& all, csoaa&, example& ec);
}  // namespace CSOAA

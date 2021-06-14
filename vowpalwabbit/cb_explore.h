// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include "reductions_fwd.h"
#include "cb.h"

vw::LEARNER::base_learner* cb_explore_setup(vw::config::options_i& options, workspace& all);

namespace CB_EXPLORE
{
void generic_output_example(workspace& all, float loss, example& ec, CB::label& ld);
}
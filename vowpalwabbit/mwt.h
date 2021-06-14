// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include "reductions_fwd.h"

#include "io/io_adapter.h"

vw::LEARNER::base_learner* mwt_setup(vw::config::options_i& options, workspace& all);

namespace MWT
{
void print_scalars(vw::io::writer* f, v_array<float>& scalars, v_array<char>& tag);
}  // namespace MWT

// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "reductions.h"

namespace vw
{
namespace cbzo
{
vw::LEARNER::base_learner* setup(vw::config::options_i& options, workspace& all);

}  // namespace cbzo
}  // namespace vw

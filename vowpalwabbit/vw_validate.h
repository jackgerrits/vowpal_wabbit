// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once
#include <cstdint>

struct workspace;

namespace vw
{
void validate_version(workspace& all);
void validate_min_max_label(workspace& all);
void validate_default_bits(workspace& all, uint32_t local_num_bits);
void validate_num_bits(workspace& all);
}  // namespace vw

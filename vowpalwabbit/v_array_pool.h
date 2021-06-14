// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "v_array.h"
#include "object_pool.h"

namespace vw
{
template <typename T>
using v_array_pool = vw::moved_object_pool<v_array<T>>;
}  // namespace vw
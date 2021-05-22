// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "v_array.h"

#include <cstdint>


namespace CCB {
enum class example_type : uint8_t
{
  unset = 0,
  shared = 1,
  action = 2,
  slot = 3
};

struct reduction_features
{
  example_type type = example_type::unset;
  v_array<uint32_t> explicit_included_actions;
  void clear() { explicit_included_actions.clear(); }
};
}

namespace std
{
inline std::ostream& operator<<(std::ostream& os, CCB::example_type enum_val)
{
  os << static_cast<uint8_t>(enum_val);
  return os;
}

}

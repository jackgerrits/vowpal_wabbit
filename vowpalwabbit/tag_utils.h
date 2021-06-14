#pragma once

#include "vw_string_view.h"

struct example;

namespace vw
{
bool try_extract_random_seed(const example& ex, vw::string_view& view);
}

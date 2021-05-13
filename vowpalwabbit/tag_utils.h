#pragma once

#include <string_view>

struct example;

namespace vw
{
bool try_extract_random_seed(const example& ex, std::string_view& view);
}

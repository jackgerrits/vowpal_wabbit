#pragma once

#include <string_view>

struct example;

namespace VW
{
bool try_extract_random_seed(const example& ex, std::string_view& view);
}

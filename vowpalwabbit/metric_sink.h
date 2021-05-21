#pragma once
// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

namespace vw
{
struct metric_sink
{
  std::vector<std::pair<std::string, size_t>> int_metrics_list;
  std::vector<std::pair<std::string, float>> float_metrics_list;
};
}  // namespace vw

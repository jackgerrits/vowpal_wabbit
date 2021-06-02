// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "debug_log.h"
#include "interactions_predict.h"
#include "v_array.h"
#include "example_predict.h"

#include <concepts>
#include <cstddef>
#include <cstdint>

#undef VW_DEBUG_LOG
#define VW_DEBUG_LOG vw_dbg::gd_predict

template <typename FuncT>
concept ForEachFeatFunc = requires(FuncT func, float feature_value, uint64_t feature_index, float& feature_weight)
{
  {
    func(func, feature_index, feature_weight)
  }
  ->std::convertible_to<void>;
};
namespace GD
{
template <typename WeightsT, typename FuncT>
void foreach_feature_no_inter(WeightsT& weights, const features& fs, uint64_t offset, float mult, FuncT func)
{
  for (const auto& f : fs)
  {
    func(mult * f.value(), f.index() + offset, weights[(f.index() + offset)]);
  }
}

template <typename WeightsT, typename FuncT>
void foreach_feature_no_inter(WeightsT& weights, const features& fs, FuncT func)
{
  foreach_feature_no_inter(weights, fs, 0, 1.f, func);
}

// iterate through all namespaces and quadratic&cubic features, callback function FuncT(some_data_R, feature_value_x,
// WeightOrIndexT) where WeightOrIndexT is EITHER float& feature_weight OR uint64_t feature_index
template <typename WeightsT, typename FuncT>
inline void foreach_feature(WeightsT& weights, const std::vector<std::vector<namespace_index>>& interactions,
    bool permutations, example_predict& ec, size_t& num_interacted_features, FuncT func)
{
  for (features& f : ec) { foreach_feature_no_inter(weights, f, ec.ft_offset, 1.f, func); }

  INTERACTIONS::generate_interactions(weights, interactions, permutations, ec, num_interacted_features, func);
}

template <typename WeightsT, typename FuncT>
inline void foreach_feature(WeightsT& weights, const std::vector<std::vector<namespace_index>>& interactions,
    bool permutations, example_predict& ec, FuncT func)
{
  size_t num_interacted_features_ignored = 0;
  foreach_feature(weights, interactions, permutations, ec, num_interacted_features_ignored, func);
}

template <typename WeightsT>
inline float inline_predict(WeightsT& weights, const std::vector<std::vector<namespace_index>>& interactions,
    bool permutations, example_predict& ec, float initial = 0.f)
{
  auto accumulator = initial;
  foreach_feature(weights, interactions, permutations, ec,
      [&accumulator](
          float feat_value, uint64_t feat_index, float feature_weight) { accumulator += feat_value * feature_weight; });
  return accumulator;
}

template <typename WeightsT>
inline float inline_predict(WeightsT& weights, const std::vector<std::vector<namespace_index>>& interactions,
    bool permutations, example_predict& ec, size_t& num_interacted_features, float initial = 0.f)
{
  auto accumulator = initial;
  foreach_feature(weights, interactions, permutations, ec, num_interacted_features,
      [&accumulator](
          float feat_value, uint64_t feat_index, float feature_weight) { accumulator += feat_value * feature_weight; });
  return accumulator;
}
}  // namespace GD

// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "debug_log.h"
#include "interactions_predict.h"
#include "v_array.h"

#undef VW_DEBUG_LOG
#define VW_DEBUG_LOG vw_dbg::gd_predict

namespace GD
{
// iterate through one namespace (or its part), callback function FuncT(some_data_R, feature_value_x, feature_index)
template <class StateType, void (*FuncT)(StateType&, float, uint64_t), class WeightsType>
void foreach_feature(WeightsType& /*weights*/, const features& fs, StateType& dat, uint64_t offset = 0, float mult = 1.)
{
  for (const auto& f : fs) { FuncT(dat, mult * f.value(), f.index() + offset); }
}

// iterate through one namespace (or its part), callback function FuncT(some_data_R, feature_value_x, feature_weight)
template <class StateType, void (*FuncT)(StateType&, const float, float&), class WeightsType>
inline void foreach_feature(WeightsType& weights, const features& fs, StateType& dat, uint64_t offset = 0, float mult = 1.)
{
  for (const auto& f : fs)
  {
    weight& w = weights[(f.index() + offset)];
    FuncT(dat, mult * f.value(), w);
  }
}

// iterate through one namespace (or its part), callback function FuncT(some_data_R, feature_value_x, feature_weight)
template <class StateType, void (*FuncT)(StateType&, const float, const float&), class WeightsType>
inline void foreach_feature(const WeightsType& weights, const features& fs, StateType& dat, uint64_t offset = 0, float mult = 1.)
{
  for (const auto& f : fs)
  {
    const weight& w = weights[(f.index() + offset)];
    FuncT(dat, mult * f.value(), w);
  }
}

template <class StateType>
inline void dummy_func(StateType&, const audit_strings*)
{
}  // should never be called due to call_audit overload

template <class StateType, class WeightsItemType, void (*FuncT)(StateType&, float, WeightsItemType), class WeightsType>  // nullptr func can't be used as template param in old
                                                               // compilers
inline void generate_interactions(namespace_interactions& interactions, bool permutations, example_predict& ec, StateType& dat,
    WeightsType& weights)  // default value removed to eliminate
                 // ambiguity in old complers
{
  INTERACTIONS::generate_interactions<StateType, WeightsItemType, FuncT, false, dummy_func<StateType>, WeightsType>(interactions, permutations, ec, dat, weights);
}

// iterate through all namespaces and quadratic&cubic features, callback function FuncT(some_data_R, feature_value_x, WeightsItemType)
// where WeightsItemType is EITHER float& feature_weight OR uint64_t feature_index
template <class StateType, class WeightsItemType, void (*FuncT)(StateType&, float, WeightsItemType), class WeightsType>
inline void foreach_feature(WeightsType& weights, bool ignore_some_linear, std::array<bool, NUM_NAMESPACES>& ignore_linear,
    namespace_interactions& interactions, bool permutations, example_predict& ec, StateType& dat)
{
  uint64_t offset = ec.ft_offset;
  if (ignore_some_linear)
    for (example_predict::iterator i = ec.begin(); i != ec.end(); ++i)
    {
      if (!ignore_linear[i.index()])
      {
        features& f = *i;
        foreach_feature<StateType, FuncT, WeightsType>(weights, f, dat, offset);
      }
    }
  else
    for (features& f : ec) foreach_feature<StateType, FuncT, WeightsType>(weights, f, dat, offset);

  generate_interactions<StateType, WeightsItemType, FuncT, WeightsType>(interactions, permutations, ec, dat, weights);
}

inline void vec_add(float& p, const float fx, const float& fw) { p += fw * fx; }

template <class WeightsType>
inline float inline_predict(WeightsType& weights, bool ignore_some_linear, std::array<bool, NUM_NAMESPACES>& ignore_linear,
    namespace_interactions& interactions, bool permutations, example_predict& ec, float initial = 0.f)
{
  foreach_feature<float, const float&, vec_add, WeightsType>(
      weights, ignore_some_linear, ignore_linear, interactions, permutations, ec, initial);
  return initial;
}
}  // namespace GD

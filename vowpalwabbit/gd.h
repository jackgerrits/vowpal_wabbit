// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "parse_regressor.h"
#include "constant.h"
#include "interactions.h"
#include "array_parameters.h"
#include "gd_predict.h"
#include "vw_math.h"

namespace GD
{
VW::LEARNER::base_learner* setup(VW::config::options_i& options, vw& all);

struct gd;

float finalize_prediction(shared_data* sd, vw_logger& logger, float ret);
void print_features(vw& all, example& ec);
void print_audit_features(vw&, example& ec);
void save_load_regressor(vw& all, io_buf& model_file, bool read, bool text);
void save_load_online_state(vw& all, io_buf& model_file, bool read, bool text, double& total_weight,
    GD::gd* g = nullptr, uint32_t ftrl_size = 0);

template <class T>
struct multipredict_info
{
  size_t count;
  size_t step;
  polyprediction* pred;
  const T& weights; /* & for l1: */
  float gravity;
};

template <class T>
inline void vec_add_multipredict(multipredict_info<T>& mp, const float fx, uint64_t fi)
{
  if ((-1e-10 < fx) && (fx < 1e-10)) return;
  uint64_t mask = mp.weights.mask();
  polyprediction* p = mp.pred;
  fi &= mask;
  uint64_t top = fi + (uint64_t)((mp.count - 1) * mp.step);
  uint64_t i = 0;
  if (top <= mask)
  {
    i += fi;
    for (; i <= top; i += mp.step, ++p)
    {
      p->scalar += fx * mp.weights[i];  // TODO: figure out how to use
                                        // weight_parameters::iterator (not using
                                        // change_begin())
    }
  }
  else  // TODO: this could be faster by unrolling into two loops
    for (size_t c = 0; c < mp.count; ++c, fi += (uint64_t)mp.step, ++p)
    {
      fi &= mask;
      p->scalar += fx * mp.weights[fi];
    }
}

// iterate through one namespace (or its part), callback function T(some_data_R, feature_value_x, feature_weight)
template <class StateType, typename FuncT>
inline void foreach_feature(vw& all, features& fs, StateType& dat, uint64_t offset = 0, float mult = 1.)
{
  if (all.weights.sparse)
    foreach_feature(all.weights.sparse_weights, fs, dat, offset, mult);
  else
    foreach_feature(all.weights.dense_weights, fs, dat, offset, mult);
}

template <class StateType, class WeightsItemType, void (*FuncT)(StateType&, float, WeightsItemType)>
inline void foreach_feature(vw& all, example& ec, StateType& dat)
{
  return all.weights.sparse
      ? foreach_feature<StateType, WeightsItemType, FuncT, sparse_parameters>(all.weights.sparse_weights, all.ignore_some_linear,
            all.ignore_linear, *ec.interactions, all.permutations, ec, dat)
      : foreach_feature<StateType, WeightsItemType, FuncT, dense_parameters>(all.weights.dense_weights, all.ignore_some_linear,
            all.ignore_linear,
            *ec.interactions, all.permutations, ec, dat);
}

// iterate through all namespaces and quadratic&cubic features, callback function FuncT(some_data_R, feature_value_x,
// feature_weight)
template <class StateType, void (*FuncT)(StateType&, float, float&)>
inline void foreach_feature(vw& all, example& ec, StateType& dat)
{
  foreach_feature<StateType, float&, FuncT>(all, ec, dat);
}

template <class StateType, void (*FuncT)(StateType&, float, const float&)>
inline void foreach_feature(vw& all, example& ec, StateType& dat)
{
  foreach_feature<StateType, const float&, FuncT>(all, ec, dat);
}

inline float inline_predict(vw& all, example& ec)
{
  const auto& simple_red_features = ec._reduction_features.template get<simple_label_reduction_features>();
  return all.weights.sparse
      ? inline_predict<sparse_parameters>(all.weights.sparse_weights, all.ignore_some_linear, all.ignore_linear,
            *ec.interactions, all.permutations, ec, simple_red_features.initial)
      : inline_predict<dense_parameters>(all.weights.dense_weights, all.ignore_some_linear, all.ignore_linear,
            *ec.interactions, all.permutations, ec, simple_red_features.initial);
}

inline float trunc_weight(const float w, const float gravity)
{
  return (gravity < fabsf(w)) ? w - VW::math::sign(w) * gravity : 0.f;
}

}  // namespace GD

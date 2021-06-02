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
vw::LEARNER::base_learner* setup(vw::config::options_i& options, workspace& all);

struct gd;

float finalize_prediction(shared_data* sd, vw_logger& logger, float ret);
float finalize_prediction(const shared_data* sd, float value);
// void print_features(workspace& all, example& ec);
void print_audit_features(workspace&, example& ec);
void save_load_regressor(workspace& all, io_buf& model_file, bool read, bool text);
void save_load_online_state(workspace& all, io_buf& model_file, bool read, bool text, double& total_weight,
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

// iterate through one namespace (or its part), callback function FuncT(some_data_R, feature_value_x, feature_weight)
template <typename FuncT>
inline void foreach_feature(workspace& all, example& ec, FuncT func)
{
  return all.weights.sparse ? foreach_feature<sparse_parameters, FuncT>(
                                  all.weights.sparse_weights, *ec.interactions, all.permutations, ec, func)
                            : foreach_feature<dense_parameters, FuncT>(
                                  all.weights.dense_weights, *ec.interactions, all.permutations, ec, func);
}

// iterate through one namespace (or its part), callback function FuncT(some_data_R, feature_value_x, feature_weight)
template <typename FuncT>
inline void foreach_feature(workspace& all, example& ec, size_t& num_interacted_features, FuncT func)
{
  return all.weights.sparse
      ? foreach_feature<sparse_parameters, FuncT>(
            all.weights.sparse_weights, *ec.interactions, all.permutations, ec, num_interacted_features, func)
      : foreach_feature<dense_parameters, FuncT>(
            all.weights.dense_weights, *ec.interactions, all.permutations, ec, num_interacted_features, func);
}

inline float inline_predict(workspace& all, example& ec)
{
  const auto& simple_red_features = ec._reduction_features.template get<simple_label_reduction_features>();
  return all.weights.sparse ? inline_predict<sparse_parameters>(all.weights.sparse_weights, *ec.interactions,
                                  all.permutations, ec, simple_red_features.initial)
                            : inline_predict<dense_parameters>(all.weights.dense_weights, *ec.interactions,
                                  all.permutations, ec, simple_red_features.initial);
}

inline float inline_predict(workspace& all, example& ec, size_t& num_generated_features)
{
  const auto& simple_red_features = ec._reduction_features.template get<simple_label_reduction_features>();
  return all.weights.sparse ? inline_predict<sparse_parameters>(all.weights.sparse_weights, *ec.interactions,
                                  all.permutations, ec, num_generated_features, simple_red_features.initial)
                            : inline_predict<dense_parameters>(all.weights.dense_weights, *ec.interactions,
                                  all.permutations, ec, num_generated_features, simple_red_features.initial);
}

}  // namespace GD

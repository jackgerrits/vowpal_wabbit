// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include <cstdint>
#include "constant.h"
#include "feature_group.h"
#include "interactions.h"
#include "example_predict.h"
#include <vector>
#include <string>

template <typename FuncT>
concept ForEachFeatWeightValueFunc = requires(FuncT func, float feature_value, uint64_t feature_index, float feature_weight)
{
  {
    func(feature_value, feature_index, feature_weight)
  }
  ->std::convertible_to<void>;
};

template <typename FuncT>
concept ForEachFeatWeightPtrFunc = requires(FuncT func, float feature_value, uint64_t feature_index, float* feature_weight)
{
  {
    func(feature_value, feature_index, feature_weight)
  }
  ->std::convertible_to<void>;
};

namespace INTERACTIONS
{
/*
 * By default include interactions of feature with itself.
 * This approach produces slightly more interactions but it's safer
 * for some cases, as discussed in issues/698
 * Previous behaviour was: include interactions of feature with itself only if its value != value^2.
 *
 */

// state data used in non-recursive feature generation algorithm
// contains N feature_gen_data records (where N is length of interaction)
struct feature_gen_data
{
  size_t loop_idx;  // current feature id in namespace
  uint64_t hash;    // hash of feature interactions of previous namespaces in the list
  float x;          // value of feature interactions of previous namespaces in the list
  size_t loop_end;  // last feature id. May be less than number of features if namespace involved in interaction more
                    // than once calculated at preprocessing together with same_ns
  size_t self_interaction;  // namespace interacting with itself
  features* ft_arr;
  //    feature_gen_data(): loop_idx(0), x(1.), loop_end(0), self_interaction(false) {}
};

// The inline function below may be adjusted to change the way
// synthetic (interaction) features' values are calculated, e.g.,
// fabs(value1-value2) or even value1>value2?1.0:-1.0
// Beware - its result must be non-zero.
inline float INTERACTION_VALUE(float value1, float value2) { return value1 * value2; }

// uncomment line below to disable usage of inner 'for' loops for pair and triple interactions
// end switch to usage of non-recursive feature generation algorithm for interactions of any length

// #define GEN_INTER_LOOP

inline void inner_kernel(auto& weights, features::const_iterator& begin, features::const_iterator& end,
    uint64_t offset, feature_value ft_value, feature_index halfhash, ForEachFeatWeightValueFunc auto func)
{
  for (; begin != end; ++begin)
  {
    const auto interacted_value = INTERACTION_VALUE(ft_value, begin.value());
    const auto interacted_index = (begin.index() ^ halfhash) + offset;
    func(interacted_value, interacted_index, weights[interacted_index]);
  }
}


inline void inner_kernel(auto& weights, features::const_iterator& begin, features::const_iterator& end,
    uint64_t offset, feature_value ft_value, feature_index halfhash, ForEachFeatWeightPtrFunc auto func)
{
  for (; begin != end; ++begin)
  {
    const auto interacted_value = INTERACTION_VALUE(ft_value, begin.value());
    const auto interacted_index = (begin.index() ^ halfhash) + offset;
    func(interacted_value, interacted_index, &weights[interacted_index]);
  }
}

// this templated function generates new features for given example and set of interactions
// and passes each of them to given function FuncT()
template <typename WeightsT, typename FuncT>
inline void generate_interactions(WeightsT& weights, const std::vector<std::vector<namespace_index>>& interactions,
    bool permutations, example_predict& ec, size_t& num_features, FuncT func)
{
  num_features = 0;
  features* features_data = ec.feature_space.data();

  // often used values
  const uint64_t offset = ec.ft_offset;
  //    const uint64_t stride_shift = all.stride_shift; // it seems we don't need stride shift in FTRL-like hash

  // statedata for generic non-recursive iteration
  v_array<feature_gen_data> state_data;

  feature_gen_data empty_ns_data;  // micro-optimization. don't want to call its constructor each time in loop.
  empty_ns_data.loop_idx = 0;
  empty_ns_data.x = 1.;
  empty_ns_data.loop_end = 0;
  empty_ns_data.self_interaction = false;

  for (const auto& ns : interactions)
  {  // current list of namespaces to interact.

#ifndef GEN_INTER_LOOP

    // unless GEN_INTER_LOOP is defined we use nested 'for' loops for interactions length 2 (pairs) and 3 (triples)
    // and generic non-recursive algorythm for all other cases.
    // nested 'for' loops approach is faster, but can't be used for interation of any length.
    const size_t len = ns.size();

    if (len == 2)  // special case of pairs
    {
      const features& first = features_data[ns[0]];
      if (first.nonempty())
      {
        const features& second = features_data[ns[1]];
        if (second.nonempty())
        {
          const bool same_namespace = (!permutations && (ns[0] == ns[1]));

          for (size_t i = 0; i < first.indicies.size(); ++i)
          {
            feature_index halfhash = FNV_prime * static_cast<uint64_t>(first.indicies[i]);
            // next index differs for permutations and simple combinations
            feature_value ft_value = first.values[i];
            auto begin = second.cbegin();
            if (same_namespace) { begin += (PROCESS_SELF_INTERACTIONS(ft_value)) ? i : i + 1; }
            auto end = second.cend();
            num_features += std::distance(begin, end);
            inner_kernel(weights, begin, end, offset, ft_value, halfhash, func);
          }  // end for(fst)
        }    // end if (data[snd] size > 0)
      }      // end if (data[fst] size > 0)
    }
    else if (len == 3)  // special case for triples
    {
      features& first = features_data[ns[0]];
      if (first.nonempty())
      {
        features& second = features_data[ns[1]];
        if (second.nonempty())
        {
          features& third = features_data[ns[2]];
          if (third.nonempty())
          {  // don't compare 1 and 3 as interaction is sorted
            const bool same_namespace1 = (!permutations && (ns[0] == ns[1]));
            const bool same_namespace2 = (!permutations && (ns[1] == ns[2]));

            for (size_t i = 0; i < first.indicies.size(); ++i)
            {
              const uint64_t halfhash1 = FNV_prime * static_cast<uint64_t>(first.indicies[i]);
              float first_ft_value = first.values[i];
              size_t j = 0;
              if (same_namespace1)  // next index differs for permutations and simple combinations
                j = (PROCESS_SELF_INTERACTIONS(first_ft_value)) ? i : i + 1;

              for (; j < second.indicies.size(); ++j)
              {  // f3 x k*(f2 x k*f1)
                feature_index halfhash = FNV_prime * (halfhash1 ^ static_cast<uint64_t>(second.indicies[j]));
                feature_value ft_value = INTERACTION_VALUE(first_ft_value, second.values[j]);

                auto begin = third.cbegin();
                // next index differs for permutations and simple combinations
                if (same_namespace2) { begin += (PROCESS_SELF_INTERACTIONS(ft_value)) ? j : j + 1; }
                auto end = third.cend();
                num_features += std::distance(begin, end);
                inner_kernel(weights, begin, end, offset, ft_value, halfhash, func);
              }  // end for (snd)
            }    // end for (fst)

          }  // end if (data[thr] size > 0)
        }    // end if (data[snd] size > 0)
      }      // end if (data[fst] size > 0)
    }
    else  // generic case: quatriples, etc.

#endif
    {
      bool must_skip_interaction = false;
      // preparing state data
      feature_gen_data* fgd = state_data.begin();
      feature_gen_data* fgd2;  // for further use
      for (auto n : ns)
      {
        features& ft = features_data[static_cast<int32_t>(n)];
        const size_t ft_cnt = ft.indicies.size();

        if (ft_cnt == 0)
        {
          must_skip_interaction = true;
          break;
        }

        if (fgd == state_data.end())
        {
          state_data.push_back(empty_ns_data);
          fgd = state_data.end() - 1;  // reassign as memory could be realloced
        }

        fgd->loop_end = ft_cnt - 1;  // saving number of features for each namespace
        fgd->ft_arr = &ft;
        ++fgd;
      }

      // if any of interacting namespace has 0 features - whole interaction is skipped
      if (must_skip_interaction) continue;  // no_data_to_interact

      if (!permutations)  // adjust state_data for simple combinations
      {                   // if permutations mode is disabeled then namespaces in ns are already sorted and thus grouped
        // (in fact, currently they are sorted even for enabled permutations mode)
        // let's go throw the list and calculate number of features to skip in namespaces which
        // repeated more than once to generate only simple combinations of features

        size_t margin = 0;  // number of features to ignore if namespace has been seen before

        // iterate list backward as margin grows in this order

        for (fgd = state_data.end() - 1; fgd > state_data.begin(); --fgd)
        {
          fgd2 = fgd - 1;
          fgd->self_interaction = (fgd->ft_arr == fgd2->ft_arr);  // state_data.begin().self_interaction is always false
          if (fgd->self_interaction)
          {
            size_t& loop_end = fgd2->loop_end;

            if (!PROCESS_SELF_INTERACTIONS((*fgd2->ft_arr).values[loop_end - margin]))
            {
              ++margin;  // otherwise margin can't be increased
              must_skip_interaction = loop_end < margin;
              if (must_skip_interaction) break;
            }

            if (margin != 0) loop_end -= margin;  // skip some features and increase margin
          }
          else if (margin != 0)
            margin = 0;
        }

        // if impossible_without_permutations == true then we faced with case like interaction 'aaaa'
        // where namespace 'a' contains less than 4 unique features. It's impossible to make simple
        // combination of length 4 without repetitions from 3 or less elements.
        if (must_skip_interaction) continue;  // impossible_without_permutations
      }                                       // end of state_data adjustment

      fgd = state_data.begin();     // always equal to first ns
      fgd2 = state_data.end() - 1;  // always equal to last ns
      fgd->loop_idx = 0;            // loop_idx contains current feature id for curently processed namespace.

      // beware: micro-optimization.
      /* start & end are always point to features in last namespace of interaction.
      for 'all.permutations == true' they are constant.*/
      size_t start_i = 0;

      feature_gen_data* cur_data = fgd;
      // end of micro-optimization block

      // generic feature generation cycle for interactions of any length
      bool do_it = true;
      while (do_it)
      {
        if (cur_data < fgd2)  // can go further threw the list of namespaces in interaction
        {
          feature_gen_data* next_data = cur_data + 1;
          size_t feature = cur_data->loop_idx;
          features& fs = *(cur_data->ft_arr);

          if (next_data->self_interaction)
          {  // if next namespace is same, we should start with loop_idx + 1 to avoid feature interaction with itself
            // unless feature has value x and x != x*x. E.g. x != 0 and x != 1. Features with x == 0 are already
            // filtered out in parce_args.cc::maybeFeature().

            next_data->loop_idx =
                (PROCESS_SELF_INTERACTIONS(fs.values[feature])) ? cur_data->loop_idx : cur_data->loop_idx + 1;
          }
          else
            next_data->loop_idx = 0;

          if (cur_data == fgd)  // first namespace
          {
            next_data->hash = FNV_prime * static_cast<uint64_t>(fs.indicies[feature]);
            next_data->x = fs.values[feature];  // data->x == 1.
          }
          else
          {  // feature2 xor (16777619*feature1)
            next_data->hash = FNV_prime * (cur_data->hash ^ static_cast<uint64_t>(fs.indicies[feature]));
            next_data->x = INTERACTION_VALUE(fs.values[feature], cur_data->x);
          }

          ++cur_data;
        }
        else
        {
          // last namespace - iterate its features and go back
          // start value is not a constant in this case
          if (!permutations) { start_i = fgd2->loop_idx; }

          features& fs = *(fgd2->ft_arr);

          feature_value ft_value = fgd2->x;
          feature_index halfhash = fgd2->hash;

          auto begin = fs.cbegin() + start_i;
          auto end = fs.cbegin() + (fgd2->loop_end + 1);
          num_features += std::distance(begin, end);
          inner_kernel(weights, begin, end, offset, ft_value, halfhash, func);

          // trying to go back increasing loop_idx of each namespace by the way

          bool go_further = true;

          do
          {
            --cur_data;
            go_further = (++cur_data->loop_idx > cur_data->loop_end);  // increment loop_idx
          } while (go_further && cur_data != fgd);

          do_it = !(cur_data == fgd && go_further);
          // if do_it==false - we've reached 0 namespace but its 'cur_data.loop_idx > cur_data.loop_end' -> exit the
          // while loop
        }  // if last namespace
      }    // while do_it
    }
  }  // foreach interaction in all.interactions
}

template <typename FuncT>
inline void generate_interactions(workspace& all, example_predict& ec, size_t& num_features, FuncT func)
{
  return all.weights.sparse ? INTERACTIONS::generate_interactions(all.weights.sparse_weights, *ec.interactions,
                                  all.permutations, ec, num_features, func)
                            : INTERACTIONS::generate_interactions(all.weights.dense_weights, *ec.interactions,
                                  all.permutations, ec, num_features, func);
}

}  // namespace INTERACTIONS

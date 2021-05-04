// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "get_pmf.h"
#include "err_constants.h"
#include "debug_log.h"
#include "parse_args.h"
#include "guard.h"

// Aliases
using std::endl;
using VW::cb_continuous::continuous_label;
using VW::cb_continuous::continuous_label_elm;
using VW::config::make_option;
using VW::config::option_group_definition;
using VW::config::options_i;
using VW::LEARNER::single_learner;

// Enable/Disable indented debug statements
#undef VW_DEBUG_LOG
#define VW_DEBUG_LOG vw_dbg::cb_explore_get_pmf

namespace VW
{
namespace continuous_action
{
////////////////////////////////////////////////////
// BEGIN sample_pdf reduction and reduction methods
struct get_pmf
{
  void learn(example& ec);
  void predict(example& ec);

  void init(single_learner* p_base, float epsilon);

private:
  single_learner* _base = nullptr;
  float _epsilon;
};

void get_pmf::learn(example& ec)
{
  _base->learn(ec);
}

void get_pmf::predict(example& ec)
{
  uint32_t base_prediction;

  {  // predict & restore prediction
    auto restore = VW::stash_guard(ec.pred);
    _base->predict(ec);
    base_prediction = ec.pred.multiclass - 1;
  }

  // Assume ec.pred.a_s allocated by the caller (probably pmf_to_pdf);
  ec.pred.a_s.clear();
  ec.pred.a_s.push_back({base_prediction, 1.0f});
}

void get_pmf::init(single_learner* p_base, float epsilon)
{
  _base = p_base;
  _epsilon = epsilon;
}

// Free function to tie function pointers to reduction class methods
template <bool is_learn>
void predict_or_learn(get_pmf& reduction, single_learner&, example& ec)
{
  if (is_learn)
    reduction.learn(ec);
  else
    reduction.predict(ec);
}

// END sample_pdf reduction and reduction methods
////////////////////////////////////////////////////

// Setup reduction in stack
LEARNER::base_learner* get_pmf_setup(config::options_i& options, vw& all)
{
  option_group_definition new_options("Continuous actions - convert to pmf");
  bool invoked = false;
  float epsilon = 0.0f;
  new_options.add(
      make_option("get_pmf", invoked).keep().necessary().help("Convert a single multiclass prediction to a pmf"));

  // If reduction was not invoked, don't add anything
  // to the reduction stack;
  if (!options.add_parse_and_check_necessary(new_options)) return nullptr;

  LEARNER::base_learner* p_base = setup_base(options, all);
  auto p_reduction = scoped_calloc_or_throw<get_pmf>();
  p_reduction->init(as_singleline(p_base), epsilon);

  LEARNER::learner<get_pmf, example>& l = init_learner(p_reduction, as_singleline(p_base), predict_or_learn<true>,
      predict_or_learn<false>, 1, prediction_type_t::pdf, all.get_setupfn_name(get_pmf_setup));

  return make_base(l);
}
}  // namespace continuous_action
}  // namespace VW

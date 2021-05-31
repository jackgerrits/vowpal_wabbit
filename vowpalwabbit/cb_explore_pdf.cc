// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "cb_explore_pdf.h"
#include "error_constants.h"
#include "debug_log.h"
#include "parse_args.h"

// Aliases
using std::endl;
using vw::config::make_option;
using vw::config::option_group_definition;
using vw::config::options_i;
using vw::LEARNER::single_learner;

// Enable/Disable indented debug statements
#undef VW_DEBUG_LOG
#define VW_DEBUG_LOG vw_dbg::cb_explore_pdf

namespace vw
{
namespace continuous_action
{
////////////////////////////////////////////////////
// BEGIN sample_pdf reduction and reduction methods
struct cb_explore_pdf
{
  void learn(example& ec);
  void predict(example& ec);

  void init(single_learner* p_base);

  float epsilon;
  float min_value;
  float max_value;
  bool first_only;

private:
  single_learner* _base = nullptr;
};

void cb_explore_pdf::learn(example& ec) { _base->learn(ec); }

void cb_explore_pdf::predict(example& ec)
{
  const auto& reduction_features = ec._reduction_features.template get<vw::continuous_actions::reduction_features>();
  if (first_only && !reduction_features.is_pdf_set() && !reduction_features.is_chosen_action_set())
  {
    // uniform random
    ec.pred.pdf.push_back(
        vw::continuous_actions::pdf_segment{min_value, max_value, static_cast<float>(1. / (max_value - min_value))});
    return;
  }
  else if (first_only && reduction_features.is_pdf_set())
  {
    // pdf provided
    ec.pred.pdf = reduction_features.pdf;
    return;
  }

  _base->predict(ec);

  continuous_actions::probability_density_function& _pred_pdf = ec.pred.pdf;
  for (uint32_t i = 0; i < _pred_pdf.size(); i++)
  { _pred_pdf[i].pdf_value = _pred_pdf[i].pdf_value * (1 - epsilon) + epsilon / (max_value - min_value); }
}

void cb_explore_pdf::init(single_learner* p_base) { _base = p_base; }

// Free function to tie function pointers to reduction class methods
template <bool is_learn>
void predict_or_learn(cb_explore_pdf& reduction, single_learner&, example& ec)
{
  if (is_learn)
    reduction.learn(ec);
  else
    reduction.predict(ec);
}

// END sample_pdf reduction and reduction methods
////////////////////////////////////////////////////

// Setup reduction in stack
LEARNER::base_learner* cb_explore_pdf_setup(config::options_i& options, workspace& all)
{
  option_group_definition new_options("Continuous actions - cb_explore_pdf");
  bool invoked = false;
  float epsilon;
  float min;
  float max;
  bool first_only = false;
  new_options
      .add(make_option("cb_explore_pdf", invoked)
               .keep()
               .necessary()
               .help("Sample a pdf and pick a continuous valued action"))
      .add(make_option("epsilon", epsilon)
               .keep()
               .allow_override()
               .default_value(0.05f)
               .help("epsilon-greedy exploration"))
      .add(make_option("min_value", min).keep().default_value(0.0f).help("min value for continuous range"))
      .add(make_option("max_value", max).keep().default_value(1.0f).help("max value for continuous range"))
      .add(make_option("first_only", first_only)
               .keep()
               .help("Use user provided first action or user provided pdf or uniform random"));

  // If reduction was not invoked, don't add anything
  // to the reduction stack;
  if (!options.add_parse_and_check_necessary(new_options)) return nullptr;

  if (!options.was_supplied("min_value") || !options.was_supplied("max_value"))
    throw vw::error(vw::error_code::unknown, "error: min and max values must be supplied with cb_explore_pdf");

  LEARNER::base_learner* p_base = setup_base(options, all);
  auto p_reduction = scoped_calloc_or_throw<cb_explore_pdf>();
  p_reduction->init(as_singleline(p_base));
  p_reduction->epsilon = epsilon;
  p_reduction->min_value = min;
  p_reduction->max_value = max;
  p_reduction->first_only = first_only;

  LEARNER::learner<cb_explore_pdf, example>& l =
      init_learner(p_reduction, as_singleline(p_base), predict_or_learn<true>, predict_or_learn<false>, 1,
          prediction_type_t::pdf, all.get_setupfn_name(cb_explore_pdf_setup));

  return make_base(l);
}
}  // namespace continuous_action
}  // namespace vw

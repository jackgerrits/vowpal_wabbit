// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#include <cfloat>
#include <sstream>
#include <queue>
#include <utility>

#include "topk.h"
#include "learner.h"
#include "parse_args.h"
#include "vw.h"
#include "shared_data.h"

#include "io/logger.h"

using namespace vw::config;
namespace logger = vw::io::logger;

namespace vw
{
class topk
{
  using container_t = std::multimap<float, v_array<char>>;

public:
  using const_iterator_t = container_t::const_iterator;
  topk(uint32_t k_num);

  void predict(vw::LEARNER::single_learner& base, multi_ex& ec_seq);
  void learn(vw::LEARNER::single_learner& base, multi_ex& ec_seq);
  std::pair<const_iterator_t, const_iterator_t> get_container_view();
  void clear_container();

private:
  void update_priority_queue(float pred, v_array<char>& tag);

  const uint32_t _k_num;
  container_t _pr_queue;
};
}  // namespace vw

vw::topk::topk(uint32_t k_num) : _k_num(k_num) {}

void vw::topk::predict(vw::LEARNER::single_learner& base, multi_ex& ec_seq)
{
  for (auto ec : ec_seq)
  {
    base.predict(*ec);
    update_priority_queue(ec->pred.scalar, ec->tag);
  }
}

void vw::topk::learn(vw::LEARNER::single_learner& base, multi_ex& ec_seq)
{
  for (auto ec : ec_seq)
  {
    base.learn(*ec);
    update_priority_queue(ec->pred.scalar, ec->tag);
  }
}

void vw::topk::update_priority_queue(float pred, v_array<char>& tag)
{
  if (_pr_queue.size() < _k_num) { _pr_queue.insert({pred, tag}); }
  else if (_pr_queue.begin()->first < pred)
  {
    _pr_queue.erase(_pr_queue.begin());
    _pr_queue.insert({pred, tag});
  }
}

std::pair<vw::topk::const_iterator_t, vw::topk::const_iterator_t> vw::topk::get_container_view()
{
  return {_pr_queue.cbegin(), _pr_queue.cend()};
}

void vw::topk::clear_container() { _pr_queue.clear(); }

void print_result(
    vw::io::writer* file_descriptor, std::pair<vw::topk::const_iterator_t, vw::topk::const_iterator_t> const& view)
{
  if (file_descriptor != nullptr)
  {
    std::stringstream ss;
    for (auto it = view.first; it != view.second; it++)
    {
      ss << std::fixed << it->first << " ";
      print_tag_by_ref(ss, it->second);
      ss << " \n";
    }
    ss << '\n';
    ssize_t len = ss.str().size();
    auto t = file_descriptor->write(ss.str().c_str(), len);
    if (t != len) logger::errlog_error("write error: {}", vw::strerror_to_string(errno));
  }
}

void output_example(workspace& all, example& ec)
{
  label_data& ld = ec.l.simple;

  all.sd->update(ec.test_only, ld.label != FLT_MAX, ec.loss, ec.weight, ec.num_features);
  if (ld.label != FLT_MAX) all.sd->weighted_labels += (static_cast<double>(ld.label)) * ec.weight;

  print_update(all, ec);
}

template <bool is_learn>
void predict_or_learn(vw::topk& d, vw::LEARNER::single_learner& base, multi_ex& ec_seq)
{
  if (is_learn)
    d.learn(base, ec_seq);
  else
    d.predict(base, ec_seq);
}

void finish_example(workspace& all, vw::topk& d, multi_ex& ec_seq)
{
  for (auto ec : ec_seq) output_example(all, *ec);
  for (auto& sink : all.final_prediction_sink) print_result(sink.get(), d.get_container_view());
  d.clear_container();
  vw::finish_example(all, ec_seq);
}

vw::LEARNER::base_learner* topk_setup(options_i& options, workspace& all)
{
  uint32_t K;
  option_group_definition new_options("Top K");
  new_options.add(make_option("top", K).keep().necessary().help("top k recommendation"));

  if (!options.add_parse_and_check_necessary(new_options)) return nullptr;

  auto data = scoped_calloc_or_throw<vw::topk>(K);

  vw::LEARNER::learner<vw::topk, multi_ex>& l = init_learner(data, as_singleline(setup_base(options, all)),
      predict_or_learn<true>, predict_or_learn<false>, all.get_setupfn_name(topk_setup), true);
  l.set_finish_example(finish_example);

  return make_base(l);
}

// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include <cfloat>
#include <cmath>
#include "gd.h"
#include "vw.h"
#include "vw_exception.h"
#include <string_view>
#include "example.h"
#include "parse_primitives.h"
#include "shared_data.h"

#include "io/logger.h"

namespace logger = vw::io::logger;

namespace COST_SENSITIVE
{
std::vector<std::string_view> name_value(std::string_view s, float& v)
{
  auto name = tokenize(':', s);

  switch (name.size())
  {
    case 0:
    case 1:
      v = 1.;
      break;
    case 2:
      v = float_of_string(name[1]);
      if (std::isnan(v)) THROW("error NaN value for: " << name[0]);
      break;
    default:
      logger::errlog_error("example with a wierd name. What is '{}'?", s);
  }
  return name;
}

char* bufread_label(label& ld, char* c, io_buf& cache)
{
  size_t num = *reinterpret_cast<size_t*>(c);
  ld.costs.clear();
  c += sizeof(size_t);
  size_t total = sizeof(wclass) * num;
  if (cache.buf_read(c, static_cast<int>(total)) < total) { THROW("error in demarshal of cost data"); }
  for (size_t i = 0; i < num; i++)
  {
    wclass temp = *reinterpret_cast<wclass*>(c);
    c += sizeof(wclass);
    ld.costs.push_back(temp);
  }

  return c;
}

size_t read_cached_label(label& ld, io_buf& cache)
{
  ld.costs.clear();
  char* c;
  size_t total = sizeof(size_t);
  if (cache.buf_read(c, static_cast<int>(total)) < total) return 0;
  bufread_label(ld, c, cache);

  return total;
}

char* bufcache_label(label& ld, char* c)
{
  *reinterpret_cast<size_t*>(c) = ld.costs.size();
  c += sizeof(size_t);
  for (unsigned int i = 0; i < ld.costs.size(); i++)
  {
    *reinterpret_cast<wclass*>(c) = ld.costs[i];
    c += sizeof(wclass);
  }
  return c;
}

void cache_label(label& ld, io_buf& cache)
{
  char* c;
  cache.buf_write(c, sizeof(size_t) + sizeof(wclass) * ld.costs.size());
  bufcache_label(ld, c);
}

void default_label(label& ld) { ld.costs.clear(); }

bool test_label_internal(const label& ld)
{
  if (ld.costs.size() == 0) return true;
  for (unsigned int i = 0; i < ld.costs.size(); i++)
    if (FLT_MAX != ld.costs[i].x) return false;
  return true;
}

bool test_label(const label& ld) { return test_label_internal(ld); }

void parse_label(label& ld, const std::vector<std::string_view>& words, reduction_features&)
{
  ld.costs.clear();

  // handle shared and label first
  if (words.size() == 1)
  {
    float fx;
    const auto tokenized = name_value(words[0], fx);
    bool eq_shared = tokenized[0] == "***shared***" || tokenized[0] == "shared";
    bool eq_label = tokenized[0] == "***label***" || tokenized[0] == "label";

    if (eq_shared || eq_label)
    {
      if (eq_shared)
      {
        if (tokenized.size() != 1)
          logger::errlog_error("shared feature vectors should not have costs on: {}", words[0]);
        else
        {
          wclass f = {-FLT_MAX, 0, 0., 0.};
          ld.costs.push_back(f);
        }
      }
      if (eq_label)
      {
        if (tokenized.size() != 2)
          logger::errlog_error("label feature vectors should have exactly one cost on: {}", words[0]);
        else
        {
          wclass f = {float_of_string(tokenized[1]), 0, 0., 0.};
          ld.costs.push_back(f);
        }
      }
      return;
    }
  }

  // otherwise this is a "real" example
  for (unsigned int i = 0; i < words.size(); i++)
  {
    wclass f = {0., 0, 0., 0.};
    const auto tokenized = name_value(words[i], f.x);

    if (tokenized.size() == 0) THROW(" invalid cost: specification -- no names on: " << words[i]);

    if (tokenized.size() == 1 || tokenized.size() == 2 || tokenized.size() == 3)
    {
      f.class_index = static_cast<uint32_t>(hashstring(tokenized[0].data(), tokenized[0].length(), 0));
      if (tokenized.size() == 1 && f.x >= 0)  // test examples are specified just by un-valued class #s
        f.x = FLT_MAX;
    }
    else
      THROW("malformed cost specification on '" << (tokenized[0]) << "'");

    ld.costs.push_back(f);
  }
}

// clang-format off
label_parser cs_label = {
  // default_label
  [](polylabel* v) { default_label(v->cs); },
  // parse_label
  [](polylabel* v, const std::vector<std::string_view>& words, reduction_features& red_features) {
    parse_label(v->cs, words, red_features);
  },
  // cache_label
  [](polylabel* v, reduction_features&, io_buf& cache) { cache_label(v->cs, cache); },
  // read_cached_label
  [](polylabel* v, reduction_features&, io_buf& cache) { return read_cached_label(v->cs, cache); },
  // get_weight
  [](const polylabel*, const reduction_features&) { return 1.f; },
  // test_label
  [](const polylabel* v) { return test_label(v->cs); },
  label_type_t::cs
};
// clang-format on

void print_update(workspace& all, bool is_test, example& ec, multi_ex* ec_seq, bool action_scores, uint32_t prediction)
{
  if (all.sd->weighted_examples() >= all.sd->dump_interval && !all.logger.quiet && !all.bfgs)
  {
    size_t num_current_features = ec.get_num_features();
    // for csoaa_ldf we want features from the whole (multiline example),
    // not only from one line (the first one) represented by ec
    if (ec_seq != nullptr)
    {
      num_current_features = 0;
      // TODO: including quadratic and cubic.
      for (auto& ecc : *ec_seq) num_current_features += ecc->get_num_features();
    }

    std::string label_buf;
    if (is_test)
      label_buf = " unknown";
    else
      label_buf = " known";

    if (action_scores)
    {
      std::ostringstream pred_buf;

      pred_buf << std::setw(all.sd->col_current_predict) << std::right << std::setfill(' ');
      pred_buf << ec.pred.a_s[0].action;
      if (action_scores) pred_buf << ".....";
      all.sd->print_update(*all.trace_message, all.holdout_set_off, all.current_pass, label_buf, pred_buf.str(),
          num_current_features, all.progress_add, all.progress_arg);
      ;
    }
    else
      all.sd->print_update(*all.trace_message, all.holdout_set_off, all.current_pass, label_buf, prediction,
          num_current_features, all.progress_add, all.progress_arg);
  }
}

void output_example(workspace& all, example& ec, const COST_SENSITIVE::label& cs_label, uint32_t multiclass_prediction)
{
  float loss = 0.;
  if (!test_label(cs_label))
  {
    // need to compute exact loss
    size_t pred = static_cast<size_t>(multiclass_prediction);

    float chosen_loss = FLT_MAX;
    float min = FLT_MAX;
    for (const auto& cl : cs_label.costs)
    {
      if (cl.class_index == pred) chosen_loss = cl.x;
      if (cl.x < min) min = cl.x;
    }
    if (chosen_loss == FLT_MAX)
      logger::errlog_warn("csoaa predicted an invalid class. Are all multi-class labels in the {1..k} range?");

    loss = (chosen_loss - min) * ec.weight;
    // TODO(alberto): add option somewhere to allow using absolute loss instead?
    // loss = chosen_loss;
  }

  all.sd->update(ec.test_only, !test_label(cs_label), loss, ec.weight, ec.get_num_features());

  for (auto& sink : all.final_prediction_sink)
  { all.print_by_ref(sink.get(), static_cast<float>(multiclass_prediction), 0, ec.tag); }

  if (all.raw_prediction != nullptr)
  {
    std::stringstream outputStringStream;
    for (unsigned int i = 0; i < cs_label.costs.size(); i++)
    {
      wclass cl = cs_label.costs[i];
      if (i > 0) outputStringStream << ' ';
      outputStringStream << cl.class_index << ':' << cl.partial_prediction;
    }
    all.print_text_by_ref(all.raw_prediction.get(), outputStringStream.str(), ec.tag);
  }

  print_update(all, test_label(cs_label), ec, nullptr, false, multiclass_prediction);
}

void output_example(workspace& all, example& ec) { output_example(all, ec, ec.l.cs, ec.pred.multiclass); }

void finish_example(workspace& all, example& ec)
{
  output_example(all, ec, ec.l.cs, ec.pred.multiclass);
  vw::finish_example(all, ec);
}

bool example_is_test(example& ec)
{
  const auto& costs = ec.l.cs.costs;
  if (costs.size() == 0) return true;
  for (size_t j = 0; j < costs.size(); j++)
    if (costs[j].x != FLT_MAX) return false;
  return true;
}

bool ec_is_example_header(example const& ec)  // example headers look like "shared"
{
  const auto& costs = ec.l.cs.costs;
  if (costs.size() != 1) return false;
  if (costs[0].class_index != 0) return false;
  if (costs[0].x != -FLT_MAX) return false;
  return true;
}
}  // namespace COST_SENSITIVE

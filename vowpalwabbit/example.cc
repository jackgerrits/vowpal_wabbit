// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#include <cstdint>
#include <algorithm>

#include "example.h"
#include "gd.h"
#include "simple_label_parser.h"
#include "interactions.h"

float calculate_total_sum_features_squared(bool permutations, example& ec)
{
  float sum_features_squared = 0.f;
  for (const features& fs : ec) { sum_features_squared += fs.sum_feat_sq; }

  size_t ignored_interacted_feature_count = 0;
  float calculated_sum_features_squared = 0.f;
  INTERACTIONS::eval_count_of_generated_ft(permutations, *ec.interactions, ec.feature_space,
      ignored_interacted_feature_count, calculated_sum_features_squared);
  sum_features_squared += calculated_sum_features_squared;
  return sum_features_squared;
}

VW_WARNING_STATE_PUSH
VW_WARNING_DISABLE_DEPRECATED_USAGE
example::~example()
{
  if (passthrough)
  {
    delete passthrough;
    passthrough = nullptr;
  }
}
VW_WARNING_STATE_POP

float collision_cleanup(features& fs)
{
  uint64_t last_index = static_cast<uint64_t>(-1);
  float sum_sq = 0.f;
  features::iterator pos = fs.begin();
  for (features::iterator& f : fs)
  {
    if (last_index == f.index())
      pos.value() += f.value();
    else
    {
      sum_sq += pos.value() * pos.value();
      ++pos;
      pos.value() = f.value();
      pos.index() = f.index();
      last_index = f.index();
    }
  }

  sum_sq += pos.value() * pos.value();
  fs.sum_feat_sq = sum_sq;
  ++pos;
  fs.truncate_to(pos);

  return sum_sq;
}

namespace vw
{
void copy_example_label(example* dst, example* src, void (*)(polylabel*, polylabel*))
{
  dst->l = src->l;
  dst->_reduction_features = src->_reduction_features;
}

void copy_example_label(example* dst, const example* src) { dst->l = src->l; }

void copy_example_metadata(example* dst, const example* src)
{
  dst->tag = src->tag;
  dst->example_counter = src->example_counter;

  dst->ft_offset = src->ft_offset;

  dst->partial_prediction = src->partial_prediction;
  if (src->passthrough == nullptr)
    dst->passthrough = nullptr;
  else
  {
    dst->passthrough = new features;
    dst->passthrough->deep_copy_from(*src->passthrough);
  }
  dst->loss = src->loss;
  dst->weight = src->weight;
  dst->confidence = src->confidence;
  dst->test_only = src->test_only;
  dst->end_pass = src->end_pass;
  dst->is_newline = src->is_newline;
  dst->sorted = src->sorted;
}

void copy_example_data(example* dst, const example* src)
{
  copy_example_metadata(dst, src);

  // copy feature data
  dst->indices = src->indices;
  for (namespace_index c : src->indices) dst->feature_space[c].deep_copy_from(src->feature_space[c]);
  dst->num_features = src->num_features;
  dst->total_sum_feat_sq = src->total_sum_feat_sq;
  dst->total_sum_feat_sq_calculated = src->total_sum_feat_sq_calculated;
  dst->use_permutations = src->use_permutations;
  dst->interactions = src->interactions;
  dst->_debug_current_reduction_depth = src->_debug_current_reduction_depth;
}

void copy_example_data_with_label(example* dst, const example* src)
{
  copy_example_data(dst, src);
  copy_example_label(dst, src);
}

}  // namespace vw

std::string cb_label_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "[l.cb={";
  auto& costs = ec.l.cb.costs;
  for (auto c = costs.cbegin(); c != costs.cend(); ++c)
  {
    strstream << "{c=" << c->cost << ",a=" << c->action << ",p=" << c->probability << ",pp=" << c->partial_prediction
              << "}";
  }
  strstream << "}]";
  return strstream.str();
}

std::string simple_label_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "[l=" << ec.l.simple.label << ",w=" << ec.weight << "]";
  return strstream.str();
}

std::string scalar_pred_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "[p=" << ec.pred.scalar << ", pp=" << ec.partial_prediction << "]";
  return strstream.str();
}

std::string a_s_pred_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "ec.pred.a_s[";
  for (uint32_t i = 0; i < ec.pred.a_s.size(); i++)
  { strstream << "(" << i << " = " << ec.pred.a_s[i].action << ", " << ec.pred.a_s[i].score << ")"; }
  strstream << "]";
  return strstream.str();
}

std::string multiclass_pred_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "ec.pred.multiclass = " << ec.pred.multiclass;
  return strstream.str();
}

std::string prob_dist_pred_to_string(const example& ec)
{
  std::stringstream strstream;
  strstream << "ec.pred.prob_dist[";
  for (uint32_t i = 0; i < ec.pred.pdf.size(); i++)
  {
    strstream << "(" << i << " = " << ec.pred.pdf[i].left << "-" << ec.pred.pdf[i].right << ", "
              << ec.pred.pdf[i].pdf_value << ")";
  }
  strstream << "]";
  return strstream.str();
}

namespace vw
{
example* alloc_examples(size_t count)
{
  example* ec = calloc_or_throw<example>(count);
  if (ec == nullptr) return nullptr;
  for (size_t i = 0; i < count; i++) { new (ec + i) example; }
  return ec;
}

void dealloc_examples(example* example_ptr, size_t count)
{
  for (size_t i = 0; i < count; i++) { (example_ptr + i)->~example(); }
  free(example_ptr);
}

void finish_example(workspace&, example&);
void clean_example(workspace&, example&, bool rewind);

void finish_example(workspace& all, multi_ex& ec_seq)
{
  for (example* ecc : ec_seq) vw::finish_example(all, *ecc);
}

void return_multiple_example(workspace& all, v_array<example*>& examples)
{
  for (auto ec : examples) { clean_example(all, *ec, true); }
  examples.clear();
}

}  // namespace vw

std::string debug_depth_indent_string(const example& ec)
{
  return debug_depth_indent_string(ec._debug_current_reduction_depth);
}
std::string debug_depth_indent_string(const multi_ex& ec) { return debug_depth_indent_string(*ec[0]); }

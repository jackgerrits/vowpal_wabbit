// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "named_labels.h"
#include "parse_primitives.h"

#include "io/logger.h"

namespace logger = vw::io::logger;

void vw::named_labels::initialize_maps_from_input_string()
{
  tokenize(',', m_label_list, m_id2name);

  m_K = static_cast<uint32_t>(m_id2name.size());
  m_name2id.max_load_factor(0.25);
  m_name2id.reserve(m_K);

  for (uint32_t k = 0; k < m_K; k++)
  {
    const std::string_view& l = m_id2name[static_cast<size_t>(k)];
    auto iter = m_name2id.find(l);
    if (iter != m_name2id.end()) THROW("error: label dictionary initialized with multiple occurances of: " << l);
    m_name2id.emplace(l, k + 1);
  }
}

vw::named_labels::named_labels(std::string label_list) : m_label_list(std::move(label_list))
{
  initialize_maps_from_input_string();
}

vw::named_labels::named_labels(const named_labels& other) : m_label_list(other.m_label_list)
{
  initialize_maps_from_input_string();
}

vw::named_labels& vw::named_labels::operator=(const vw::named_labels& other)
{
  if (this != &other)
  {
    m_label_list = other.m_label_list;
    initialize_maps_from_input_string();
  }

  return *this;
}

uint32_t vw::named_labels::get(std::string_view s) const
{
  auto iter = m_name2id.find(s);
  if (iter == m_name2id.end())
  {
    logger::errlog_warn("warning: missing named label '{}'", s);
    return 0;
  }
  return iter->second;
}

uint32_t vw::named_labels::getK() const { return m_K; }

std::string_view vw::named_labels::get(uint32_t v) const
{
  static_assert(sizeof(size_t) >= sizeof(uint32_t), "size_t is smaller than 32-bits. Potential overflow issues.");
  if ((v == 0) || (v > m_K)) { return std::string_view(); }

  return m_id2name[static_cast<size_t>(v - 1)];
}

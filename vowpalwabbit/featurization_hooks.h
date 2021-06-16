// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "vw_string_view.h"
#include "feature_group.h"
#include "example.h"
#include "hashstring.h"


#include <vector>
#include <array>
#include <shared_ptr>

struct featurization_hook
{
  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value) = 0;
};

template <bool audit>
struct affix_hook : public featurization_hook
{
  std::array<uint64_t, NUM_NAMESPACES>* _affix_features;
  hash_func_t hasher;

  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value)
  {
    if (((*_affix_features)[ns_index] > 0) && (!feature_name.empty()))
    {
      features& affix_fs = ex.feature_space[affix_namespace];
      if (affix_fs.size() == 0) ex.indices.push_back(affix_namespace);
      uint64_t affix = (*_affix_features)[ns_index];

      while (affix > 0)
      {
        bool is_prefix = affix & 0x1;
        uint64_t len = (affix >> 1) & 0x7;
        VW::string_view affix_name(feature_name);
        if (affix_name.size() > len)
        {
          if (is_prefix)
            affix_name.remove_suffix(affix_name.size() - len);
          else
            affix_name.remove_prefix(affix_name.size() - len);
        }

        auto word_hash = hasher(affix_name.begin(), affix_name.length(), namespace_hash) *
            (affix_constant + (affix & 0xF) * quadratic_constant);
        affix_fs.push_back(feat_value, word_hash);
        if (audit)
        {
          auto idx_str = feat_idx == ' ' ? "" : std::to_string(feat_idx);
          auto audit_str =
              fmt::format("{}{}{}={}", idx_str, is_prefix ? '+' : '-', '0' + static_cast<char>(len), affix_name);
          affix_fs.space_names.push_back(audit_strings_ptr(new audit_strings("affix", audit_str)));
        }
        affix >>= 4;
      }
    }
  }
};

template <bool audit>
struct spelling_hook : public featurization_hook
{
  std::array<bool, NUM_NAMESPACES>* _spelling_features;
  v_array<char> _spelling;

  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value)
  {
    if ((*_spelling_features)[ns_index])
    {
      features& spell_fs = ex.feature_space[spelling_namespace];
      if (spell_fs.size() == 0) ex.indices.push_back(spelling_namespace);
      // v_array<char> spelling;
      _spelling.clear();
      for (char c : feature_name)
      {
        char d = 0;
        if ((c >= '0') && (c <= '9'))
          d = '0';
        else if ((c >= 'a') && (c <= 'z'))
          d = 'a';
        else if ((c >= 'A') && (c <= 'Z'))
          d = 'A';
        else if (c == '.')
          d = '.';
        else
          d = '#';
        // if ((spelling.size() == 0) || (spelling.last() != d))
        _spelling.push_back(d);
      }

      VW::string_view spelling_strview(_spelling.begin(), _spelling.size());
      auto word_hash = hashstring(spelling_strview.begin(), spelling_strview.length(), namespace_hash);
      spell_fs.push_back(feat_value, word_hash);
      if (audit)
      {
        v_array<char> spelling_v = v_init<char>();
        if (ns_index != ' ')
        {
          spelling_v.push_back(ns_index);
          spelling_v.push_back('_');
        }
        spelling_v.insert(spelling_v.end(), spelling_strview.begin(), spelling_strview.end());
        spelling_v.push_back('\0');
        spell_fs.space_names.push_back(audit_strings_ptr(new audit_strings("spelling", spelling_v.begin())));
      }
    }
  }
};

template <bool audit>
struct dictionary_hook : public featurization_hook
{
  std::array<std::vector<std::shared_ptr<feature_dict>>, NUM_NAMESPACES>* _namespace_dictionaries;

  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value)
  {
    if ((*_namespace_dictionaries)[ns_index].size() > 0)
    {
      // Heterogeneous lookup not yet implemented in std
      // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0919r0.html
      const std::string feature_name_str(feature_name);
      for (const auto& map : (*_namespace_dictionaries)[ns_index])
      {
        const auto& feats_it = map->find(feature_name_str);
        if ((feats_it != map->end()) && (feats_it->second->values.size() > 0))
        {
          const auto& feats = feats_it->second;
          features& dict_fs = ex.feature_space[dictionary_namespace];
          if (dict_fs.size() == 0) ex.indices.push_back(dictionary_namespace);
          dict_fs.values.insert(dict_fs.values.end(), feats->values.begin(), feats->values.end());
          dict_fs.indicies.insert(dict_fs.indicies.end(), feats->indicies.begin(), feats->indicies.end());
          dict_fs.sum_feat_sq += feats->sum_feat_sq;
          if (audit)
          {
            for (const auto& id : feats->indicies)
            {
              dict_fs.space_names.push_back(
                  audit_strings_ptr(new audit_strings("dictionary", fmt::format("{}_{}={}", ns_index, feature_name, id))));
            }
          }
        }
      }
    }
  }
};


template class dictionary_hook<true>;
template class dictionary_hook<false>;
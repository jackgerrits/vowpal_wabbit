// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "featurization_hooks.h"

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
      float feat_value) override;

};

template <bool audit>
struct spelling_hook : public featurization_hook
{
  std::array<bool, NUM_NAMESPACES>* _spelling_features;
  v_array<char> _spelling;

  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value) override;
};

template <bool audit>
struct dictionary_hook : public featurization_hook
{
  std::array<std::vector<std::shared_ptr<feature_dict>>, NUM_NAMESPACES>* _namespace_dictionaries;

  virtual void handle(example& ex, VW::string_view ns_str, VW::string_view feature_name,
      VW::string_view string_feature_value, uint64_t namespace_hash, namespace_index ns_index, feature_index feat_idx,
      float feat_value) override;
};

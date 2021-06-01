// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "namespaced_features.h"
#include "vw_exception.h"

using namespace VW;

features* namespaced_features::get_feature_group(uint64_t hash)
{
  auto it = _hash_to_index_mapping.find(hash);
  if (it == _hash_to_index_mapping.end()) { return nullptr; }

  return &_feature_groups[it->second];
}

const features* namespaced_features::get_feature_group(uint64_t hash) const
{
  auto it = _hash_to_index_mapping.find(hash);
  if (it == _hash_to_index_mapping.end()) { return nullptr; }

  return &_feature_groups[it->second];
}

std::vector<namespace_index> namespaced_features::get_indices() const
{
  auto indices_copy = _namespace_indices;
  std::sort(indices_copy.begin(), indices_copy.end());
  auto last = std::unique(indices_copy.begin(), indices_copy.end());
  indices_copy.erase(last, indices_copy.end());
  return indices_copy;
}

std::pair<namespaced_features::indexed_iterator, namespaced_features::indexed_iterator>
namespaced_features::get_namespace_index_groups(namespace_index ns_index)
{
  return std::make_pair(namespace_index_begin(ns_index), namespace_index_end(ns_index));
}

std::pair<namespaced_features::const_indexed_iterator, namespaced_features::const_indexed_iterator>
namespaced_features::get_namespace_index_groups(namespace_index ns_index) const
{
  return std::make_pair(namespace_index_begin(ns_index), namespace_index_end(ns_index));
}

features& namespaced_features::get_or_create_feature_group(uint64_t hash, namespace_index ns_index)
{
  auto* existing_group = get_feature_group(hash);
  if (existing_group == nullptr)
  {
    _feature_groups.emplace_back();
    _namespace_indices.push_back(ns_index);
    _namespace_hashes.push_back(hash);
    auto new_index = _feature_groups.size() - 1;
    _hash_to_index_mapping[hash] = new_index;
    _legacy_indices_to_index_mapping[ns_index].push_back(new_index);
    existing_group = &_feature_groups.back();
  }

  return *existing_group;
}

const features& namespaced_features::operator[](uint64_t hash) const
{
  auto* existing_group = get_feature_group(hash);
#ifndef VW_NOEXCEPT
  if (existing_group == nullptr) { THROW("No group found for hash: " << hash); }
#endif
  return *existing_group;
}
features& namespaced_features::operator[](uint64_t hash)
{
  auto* existing_group = get_feature_group(hash);
#ifndef VW_NOEXCEPT
  if (existing_group == nullptr) { THROW("No group found for hash: " << hash); }
#endif
  return *existing_group;
}

void namespaced_features::remove_feature_group(uint64_t hash)
{
  if (_hash_to_index_mapping.count(hash) == 0) { return; }
  auto existing_index = _hash_to_index_mapping[hash];

  // Remove item from each vector at this index.
  _feature_groups.erase(_feature_groups.begin() + existing_index);
  _namespace_indices.erase(_namespace_indices.begin() + existing_index);
  _namespace_hashes.erase(_namespace_hashes.begin() + existing_index);
  _hash_to_index_mapping.erase(hash);

  for (auto& kv : _legacy_indices_to_index_mapping)
  {
    auto& index_vec = kv.second;
    // Remove this index from ns_index mappings if it exists
    auto it = std::find(index_vec.begin(), index_vec.end(), existing_index);
    if (it != index_vec.end()) { index_vec.erase(it); }

    // Shift down any index that came after this one.
    for (auto& idx : index_vec)
    {
      if (idx > existing_index) { idx -= 1; }
    }
  }

  // If any groups are left empty, remove them.
  for (auto it = _legacy_indices_to_index_mapping.begin(); it != _legacy_indices_to_index_mapping.end();)
  {
    if (it->second.empty()) { it = _legacy_indices_to_index_mapping.erase(it); }
    else
    {
      ++it;
    }
  }

  for (auto& kv : _hash_to_index_mapping)
  {
    if (kv.second > existing_index) { kv.second -= 1; }
  }
}

generic_range<namespaced_features::indexed_iterator> namespaced_features::namespace_index_range(
    namespace_index ns_index)
{
  return {namespace_index_begin(ns_index), namespace_index_end(ns_index)};
}

generic_range<namespaced_features::const_indexed_iterator> namespaced_features::namespace_index_range(
    namespace_index ns_index) const
{
  return {namespace_index_begin(ns_index), namespace_index_end(ns_index)};
}

namespaced_features::indexed_iterator namespaced_features::namespace_index_begin(namespace_index ns_index)
{
  auto it = _legacy_indices_to_index_mapping.find(ns_index);
  if (it == _legacy_indices_to_index_mapping.end())
  { return {nullptr, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()}; }
  return {it->second.data(), _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}

namespaced_features::indexed_iterator namespaced_features::namespace_index_end(namespace_index ns_index)
{
  auto it = _legacy_indices_to_index_mapping.find(ns_index);
  if (it == _legacy_indices_to_index_mapping.end())
  { return {nullptr, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()}; }
  return {it->second.data() + it->second.size(), _feature_groups.data(), _namespace_indices.data(),
      _namespace_hashes.data()};
}

namespaced_features::const_indexed_iterator namespaced_features::namespace_index_begin(namespace_index ns_index) const
{
  auto it = _legacy_indices_to_index_mapping.find(ns_index);
  if (it == _legacy_indices_to_index_mapping.end())
  { return {nullptr, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()}; }
  return {it->second.data(), _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}

namespaced_features::const_indexed_iterator namespaced_features::namespace_index_end(namespace_index ns_index) const
{
  auto it = _legacy_indices_to_index_mapping.find(ns_index);
  if (it == _legacy_indices_to_index_mapping.end())
  { return {nullptr, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()}; }
  return {it->second.data() + it->second.size(), _feature_groups.data(), _namespace_indices.data(),
      _namespace_hashes.data()};
}

namespaced_features::const_indexed_iterator namespaced_features::namespace_index_cbegin(namespace_index ns_index) const
{
  return namespace_index_begin(ns_index);
}

namespaced_features::const_indexed_iterator namespaced_features::namespace_index_cend(namespace_index ns_index) const
{
  return namespace_index_end(ns_index);
}

namespaced_features::iterator namespaced_features::begin()
{
  return {0, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}
namespaced_features::iterator namespaced_features::end()
{
  return {_feature_groups.size(), _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}
namespaced_features::const_iterator namespaced_features::begin() const
{
  return {0, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}
namespaced_features::const_iterator namespaced_features::end() const
{
  return {_feature_groups.size(), _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}
namespaced_features::const_iterator namespaced_features::cbegin() const
{
  return {0, _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}
namespaced_features::const_iterator namespaced_features::cend() const
{
  return {_feature_groups.size(), _feature_groups.data(), _namespace_indices.data(), _namespace_hashes.data()};
}

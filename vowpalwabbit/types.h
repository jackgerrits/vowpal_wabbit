// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "phantom_type.h"

namespace VW
{
// Mask is the parse mask bit mask
// Multiplier is the *= wpp << stride_shift multiplier
template <bool mask_applied, bool multiplier_applied>
struct hash_prop_tag_t {};

using strong_namespace_index = details::phantom_type_t<unsigned char, struct namespace_index_tag_t>;
using raw_hash = details::phantom_type_t<uint64_t, hash_prop_tag_t<false, false>>;
using masked_hash = details::phantom_type_t<uint64_t, hash_prop_tag_t<true, false>>;
using prepped_hash = details::phantom_type_t<uint64_t, hash_prop_tag_t<true, true>>;
}

inline VW::strong_namespace_index operator "" _ns(char namespace_char)
{
    return VW::strong_namespace_index{static_cast<unsigned char>(namespace_char)};
};
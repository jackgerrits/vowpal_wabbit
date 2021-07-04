// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include <utility>

namespace VW
{
namespace details
{
template <typename BaseType, typename TagType>
struct phantom_type_t
{
  constexpr phantom_type_t() = default;
  explicit constexpr phantom_type_t(const BaseType& value) : _value(value) {}
  explicit constexpr phantom_type_t(const BaseType&& value) : _value(std::move(value)) {}
  operator BaseType&() noexcept { return _value; }
  constexpr operator const BaseType&() const noexcept { return _value; }
  BaseType& get() noexcept { return _value; }
  constexpr const BaseType& get() const noexcept { return _value; }

private:
  BaseType _value;
};

}  // namespace details
}  // namespace VW

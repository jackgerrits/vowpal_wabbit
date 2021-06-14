#pragma once

#include "hashstring.h"

#include <boost/version.hpp>

#if BOOST_VERSION < 106100
#  include <boost/utility/string_ref.hpp>
namespace vw
{
using string_view = boost::string_ref;
}
#else
#  include <boost/utility/string_view.hpp>
namespace vw
{
using string_view = boost::string_view;
}
#endif

namespace std
{
// boost vw::string_view hashing isn't available until 1.69. Implement our own for now
template <>
struct hash<vw::string_view>
{
  size_t operator()(const vw::string_view& s) const { return hashstring(s.begin(), s.length(), 0); }
};
}  // namespace std

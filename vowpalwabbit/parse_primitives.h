// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>

#include "v_array.h"
#include "hashstring.h"
#include <string_view>
#include "fast_pow10.h"
#include "future_compat.h"

#include "io/logger.h"

inline std::vector<std::string_view> tokenize(char delim, std::string_view s)
{
  std::vector<std::string_view> tokens;
  size_t token_start = 0;
  size_t token_end = 0;

  for (size_t i = 0; i < s.size(); i++)
  {
    if (s[i] == delim)
    {
      if (token_end - token_start > 0)
      {
        tokens.emplace_back(s.data() + token_start, token_end - token_start);
        token_start = token_end + 1;
        token_end = token_start;
      }
      else
      {
        token_start++;
        token_end++;
      }
    }
    else
    {
      token_end++;
    }
  }

  if (token_end - token_start > 0) { tokens.emplace_back(s.data() + token_start, token_end - token_start); }

  return tokens;
}

// This function returns a vector of strings (not string_views) because we need to remove the escape characters
std::vector<std::string> escaped_tokenize(char delim, std::string_view s);

inline const char* safe_index(const char* start, char v, const char* max)
{
  while (start != max && *start != v) start++;
  return start;
}

// The following function is a home made strtof. The
// differences are :
//  - much faster (around 50% but depends on the  string to parse)
//  - less error control, but utilised inside a very strict parser
//    in charge of error detection.
inline FORCE_INLINE float parseFloat(const char* p, size_t& end_idx, const char* endLine = nullptr)
{
  const char* start = p;
  bool endLine_is_null = endLine == nullptr;

  end_idx = 0;

  if (!p || !*p) { return 0; }
  int s = 1;
  while ((*p == ' ') && (endLine_is_null || p < endLine)) p++;

  if (*p == '-')
  {
    s = -1;
    p++;
  }

  float acc = 0;
  while (*p >= '0' && *p <= '9' && (endLine_is_null || p < endLine)) acc = acc * 10 + *p++ - '0';

  int num_dec = 0;
  if (*p == '.')
  {
    while (*(++p) >= '0' && *p <= '9' && (endLine_is_null || p < endLine))
    {
      if (num_dec < 35)
      {
        acc = acc * 10 + (*p - '0');
        num_dec++;
      }
    }
  }

  int exp_acc = 0;
  if ((*p == 'e' || *p == 'E') && (endLine_is_null || p < endLine))
  {
    p++;
    int exp_s = 1;
    if (*p == '-' && (endLine_is_null || p < endLine))
    {
      exp_s = -1;
      p++;
    }
    while (*p >= '0' && *p <= '9' && (endLine_is_null || p < endLine)) exp_acc = exp_acc * 10 + *p++ - '0';
    exp_acc *= exp_s;
  }
  if (*p == ' ' || *p == '\n' || *p == '\t' || p == endLine)  // easy case succeeded.
  {
    acc *= vw::fast_pow10(static_cast<int8_t>(exp_acc - num_dec));
    end_idx = p - start;
    return s * acc;
  }
  else
  {
    // can't use stod because that throws an exception. Use strtod instead.
    char* end = nullptr;
    auto ret = strtof(start, &end);
    if (end >= start) { end_idx = end - start; }
    return ret;
  }
}

inline float float_of_string(std::string_view s)
{
  size_t end_idx;
  float f = parseFloat(s.data(), end_idx, s.data() + s.size());
  if ((end_idx == 0 && s.size() > 0) || std::isnan(f))
  {
    vw::io::logger::log_warn("warning: {} is not a good float, replacing with 0", s);
    f = 0;
  }
  return f;
}

inline int int_of_string(std::string_view s, char*& end)
{
  // can't use stol because that throws an exception. Use strtol instead.
  int i = strtol(s.data(), &end, 10);
  if (end <= s.data() && s.size() > 0)
  {
    vw::io::logger::log_warn("warning: {} is not a good int, replacing with 0", s);
    i = 0;
  }

  return i;
}

inline int int_of_string(std::string_view s)
{
  char* end = nullptr;
  return int_of_string(s, end);
}

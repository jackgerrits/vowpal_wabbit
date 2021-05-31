// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include <exception>
#include <string>

namespace vw
{
enum class error_code : uint8_t
{
  sample_pdf_failed = 1,
  num_actions_gt_zero,
  options_disagree,
  not_implemented,
  strict_parse,
  invalid_value,
  unrecognized_option,
  unknown,
  unsupported
};

/// errno -> std::string
std::string errno_to_string(int error_number);
const char* to_string(error_code code);

class error : public ::std::exception
{
private:
  vw::error_code _code;
  std::string _message;

public:
  explicit error(std::string message) : _code(vw::error_code::unknown), _message(message) {}
  error(vw::error_code code, std::string message) : _code(code), _message(message) {}

  error(const error& ex) = default;
  error& operator=(const error& other) = default;
  error(error&& ex) = default;
  error& operator=(error&& other) = default;
  ~error() noexcept = default;

  const char* what() const noexcept override { return _message.c_str(); }
};
}  // namespace vw

#define _UNUSED(x) ((void)(x))

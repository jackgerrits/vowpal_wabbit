// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include "vw_exception.h"

#include <string.h>

#ifdef _WIN32
#  define NOMINMAX
#  include <Windows.h>
#else
#  include <locale.h>
#endif

namespace vw
{
std::string errno_to_string(int error_number)
{
#ifdef _WIN32
  constexpr auto BUFFER_SIZE = 256;
  std::array<char, BUFFER_SIZE> error_message_buffer;
  auto result = strerror_s(error_message_buffer.data(), error_message_buffer.size() - 1, error_number);
  if (result != 0) { return "unknown message for errno: " + std::to_string(error_number); }

  auto length = std::strlen(error_message_buffer.data());
  return std::string(error_message_buffer.data(), length);
#elif __APPLE__
  constexpr auto BUFFER_SIZE = 256;
  std::array<char, BUFFER_SIZE> error_message_buffer;
#  if defined(__GLIBC__) && defined(_GNU_SOURCE)
  // You must use the returned bffer and not the passed in buffer the GNU version.
  char* message_buffer = strerror_r(error_number, error_message_buffer.data(), error_message_buffer.size() - 1);
  auto length = std::strlen(message_buffer);
  return std::string(message_buffer, length);
#  else
  auto result = strerror_r(error_number, error_message_buffer.data(), error_message_buffer.size() - 1);
  if (result != 0) { return "unknown message for errno: " + std::to_string(error_number); }
  auto length = std::strlen(error_message_buffer.data());
  return std::string(error_message_buffer.data(), length);
#  endif
#else
  // Passing "" for the locale means use the default system locale
  locale_t locale = newlocale(LC_ALL_MASK, "", static_cast<locale_t>(nullptr));

  if (locale == static_cast<locale_t>(nullptr))
  { return "Failed to create locale when getting error message for errno: " + std::to_string(error_number); }

  // Even if error_number is unknown, will return a "Unknown error nnn" message.
  std::string message = strerror_l(error_number, locale);
  freelocale(locale);
  return message;
#endif
}

const char* to_string(error_code code)
{
  switch (code)
  {
    case error_code::sample_pdf_failed:
      return "Failed to sample from pdf";
    case error_code::num_actions_gt_zero:
      return "Number of leaf nodes must be greater than zero";
    case error_code::options_disagree:
      return "Different values specified for two options that are constrained to be the same.";
    case error_code::not_implemented:
      return "Not implemented.";
    default:
      return "Unknown error";
  }
}

}  // namespace vw

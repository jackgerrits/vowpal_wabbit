#ifndef STATIC_LINK_VW
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include "parse_args.h"
#include "parse_example.h"

BOOST_AUTO_TEST_CASE(spoof_hex_encoded_namespace_test)
{
  BOOST_CHECK_EQUAL(spoof_hex_encoded_namespaces("test"), "test");
  BOOST_CHECK_EQUAL(spoof_hex_encoded_namespaces("10"), "10");
  BOOST_CHECK_EQUAL(spoof_hex_encoded_namespaces("\\x01"), "\x01");
  BOOST_CHECK_EQUAL(spoof_hex_encoded_namespaces("\\xab"), "\xab");
  BOOST_CHECK_EQUAL(spoof_hex_encoded_namespaces("\\x01 unrelated \\x56"), "\x01 unrelated \x56");
}

BOOST_AUTO_TEST_CASE(to_encoded_string_test)
{
  BOOST_CHECK_EQUAL(VW::diagnostics::encode_string("test"), "test");
  BOOST_CHECK_EQUAL(VW::diagnostics::encode_string(std::string("te\x0st", 5)), "te\\x00st");
  BOOST_CHECK_EQUAL(VW::diagnostics::encode_string(std::string("te\xC5st", 5)), "te\\xC5st");
}

BOOST_AUTO_TEST_CASE(to_encoded_index_test)
{
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index("test", 2), 2);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index("test", 4), 4);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index("test", 6), 4);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index(std::string("te\x0st", 5), 1), 1);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index(std::string("te\x0st", 5), 2), 2);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index(std::string("te\x0st", 5), 3), 6);
  BOOST_CHECK_EQUAL(VW::diagnostics::to_encoded_string_index(std::string("te\x0st", 5), 3), 6);
}

BOOST_AUTO_TEST_CASE(to_encoded_create_underlines)
{
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("test", 0, 0), "^");
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("test", 0, 1), "^~");
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("test", 1, 1), " ^");
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("test", 1, 2), " ^~");

  // te\xC5st
  //        ^
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("te\xC5st", 4, 4), "       ^");
  // te\xC5st
  //   ^~~~
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("te\xC5st", 2, 2), "  ^~~~");
  // te\xC5st
  //   ^~~~~
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("te\xC5st", 2, 3), "  ^~~~~");
  // te\xC5st
  //  ^~~~~~
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("te\xC5st", 1, 3), " ^~~~~~");
  // te\xC5
  //   ^~~~
  BOOST_CHECK_EQUAL(VW::diagnostics::make_diagnostic_underline("te\xC5", 2, 2), "  ^~~~");
}

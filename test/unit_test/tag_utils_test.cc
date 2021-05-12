#ifndef STATIC_LINK_VW
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include "test_common.h"
#include "vw.h"
#include "example.h"

#include <vector>
#include "tag_utils.h"
#include "test_common.h"

BOOST_AUTO_TEST_CASE(tag_with_seed__seed_extraction)
{
  auto vw = vw::initialize("--json --chain_hash --no_stdin --quiet", nullptr, false, nullptr, nullptr);
  std::string json = R"(
  {
    "_label": 1,
    "_tag": "seed=test_seed",
    "features": {
      "f":0
    }
  })";

  auto examples = parse_json(*vw, json);
  auto example = examples[0];

  std::string_view expected{"test_seed"};

  std::string_view seed;

  auto extracted = vw::try_extract_random_seed(*example, seed);
  BOOST_CHECK_EQUAL(true, extracted);
  BOOST_CHECK_EQUAL(expected, seed);

  vw::finish_example(*vw, examples);
  vw::finish(*vw);
}

BOOST_AUTO_TEST_CASE(tag_without_seed__seed_extraction)
{
  auto vw = vw::initialize("--json --chain_hash --no_stdin --quiet", nullptr, false, nullptr, nullptr);
  std::string json = R"(
  {
    "_label": 1,
    "_tag": "some tag without seed",
    "features": {
      "f":0
    }
  })";

  auto examples = parse_json(*vw, json);
  auto example = examples[0];

  std::string_view seed;

  auto extracted = vw::try_extract_random_seed(*example, seed);
  BOOST_CHECK_EQUAL(false, extracted);

  vw::finish_example(*vw, examples);
  vw::finish(*vw);
}

BOOST_AUTO_TEST_CASE(no_tag__seed_extraction)
{
  auto vw = vw::initialize("--json --chain_hash --no_stdin --quiet", nullptr, false, nullptr, nullptr);
  std::string json = R"(
  {
    "_label": 1,
    "features": {
      "f":0
    }
  })";

  auto examples = parse_json(*vw, json);
  auto example = examples[0];

  std::string_view seed;

  auto extracted = vw::try_extract_random_seed(*example, seed);
  BOOST_CHECK_EQUAL(false, extracted);

  vw::finish_example(*vw, examples);
  vw::finish(*vw);
}



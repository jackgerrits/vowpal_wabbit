#ifndef STATIC_LINK_VW
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include "test_common.h"
#include "vw.h"

BOOST_AUTO_TEST_CASE(chain_hashing_between_formats)
{
  feature_index txt_idx;
  feature_index json_idx;

  std::string text("1 |f a:b");
  std::string json_text = R"(
    {
      "_label": 1,
      "f": {
        "a": "b"
      }
    })";

  auto vw = vw::initialize("--quiet --chain_hash", nullptr, false, nullptr, nullptr);
  {
    multi_ex examples;
    examples.push_back(&vw::get_unused_example(vw));
    auto example = examples[0];
    vw::read_line(*vw, example, const_cast<char*>(text.c_str()));
    auto& indices = example->feature_space['f'].indicies;
    txt_idx = indices[0];
    vw::finish_example(*vw, examples);
  }
  {
    auto examples = parse_json(*vw, json_text);
    auto example = examples[0];

    auto& indices = example->feature_space['f'].indicies;
    json_idx = indices[0];
    vw::finish_example(*vw, examples);
  }
  BOOST_CHECK_EQUAL(txt_idx, json_idx);
  vw::finish(*vw);
}
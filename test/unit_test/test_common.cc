#include "test_common.h"

multi_ex parse_json(workspace& all, const std::string& line)
{
  v_array<example*> examples;
  examples.push_back(&vw::get_unused_example(&all));
  vw::read_line_json_s<true>(
      all, examples, (char*)line.c_str(), line.length(), (vw::example_factory_t)&vw::get_unused_example, (void*)&all);

  multi_ex result;
  for (size_t i = 0; i < examples.size(); ++i)
  {
    result.push_back(examples[i]);
  }
  return result;
}

multi_ex parse_dsjson(workspace& all, std::string line, DecisionServiceInteraction* interaction)
{
  v_array<example*> examples;
  examples.push_back(&vw::get_unused_example(&all));

  DecisionServiceInteraction local_interaction;
  if (interaction == nullptr) { interaction = &local_interaction; }

  vw::read_line_decision_service_json<true>(all, examples, (char*)line.c_str(), line.size(), false,
      (vw::example_factory_t)&vw::get_unused_example, (void*)&all, interaction);

  multi_ex result;
  for (const auto& ex : examples) { result.push_back(ex); }
  return result;
}

bool is_invoked_with(const std::string& arg)
{
  for (size_t i = 0; i < boost::unit_test::framework::master_test_suite().argc; i++)
  {
    if (std::string_view(boost::unit_test::framework::master_test_suite().argv[i]).find(arg) != std::string::npos)
    { return true; } }
  return false;
}

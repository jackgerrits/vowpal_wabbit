add_executable(vw-unit-test.out
  cats_test.cc
  cats_tree_tests.cc
  cats_user_provided_pdf.cc
  cb_explore_adf_test.cc
  ccb_parser_test.cc
  ccb_test.cc
  chain_hashing.cc
  chain_hashing.cc
  continuous_actions_parser_test.cc
  dsjson_parser_test.cc
  error_test.cc
  example_header_test.cc
  example_test.cc
  explore_test.cc
  guard_test.cc
  initialize_test.cc
  io_adapter_test.cc
  json_parser_test.cc
  main.cc
  math_test.cc
  multiclass_label_parser_test.cc
  numeric_cast_tests.cc
  object_pool_test.cc
  offset_tree_tests.cc
  offset_tree_tests.cc
  options_boost_po_test.cc
  options_test.cc
  ostream_test.cc
  parse_args_test.cc
  parser_test.cc
  pmf_to_pdf_test.cc
  power_test.cc
  prediction_test.cc
  random_test.cc
  random_test.cc
  scope_exit_test.cc
  slates_parser_test.cc
  slates_test.cc
  stable_unique_tests.cc
  tag_utils_test.cc
  test_common.cc
  test_common.h
  tokenize_tests.cc
  v_array_test.cc
  vw_versions_test.cc
  weights_test.cc
)

if(BUILD_FLATBUFFERS)
  target_sources(vw-unit-test.out PRIVATE flatbuffer_parser_test.cc)
endif()

# Add the include directories from vw target for testing
target_include_directories(vw-unit-test.out PRIVATE $<TARGET_PROPERTY:vw,INCLUDE_DIRECTORIES>)
target_link_libraries(vw-unit-test.out PRIVATE vw allreduce Boost::unit_test_framework)

# Communicate that Boost Unit Test is being statically linked
if(STATIC_LINK_VW)
  target_compile_definitions(vw-unit-test.out PRIVATE STATIC_LINK_VW)
endif()

add_test(
  NAME vw_unit_test
  COMMAND $<TARGET_FILE:vw-unit-test.out>
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(config.h.in config.h)

# Use position independent code for all targets in this directory
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(vw_io STATIC io/io_adapter.h io/io_adapter.cc io/logger.h io/logger.cc)
target_include_directories(vw_io PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(vw_io PUBLIC ${spdlog_target} fmt::fmt PRIVATE vw_common_flags ZLIB::ZLIB)

if(SPDLOG_SYS_DEP)
  # this doesn't get defined when using a system-installed spdlog
  target_compile_definitions(vw_io PUBLIC SPDLOG_FMT_EXTERNAL)
endif()

add_library(VowpalWabbit::io ALIAS vw_io)

if(BUILD_FLATBUFFERS)
  add_subdirectory(parser/flatbuffer)
  add_library(FlatbuffersTarget INTERFACE)
  target_include_directories(FlatbuffersTarget INTERFACE ${FLATBUFFERS_INCLUDE_DIR})
  add_dependencies(FlatbuffersTarget fbschemas)
endif()

set(vw_all_headers
  action_score.h
  array_parameters_dense.h
  array_parameters.h
  audit_regressor.h
  autolink.h
  baseline.h
  best_constant.h
  binary.h
  boosting.h
  cache.h
  cats_pdf.h
  cats_tree.h
  cats.h
  cb_adf.h
  cb_algs.h
  cb_continuous_label.h
  cb_dro.h
  cb_explore_adf_bag.h
  cb_explore_adf_common.h
  cb_explore_adf_cover.h
  cb_explore_adf_first.h
  cb_explore_adf_greedy.h
  cb_explore_adf_regcb.h
  cb_explore_adf_squarecb.h
  cb_explore_adf_synthcover.h
  cb_explore_adf_rnd.h
  cb_explore_adf_softmax.h
  cb_explore_pdf.h
  cb_explore.h
  cb_label_parser.h
  cb.h
  cbify.h
  cb_to_cb_adf.h
  ccb_label.h
  ccb_reduction_features.h
  continuous_actions_reduction_features.h
  conditional_contextual_bandit.h
  confidence.h
  config.h.in
  constant.h
  cbzo.h
  correctedMath.h
  cost_sensitive.h
  crossplat_compat.h
  csoaa.h
  debug_print.h
  decision_scores.h
  distributionally_robust.h
  ect.h
  error_constants.h
  error_data.h
  error_reporting.h
  example_predict.h
  example.h
  explore_eval.h
  fast_pow10.h
  feature_group.h
  ftrl.h
  gd_predict.h
  gd.h
  gen_cs_example.h
  generate_interactions.h
  get_pmf.h
  global_data.h
  guard.h
  hashstring.h
  interactions_predict.h
  interactions.h
  io_buf.h
  json_utils.h
  label_dictionary.h
  label_parser.h
  learner.h
  loss_functions.h
  marginal.h
  memory.h
  metrics.h
  metric_sink.h
  multiclass.h
  multilabel_oaa.h
  multilabel.h
  numeric_casts.h
  kskip_ngram_transformer.h
  oaa.h
  object_pool.h
  offset_tree.h
  options_boost_po.h
  options_serializer_boost_po.h
  options_types.h
  options.h
  parse_args.h
  parse_dispatch_loop.h
  parse_example_json.h
  parse_example.h
  parse_primitives.h
  parse_regressor.h
  parse_slates_example_json.h
  parser.h
  pmf_to_pdf.h
  plt.h
  reduction_features.h
  print.h
  prob_dist_cont.h
  queue.h
  rand48.h
  reductions.h
  reductions_fwd.h
  sample_pdf.h
  scope_exit.h
  scorer.h
  shared_data.h
  shared_feature_merger.h
  simple_label_parser.h
  simple_label.h
  slates_label.h
  slates.h
  stable_unique.h
  tag_utils.h
  topk.h
  unique_sort.h
  v_array.h
  version.h
  vw_exception.h
  vw_math.h
  vw_validate.h
  vw_versions.h
  vw.h
  vwvis.h
  warm_cb.h
)

if(BUILD_FLATBUFFERS)
  set(vw_all_headers ${vw_all_headers} parser/flatbuffer/parse_example_flatbuffer.h)
endif()

if(BUILD_EXTERNAL_PARSER)
  set(vw_all_headers ${vw_all_headers} ${external_parser_headers})
endif()

set(vw_all_sources
  action_score.cc
  audit_regressor.cc
  autolink.cc
  baseline.cc
  best_constant.cc
  binary.cc
  boosting.cc
  cache.cc
  cats_pdf.cc
  cats_tree.cc
  cats.cc
  cb_adf.cc
  cb_algs.cc
  cb_continuous_label.cc
  cb_dro.cc
  cb_explore_adf_bag.cc
  cb_explore_adf_cover.cc
  cb_explore_adf_first.cc
  cb_explore_adf_greedy.cc
  cb_explore_adf_regcb.cc
  cb_explore_adf_squarecb.cc
  cb_explore_adf_synthcover.cc
  cb_explore_adf_rnd.cc
  cb_explore_adf_softmax.cc
  cb_explore_pdf.cc
  cb_explore.cc
  cb_sample.cc
  cb.cc
  cbify.cc
  cb_to_cb_adf.cc
  ccb_label.cc
  conditional_contextual_bandit.cc
  confidence.cc
  cbzo.cc
  cost_sensitive.cc
  csoaa.cc
  decision_scores.cc
  distributionally_robust.cc
  ect.cc
  example_predict.cc
  example.cc
  explore_eval.cc
  feature_group.cc
  ftrl.cc
  gd.cc
  gen_cs_example.cc
  generate_interactions.cc
  get_pmf.cc
  global_data.cc
  hashstring.cc
  interactions.cc
  io_buf.cc
  label_dictionary.cc
  learner.cc
  loss_functions.cc
  marginal.cc
  metrics.cc
  multiclass.cc
  multilabel_oaa.cc
  multilabel.cc
  kskip_ngram_transformer.cc
  oaa.cc
  offset_tree.cc
  options_boost_po.cc
  options_serializer_boost_po.cc
  parse_args.cc
  parse_example.cc
  parse_primitives.cc
  parse_regressor.cc
  parser.cc
  pmf_to_pdf.cc
  plt.cc
  print.cc
  prob_dist_cont.cc
  rand48.cc
  sample_pdf.cc
  scorer.cc
  shared_data.cc
  shared_feature_merger.cc
  simple_label_parser.cc
  simple_label.cc
  slates_label.cc
  slates.cc
  tag_utils.cc
  topk.cc
  unique_sort.cc
  version.cc
  vw_exception.cc
  vw_validate.cc
  warm_cb.cc
)

if(BUILD_FLATBUFFERS)
  set(vw_all_sources ${vw_all_sources}
    parser/flatbuffer/parse_example_flatbuffer.cc
    parser/flatbuffer/parse_label.cc)
endif()

if(BUILD_EXTERNAL_PARSER)
  set(vw_all_sources ${vw_all_sources} ${external_parser_sources})
endif()

add_library(vw STATIC ${vw_all_sources} ${vw_all_headers})

target_link_libraries(vw
  PUBLIC
    VowpalWabbit::explore Boost::boost ${spdlog_target} fmt::fmt
  PRIVATE
    vw_common_flags Boost::program_options ${CMAKE_DL_LIBS} ${LINK_THREADS} vw_io
    # Workaround an issue where RapidJSON needed to be exported tom install the target. This is
    # actually a private dependency and so do not "link" when processing targets for installation.
    # https://gitlab.kitware.com/cmake/cmake/issues/15415
    $<BUILD_INTERFACE:RapidJSON>)

if (BUILD_EXTERNAL_PARSER)
  target_compile_definitions(vw PUBLIC BUILD_EXTERNAL_PARSER)
  target_include_directories(vw PRIVATE ${EXTERNAL_PARSER_DIR})
  target_link_libraries(vw PRIVATE ${EXTERNAL_PARSER_LIBS})
endif()

if(BUILD_FLATBUFFERS)
  target_link_libraries(vw
      PRIVATE
      $<BUILD_INTERFACE:FlatbuffersTarget>)
  target_compile_definitions(vw PUBLIC BUILD_FLATBUFFERS)
endif()

add_library(VowpalWabbit::vw ALIAS vw)

target_include_directories(vw PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Add natvis file if the Generator is Visual Studio
if (MSVC_IDE)
  target_sources(vw PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/vw_types.natvis> )
endif()

if(NOT WIN32)
  find_file(HELP2MAN_EXECUTABLE help2man HINTS /bin /usr/bin /usr/local/bin)
  if (HELP2MAN_EXECUTABLE)
    add_custom_target(manpage ALL
      COMMAND ${HELP2MAN_EXECUTABLE}
        --no-info
        --name="Vowpal Wabbit -- fast online learning tool" $<TARGET_FILE:vw-bin>
        --output=$<TARGET_FILE:vw-bin>.1
      DEPENDS vw-bin)

      if(VW_INSTALL)
        install(FILES $<TARGET_FILE:vw-bin>.1 DESTINATION share/man/man1)
      endif()
  else()
    message(STATUS "help2man not found, please install it to generate manpages")
  endif()
endif()

# build main executable
add_executable(vw-bin main.cc)
target_link_libraries(vw-bin PRIVATE VowpalWabbit::vw Boost::program_options)
set_target_properties(vw-bin PROPERTIES OUTPUT_NAME vw)

if(STATIC_LINK_VW)
  target_link_libraries(vw-bin PRIVATE ${unix_static_flag})
endif()

if(VW_INSTALL)
  install(
    TARGETS vw vw-bin vw_io
    EXPORT VowpalWabbitConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  # VW target headers
  install(
    FILES ${vw_all_headers} ${CMAKE_CURRENT_BINARY_DIR}/config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vowpalwabbit)
  install(
    FILES io/io_adapter.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vowpalwabbit/io
  )
endif()

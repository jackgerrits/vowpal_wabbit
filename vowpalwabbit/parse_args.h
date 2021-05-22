// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include "global_data.h"
#include "options.h"

// Used in parse_source
struct input_options
{
  size_t port = 0;
  std::string pid_file;

  bool cache = false;
  std::vector<std::string> cache_files;
  bool json = false;
  bool dsjson = false;
  bool kill_cache = false;
  bool compressed = false;
  bool chain_hash_json = false;
  bool flatbuffer = false;
#ifdef BUILD_EXTERNAL_PARSER
  // pointer because it is an incomplete type
  std::unique_ptr<vw::external::parser_options> ext_opts;
#endif
};

// trace listener + context need to be passed at initialization to capture all messages.
workspace& parse_args(
    vw::config::options_i& options, trace_message_t trace_listener = nullptr, void* trace_context = nullptr);
void parse_modules(vw::config::options_i& options, workspace& all);
void parse_sources(vw::config::options_i& options, workspace& all, io_buf& model, bool skipModelLoad = false);

void merge_options_from_header_strings(const std::vector<std::string>& strings, bool skip_interactions,
    vw::config::options_i& options, bool& is_ccb_input_model);

vw::LEARNER::base_learner* setup_base(vw::config::options_i& options, workspace& all);

std::string spoof_hex_encoded_namespaces(const std::string& arg);
bool ends_with(const std::string& fullString, const std::string& ending);

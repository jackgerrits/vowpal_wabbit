// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include <string>
#include "global_data.h"
#include "options.h"

void read_regressor_file(workspace& all, std::vector<std::string> files, io_buf& io_temp);

void finalize_regressor(workspace& all, std::string reg_name);
void initialize_regressor(workspace& all);

void save_predictor(workspace& all, std::string reg_name, size_t current_pass);
void save_load_header(workspace& all, io_buf& model_file, bool read, bool text, std::string& file_options,
    vw::config::options_i& options);

void parse_mask_regressor_args(workspace& all, std::string feature_mask, std::vector<std::string> initial_regressors);

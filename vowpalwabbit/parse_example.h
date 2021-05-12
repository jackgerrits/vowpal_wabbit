// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include <cstdint>
#include "example.h"
#include "vw.h"
#include <string_view>

// example processing
typedef enum
{
  StringFeatures,
  JsonFeatures
} FeatureInputType;

void substring_to_example(workspace* all, example* ae, std::string_view example);

namespace vw
{
example& get_unused_example(workspace* all);
void read_line(workspace& all, example* ex, char* line);  // read example from the line.
void read_lines(workspace* all, const char* line, size_t len,
    v_array<example*>& examples);  // read examples from the new line separated strings.

}  // namespace vw

int read_features_string(workspace* all, v_array<example*>& examples);
size_t read_features(workspace* all, char*& line, size_t& num_chars);

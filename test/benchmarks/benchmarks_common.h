#pragma once

#include <cstddef>
#include <string>

std::string get_x_numerical_fts(int feature_size);
std::string get_x_string_fts(int feature_size);
std::string get_x_string_fts_no_label(int feature_size, size_t action_index = 0);
std::string get_x_string_fts_multi_ex(
    int feature_size, size_t actions, bool shared, bool label, size_t start_index = 0);

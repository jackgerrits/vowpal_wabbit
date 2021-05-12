
// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#pragma once

#include "v_array.h"
#include "io/io_adapter.h"
#include <iomanip>
#include <iostream>
#include <vector>

namespace ACTION_SCORE
{
struct action_score;
typedef v_array<action_score> action_scores;
}  // namespace ACTION_SCORE

struct workspace;
struct example;

namespace vw
{
// Each position in outer array is implicitly the decision corresponding to that index. Each inner array is the result
// of CB for that call.
using decision_scores_t = std::vector<ACTION_SCORE::action_scores>;

void print_decision_scores(vw::io::writer* f, const vw::decision_scores_t& decision_scores);

void print_update_ccb(
    workspace& all, std::vector<example*>& slots, const vw::decision_scores_t& decision_scores, size_t num_features);
void print_update_slates(
    workspace& all, std::vector<example*>& slots, const vw::decision_scores_t& decision_scores, size_t num_features);
}  // namespace vw

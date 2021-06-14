// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include "label_parser.h"
#include "v_array.h"

struct example;
struct vw;

namespace MULTILABEL
{
struct labels
{
  v_array<uint32_t> label_v;
};

void output_example(workspace& all, example& ec);

extern label_parser multilabel;

void print_update(workspace& all, bool is_test, example& ec, const v_array<example*>* ec_seq);
}  // namespace MULTILABEL

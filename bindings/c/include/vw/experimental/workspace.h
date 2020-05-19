// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once

#include "exports.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ErrorCode initialize(options*, bool skipModelLoad, trace_message_t trace_listener, void* trace_context, vw** output)
// typedef vw_status (read_func)(void*, char*, size_t, ssize_t*);
// vw_status initialize_with_model(options*, const uint8_t* model, size_t model_size, bool skipModelLoad, trace_message_t trace_listener, void* trace_context, vw** output)
// vw_status initialize_with_model(options*, void* context, read_func* read, bool skipModelLoad, trace_message_t trace_listener, void* trace_context, vw** output)
// ErrorCode initialize_cmdline_with_model(const char*, const uint8_t* model, size_t model_size, bool skipModelLoad, trace_message_t trace_listener, void* trace_context, vw** output);

// ErrorCode seed_vw_model(const vw*, options* opts, trace_message_t trace_listener, void* trace_context, vw**);
// ErrorCode seed_vw_model_cmdline(const vw*, const char* cmdline_opts, trace_message_t trace_listener, void* trace_context, vw**);

// const char* are_features_compatible(const vw*, const vw*);    // Probably better to return an enum with enum->string mappings available?

// // finish is now broken up into finish() and delete_vw().
// void finish(vw* all);
// void delete_vw(vw* all);
// void sync_stats(vw* all);    // This is only used with allreduce. Should we move this up the tech stack into its own wrapper?

vw_status vw_get_pred_type(vw_workspace* vw, int);
vw_status vw_get_label_type(vw_workspace* vw, int);

vw_status learn(vw_workspace* vw, vw_example* example);
vw_status learn_multiline(vw_workspace* vw, vw_example* example, int len);

vw_status predict(vw_workspace* vw, vw_example* example);
vw_status predict_multiline(vw_workspace* vw, vw_example* example, int len);

#ifdef __cplusplus
}
#endif

ErrorCode initialize_with_model(options*, vw_io_reader* model, bool skipModelLoad, trace_message_t trace_listener, void* trace_context, vw** output)

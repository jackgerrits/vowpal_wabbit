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

VW_DLL_PUBLIC vw_status example_get_feature_space_indices(const vw_example*, const unsigned char** indices, int* length);
// invalidates pointers
VW_DLL_PUBLIC vw_status example_push_feature_space_index(vw_example*, int value);
// invalidates pointers
VW_DLL_PUBLIC vw_status example_remove_feature_space_index(vw_example*, int index);

VW_DLL_PUBLIC vw_status allocate_feature_space(vw_feature_space**);
VW_DLL_PUBLIC vw_status deallocate_feature_space(vw_feature_space*);
VW_DLL_PUBLIC vw_status example_get_feature_space(const vw_example*, int index, vw_feature_space**);
VW_DLL_PUBLIC vw_status example_set_feature_space(vw_example*, int index, const vw_feature_space*);

VW_DLL_PUBLIC vw_status feature_space_copy(const vw_feature_space*, vw_feature_space*);

VW_DLL_PUBLIC vw_status feature_space_get_features(vw_feature_space*, uint64_t** ft_indices, float** ft_values, int* length);
// invalidates pointers
VW_DLL_PUBLIC vw_status feature_space_push_feature(vw_feature_space*, uint64_t ft_index, float ft_value);
// invalidates pointers
VW_DLL_PUBLIC vw_status feature_space_remove_feature(vw_feature_space*, int index);

VW_DLL_PUBLIC vw_status example_get_feature_offset(const vw_example*, int* feature_offset);
VW_DLL_PUBLIC vw_status example_set_feature_offset(vw_example*, int feature_offset );

VW_DLL_PUBLIC vw_status example_get_tag(const vw_example*, const char** tag);
VW_DLL_PUBLIC vw_status example_set_tag(vw_example*, const char* tag, int length);

VW_DLL_PUBLIC vw_status example_get_weight(const vw_example*, float* weight);
VW_DLL_PUBLIC vw_status example_set_weight(vw_example*, float weight);

VW_DLL_PUBLIC vw_status example_get_label(const vw_example*, void** label, vw_label_type);
VW_DLL_PUBLIC vw_status example_set_label(vw_example*, void* label, vw_label_type);

VW_DLL_PUBLIC vw_status example_get_prediction(const vw_example*, void** prediction, vw_prediction_type);
VW_DLL_PUBLIC vw_status example_set_prediction(vw_example*, void* prediction, vw_prediction_type);

#ifdef __cplusplus
}
#endif

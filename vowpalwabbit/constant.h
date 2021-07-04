// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#pragma once
#include <cstdint>
#include <cstddef>

#include "types.h"

constexpr int quadratic_constant = 27942141;
constexpr int cubic_constant = 21791;
constexpr int cubic_constant2 = 37663;
constexpr int affix_constant = 13903957;
constexpr uint64_t constant = 11650396;

constexpr float probability_tolerance = 1e-5f;

// FNV-like hash constant for 32bit
// http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
constexpr uint32_t FNV_prime = 16777619;

constexpr VW::strong_namespace_index default_namespace{32};// static_cast<unsigned char>(32);
constexpr VW::strong_namespace_index wildcard_namespace{58};  // :
constexpr VW::strong_namespace_index wap_ldf_namespace{126};
constexpr VW::strong_namespace_index history_namespace{127};
constexpr VW::strong_namespace_index constant_namespace{128};
constexpr VW::strong_namespace_index nn_output_namespace{129};
constexpr VW::strong_namespace_index autolink_namespace{130};
constexpr VW::strong_namespace_index neighbor_namespace{131};  // this is \x83 -- to do quadratic, say "-q a`printf "\x83"` on the command line
constexpr VW::strong_namespace_index affix_namespace{132};         // this is \x84
constexpr VW::strong_namespace_index spelling_namespace{133};      // this is \x85
constexpr VW::strong_namespace_index conditioning_namespace{134};  // this is \x86
constexpr VW::strong_namespace_index dictionary_namespace{135};    // this is \x87
constexpr VW::strong_namespace_index node_id_namespace{136};       // this is \x88
constexpr VW::strong_namespace_index baseline_enabled_message_namespace{137};  // this is \x89
constexpr VW::strong_namespace_index ccb_slot_namespace{139};
constexpr VW::strong_namespace_index ccb_id_namespace{140};

typedef float weight;

constexpr size_t NUM_NAMESPACES = 256;

constexpr const char* CCB_LABEL = "ccb";
constexpr const char* SLATES_LABEL = "slates";
constexpr const char* SHARED_TYPE = "shared";
constexpr const char* ACTION_TYPE = "action";
constexpr const char* SLOT_TYPE = "slot";
constexpr const char* CA_LABEL = "ca";
constexpr const char* PDF = "pdf";
constexpr const char* CHOSEN_ACTION = "chosen_action";

static constexpr uint32_t SHARED_EX_INDEX = 0;
static constexpr uint32_t TOP_ACTION_INDEX = 0;

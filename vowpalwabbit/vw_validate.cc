// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.
#include "global_data.h"
#include "vw_validate.h"
#include "vw_versions.h"
#include "shared_data.h"

#include "io/logger.h"

namespace logger = vw::io::logger;

namespace vw
{
void validate_version(workspace& all)
{
  if (all.model_file_ver < LAST_COMPATIBLE_VERSION)
  {
    throw vw::error(fmt::format("Model has possibly incompatible version! {}", all.model_file_ver.to_string()));
  }
  if (all.model_file_ver > PACKAGE_VERSION)
  {
    logger::errlog_warn("Warning: model version is more recent than vw version. This may not work.");
  }
}

void validate_min_max_label(workspace& all)
{
  if (all.sd->max_label < all.sd->min_label)
  {
    throw vw::error("Max label cannot be less than min label.");
  }
}

void validate_default_bits(workspace& all, uint32_t local_num_bits)
{
  if (all.default_bits != true && all.num_bits != local_num_bits)
  {
    throw vw::error(fmt::format("-b bits mismatch: command-line {} != {} stored in model", all.num_bits, local_num_bits));
  }
}

void validate_num_bits(workspace& all)
{
  const auto max_bits = sizeof(size_t) * 8 - 3;
  if (all.num_bits > max_bits)
  {
    throw vw::error(fmt::format("Only {} or fewer bits allowed.  If this is a serious limit, speak up.", max_bits));
  }
}
}  // namespace vw

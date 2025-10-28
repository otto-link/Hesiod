/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/graph/bake_config.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void BakeConfig::json_from(nlohmann::json const &json)
{
  json_safe_get(json, "resolution", resolution);
  json_safe_get(json, "nvariants", nvariants);
  json_safe_get(json, "force_distributed", force_distributed);
  json_safe_get(json, "force_auto_export", force_auto_export);
  json_safe_get(json, "rename_export_files", rename_export_files);
}

nlohmann::json BakeConfig::json_to() const
{
  nlohmann::json json;
  json["resolution"] = resolution;
  json["nvariants"] = nvariants;
  json["force_distributed"] = force_distributed;
  json["force_auto_export"] = force_auto_export;
  json["rename_export_files"] = rename_export_files;
  return json;
}

} // namespace hesiod

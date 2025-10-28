/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <filesystem>

#include "highmap/algebra.hpp"
#include "nlohmann/json.hpp"

namespace hesiod
{

// =====================================
// FlattenConfig
// =====================================
struct FlattenConfig
{
  FlattenConfig() = default;

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  // --- Methods ---
  void dump();

  // --- Members ---
  std::filesystem::path export_path = "";
  hmap::Vec2<int>       shape = {1024, 1024};
  hmap::Vec2<int>       tiling = {4, 4};
  float                 overlap = 0.5f;

  // graph_id / node_id / port_id
  std::vector<std::tuple<std::string, std::string, std::string>> ids;
};

} // namespace hesiod

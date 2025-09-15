/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file config.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <chrono>
#include <memory>

#include "nlohmann/json.hpp"

#include "highmap/algebra.hpp"

#define HSD_CONFIG hesiod::Config::get_config()

#define HSD_NODE_DOCUMENTATION_JSON_PATH "data/node_documentation.json"

namespace hesiod
{

class Config
{
public:
  Config() = default;
  static std::shared_ptr<Config> &get_config();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;
  void           load_from_file(const std::string &fname);
  void           save_to_file(const std::string &fname) const;

  // --- Data
  struct Window
  {
    bool                      open_graph_manager_at_startup = false;
    bool                      open_viewport_at_startup = false;
    std::chrono::milliseconds autosave_timer{60000}; // 60 seconds
    bool                      save_backup_file = true;
  } window;

  struct Viewer
  {
    int width = 512;
    int height = 512;
  } viewer;

  struct Nodes
  {
    hmap::Vec2<int> shape_preview = hmap::Vec2<int>(128, 128);
  } nodes;

private:
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  // static member to hold the singleton instance
  static std::shared_ptr<Config> instance;
};

} // namespace hesiod
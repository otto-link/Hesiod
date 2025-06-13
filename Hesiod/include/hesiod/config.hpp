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
#include <memory>

#define HSD_CONFIG hesiod::Config::get_config()

namespace hesiod
{

class Config
{
public:
  Config() = default;
  static std::shared_ptr<Config> &get_config();

  struct Window
  {
    bool open_graph_manager_at_startup = false;
    bool open_viewport_at_startup = true;
  } window;

private:
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  // static member to hold the singleton instance
  static std::shared_ptr<Config> instance;
};

} // namespace hesiod
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/config.hpp"

namespace hesiod
{

// Initialize the static member
std::shared_ptr<Config> Config::instance = nullptr;

std::shared_ptr<Config> &Config::get_config()
{
  if (!instance)
    instance = std::make_shared<Config>();
  return instance;
}

} // namespace hesiod

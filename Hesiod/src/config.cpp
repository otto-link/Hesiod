/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "attributes.hpp"

#include "hesiod/config.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

// Initialize the static member
std::shared_ptr<Config> Config::instance = nullptr;

Config::Config()
{
  LOG->info("Initializating Hesiod configuration...");

  std::string section = "";

  section = "Section 1/";

  this->add<attr::FloatAttribute>(section + "dx", 0.1f, -1.f, 1.f);

  section += "Section 2/";
  this->add<attr::BoolAttribute>(section + "periodic", false);

  section = "Graph editor/";

  section += "Broadcasting/";
  this->add<attr::BoolAttribute>(section + "Prevent broadcasting within the same graph",
                                 true);
}

std::shared_ptr<Config> &Config::cfg()
{
  if (!instance)
    instance = Config::create();

  return instance;
}

void Config::dump() const
{
  LOG->info("Config dump:");

  for (auto &[key, pa] : this->attr)
  {
    std::cout << key << ": " << pa->to_string() << "\n";
  }
}

} // namespace hesiod

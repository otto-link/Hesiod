/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include "nlohmann/json.hpp"

#include "attributes/abstract_attribute.hpp"

#include "hesiod/logger.hpp"

#define CFG hesiod::Config::cfg()

namespace hesiod
{

class Config
{
public:
  // get the singleton instance of the config
  static std::shared_ptr<Config> &cfg();

  template <typename T, typename... Args> void add(const std::string &key, Args &&...args)
  {
    // remove sections for the attribute label
    size_t            pos = key.find_last_of('/');
    const std::string label = key.substr(pos);

    LOG->trace("KEY: {}, LABEL: {}", key, label);

    this->attr[key] = std::make_unique<T>(label, std::forward<Args>(args)...);
  }

  static std::shared_ptr<Config> create()
  {
    return std::shared_ptr<Config>(new Config());
  }

  void dump() const;

  template <typename T> void set(const std::string &key, T new_value)
  {
    this->attr[key]->get_ref<T>->set_value(new_value);
  };

private:
  // private constructor to prevent instantiation
  Config();

  // disable copy constructor and assignment operator
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;

  // static member to hold the single instance
  static std::shared_ptr<Config> instance;

  std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> attr = {};
};

} // namespace hesiod
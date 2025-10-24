/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QApplication>

#include "hesiod/app/app_settings.hpp"

namespace hesiod
{

class AppContext
{
public:
  AppContext() = default;

  // --- Data
  AppSettings app_settings;

private:
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  const std::string ctx_file_path = "hesiod_config.json";
};

} // namespace hesiod
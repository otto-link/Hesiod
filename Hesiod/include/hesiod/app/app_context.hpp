/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QApplication>

#include "hesiod/app/app_settings.hpp"
#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/app/style_settings.hpp"

namespace hesiod
{

struct AppContext
{
  AppContext() = default;

  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  // --- Data
  AppSettings   app_settings;
  StyleSettings style_settings;

  // immutables
  const EnumMappings enum_mappings;
};

} // namespace hesiod
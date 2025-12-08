/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QIcon>

#include "nlohmann/json.hpp"

namespace hesiod
{

// =====================================
// ViewerNodeParam
// =====================================
struct ViewerNodeParam
{
  // defines how the node port data are assigned for graphic representation with this
  // viewer
  std::map<std::string, std::string> port_ids = {};
  std::map<std::string, QIcon>       icons = {};

  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;
};

} // namespace hesiod

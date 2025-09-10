/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/gui/widgets/viewers/viewer_node_param.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

void ViewerNodeParam::json_from(nlohmann::json const &json)
{
  for (auto &[key, value] : port_ids)
  {
    if (json.contains(key))
      value = json.at(key).get<std::string>();
  }
}

nlohmann::json ViewerNodeParam::json_to() const
{
  nlohmann::json j;
  for (auto const &[key, value] : port_ids)
    j[key] = value;
  return j;
}

} // namespace hesiod

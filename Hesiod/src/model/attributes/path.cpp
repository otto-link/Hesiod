/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "hesiod/model/attributes/attributes_highmap.hpp"

namespace hesiod
{

hmap::Path PathAttribute::get() { return value; }

nlohmann::json PathAttribute::json_to() const
{
  nlohmann::json json;
  json["x"] = this->value.get_x();
  json["y"] = this->value.get_y();
  json["v"] = this->value.get_values();
  return json;
}

void PathAttribute::json_from(nlohmann::json const &json)
{
  std::vector<float> x, y, v;
  x = json["x"].get<std::vector<float>>();
  y = json["y"].get<std::vector<float>>();
  v = json["v"].get<std::vector<float>>();
  this->value = hmap::Path(x, y, v);
}

} // namespace hesiod

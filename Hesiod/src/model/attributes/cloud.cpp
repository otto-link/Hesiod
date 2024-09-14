/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes_highmap.hpp"

namespace hesiod
{

hmap::Cloud CloudAttribute::get() { return value; }

nlohmann::json CloudAttribute::json_to() const
{
  nlohmann::json json;
  json["x"] = this->value.get_x();
  json["y"] = this->value.get_y();
  json["v"] = this->value.get_values();
  return json;
}

void CloudAttribute::json_from(nlohmann::json const &json)
{
  std::vector<float> x, y, v;
  x = json["x"].get<std::vector<float>>();
  y = json["y"].get<std::vector<float>>();
  v = json["v"].get<std::vector<float>>();
  this->value = hmap::Cloud(x, y, v);
}

} // namespace hesiod

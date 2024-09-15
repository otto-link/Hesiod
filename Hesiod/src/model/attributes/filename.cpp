/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

FilenameAttribute::FilenameAttribute(std::string value,
                                     std::string filter,
                                     std::string label)
    : Attribute(label), value(value), filter(filter)
{
}

std::string FilenameAttribute::get() { return value; }

nlohmann::json FilenameAttribute::json_to() const
{
  nlohmann::json json = Attribute::json_to();
  json["value"] = this->value;
  json["filter"] = this->filter;
  json["label"] = this->label;
  return json;
}

void FilenameAttribute::json_from(nlohmann::json const &json)
{
  Attribute::json_from(json);
  this->value = json["value"];
  this->filter = json["filter"];
  this->label = json["label"];
}

} // namespace hesiod

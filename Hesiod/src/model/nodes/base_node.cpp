/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/runtime_doc.hpp"

#include <iostream>

namespace hesiod
{

BaseNode::BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config)
    : gnode::Node(label), config(config)
{
  HLOG->trace("BaseNode::BaseNode, label: {}", label);

  // initialize documentation
  this->documentation = nlohmann::json::parse(runtime_doc)[label];
}

gngui::PortType BaseNode::get_port_type(int port_index) const
{
  gnode::PortType ptype = gnode::Node::get_port_type(this->get_port_label(port_index));

  if (ptype == gnode::PortType::IN)
    return gngui::PortType::IN;
  else
    return gngui::PortType::OUT;
}

void BaseNode::json_from(nlohmann::json const &json)
{
  this->set_id(json["id"]);

  for (auto &[key, a] : this->attr)
    if (!json[key].is_null())
      a->json_from(json[key]);
    else
      HLOG->error(
          "BaseNode::json_from, could not deserialize attribute: {}, using default value",
          key);
}

nlohmann::json BaseNode::json_to() const
{
  nlohmann::json json;

  for (auto &[key, a] : this->attr)
    json[key] = a->json_to();

  json["id"] = this->get_id();
  json["label"] = this->get_label();

  return json;
}

} // namespace hesiod

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

  // --- DEBUG ---

  // nlohmann::json doc = this->get_documentation();
  // if (!doc.is_null())
  // {
  //   HLOG->trace(label);
  //   std::cout << doc.dump(4) << std::endl;
  // }

  this->connect(this,
                &BaseNode::compute_started,
                [this]() {
                  HLOG->trace("{}({}), SIGNALS: computed_started",
                              this->get_label(),
                              this->get_id());
                });

  this->connect(this,
                &BaseNode::compute_finished,
                [this]()
                {
                  HLOG->trace("{}({}), SIGNALS: computed_finished",
                              this->get_label(),
                              this->get_id());
                });
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

  Q_EMIT this->node_deserialized(this->get_id());
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

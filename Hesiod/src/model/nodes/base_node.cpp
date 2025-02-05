/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

#ifdef HSD_OS_LINUX
#include "hesiod/model/nodes/runtime_doc.hpp"
#endif

#include <iostream>

namespace hesiod
{

BaseNode::BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config)
    : gnode::Node(label), config(config)
{
  LOG->trace("BaseNode::BaseNode, label: {}", label);

  this->category = get_node_inventory().at(label);

  // initialize documentation
#ifdef HSD_OS_LINUX
  this->documentation = nlohmann::json::parse(runtime_doc)[label];
#endif

  // connections
  this->connect(this,
                &BaseNode::compute_finished,
                [this]()
                {
                  if (this->data_preview)
                    this->data_preview->update_image();
                });
}

gngui::PortType BaseNode::get_port_type(int port_index) const
{
  gnode::PortType ptype = gnode::Node::get_port_type(this->get_port_label(port_index));

  if (ptype == gnode::PortType::IN)
    return gngui::PortType::IN;
  else
    return gngui::PortType::OUT;
}

QWidget *BaseNode::get_qwidget_ref()
{
  if (!this->data_preview)
    this->data_preview = new DataPreview(this);

  return (QWidget *)this->data_preview;
}

void BaseNode::json_from(nlohmann::json const &json)
{
  this->set_id(json["id"]);

  for (auto &[key, a] : this->attr)
    if (!json[key].is_null())
      a->json_from(json[key]);
    else
      LOG->error(
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

nlohmann::json BaseNode::node_parameters_to_json()
{
  nlohmann::json json;

  // --- node info

  json["label"] = this->get_label();
  json["category"] = this->category;
  json["description"] = "TODO";

  // --- ports

  nlohmann::json json_ports = nlohmann::json::array();

  for (int k = 0; k < this->get_nports(); k++)
  {
    nlohmann::json json_this_port;

    std::string stype = this->get_port_type(k) == gngui::PortType::IN ? "input"
                                                                      : "output";

    json_this_port["type"] = stype;
    json_this_port["caption"] = this->get_port_caption(k);
    json_this_port["data_type"] = this->get_data_type(k);
    json_this_port["description"] = "TODO";

    // overwrite data type
    if (this->get_data_type(k) == typeid(hmap::Array).name())
      json_this_port["data_type"] = "Array";
    else if (this->get_data_type(k) == typeid(hmap::Cloud).name())
      json_this_port["data_type"] = "Cloud";
    else if (this->get_data_type(k) == typeid(hmap::Heightmap).name())
      json_this_port["data_type"] = "Heightmap";
    else if (this->get_data_type(k) == typeid(hmap::HeightmapRGBA).name())
      json_this_port["data_type"] = "HeightmapRGBA";
    else if (this->get_data_type(k) == typeid(hmap::Path).name())
      json_this_port["data_type"] = "Path";
    else if (this->get_data_type(k) == typeid(std::vector<hmap::Heightmap>).name())
      json_this_port["data_type"] = "vector<Heightmap>";

    json_ports.push_back(json_this_port);
  }

  json["ports"] = json_ports;

  // --- attributes

  nlohmann::json json_attrs = nlohmann::json::array();

  for (auto &[key, p_attr] : this->attr)
  {
    nlohmann::json json_this_attr;

    json_this_attr["key"] = key;
    json_this_attr["label"] = p_attr->get_label();
    json_this_attr["type"] = attr::attribute_type_map.at(p_attr->get_type());
    json_this_attr["description"] = "TODO";

    json_attrs.push_back(json_this_attr);
  }

  json["attributes"] = json_attrs;

  return json;
}

} // namespace hesiod

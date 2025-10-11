/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <format>
#include <fstream>
#include <unordered_map>

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "attributes/seed_attribute.hpp"

#include "hesiod/config.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

// --- helper

std::string map_type_name(const std::string &typeid_name)
{
  static const std::unordered_map<std::string, std::string> type_name_map = {
      {typeid(hmap::Array).name(), "Array"},
      {typeid(hmap::Cloud).name(), "Cloud"},
      {typeid(hmap::Heightmap).name(), "Heightmap"},
      {typeid(hmap::HeightmapRGBA).name(), "HeightmapRGBA"},
      {typeid(hmap::Path).name(), "Path"},
      {typeid(std::vector<hmap::Heightmap>).name(), "vector<Heightmap>"}};

  auto it = type_name_map.find(typeid_name);
  return (it != type_name_map.end()) ? it->second : typeid_name;
}

// --- class definition

BaseNode::BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config)
    : gnode::Node(label), config(config)
{
  Logger::log()->trace("BaseNode::BaseNode, label: {}", label);

  this->category = get_node_inventory().at(label);

  // initialize documentation
  const std::string doc_path = HSD_NODE_DOCUMENTATION_JSON_PATH;
  nlohmann::json    json;

  // loading data
  try
  {
    std::ifstream file(doc_path);

    if (!file.is_open())
    {
      Logger::log()->error("Could not open documentation file: {}", doc_path);
      throw std::runtime_error("Documentation file not found");
    }

    file >> json;
    Logger::log()->trace("JSON successfully loaded from {}", doc_path);
  }
  catch (const std::exception &e)
  {
    Logger::log()->error("Error loading documentation: {}", e.what());
    json = nlohmann::json::object(); // Create empty JSON to prevent crashes
  }

  // safely load documentation
  if (json.contains(label) && json[label].is_object())
  {
    this->documentation = json[label];
  }
  else
  {
    Logger::log()->warn("Missing documentation for node: {}", label);
    this->documentation = nlohmann::json::object();
  }

  // connections
  this->connect(this,
                &BaseNode::compute_finished,
                [this]()
                {
                  if (this->data_preview)
                    this->data_preview->update_preview();
                });
}

std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> *BaseNode::get_attr_ref()
{
  return &this->attr;
};

std::vector<std::string> *BaseNode::get_attr_ordered_key_ref()
{
  return &this->attr_ordered_key;
};

DataPreview *BaseNode::get_data_preview_ref() { return this->data_preview; }

std::string BaseNode::get_documentation_html() const
{
  std::string html;

  try
  {
    html += std::format("<h1>{} Node</h1>", this->get_label());

    // category and description
    if (this->documentation.contains("category"))
    {
      html += std::format("<p><b>Categories: {}</b></p>",
                          this->documentation["category"].get<std::string>());
    }

    html += std::format(
        "<p>{}</p>",
        this->documentation.value("description", "No description available"));

    // ports table
    html += "<h2>Ports</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='5'>"
            "<tr><th>Name</th><th>I/O</th><th>Data Type</th><th>Description</th></tr>";

    if (this->documentation.contains("ports") && this->documentation["ports"].is_object())
    {
      for (const auto &[key, port] : this->documentation["ports"].items())
      {
        html += std::format(
            "<tr><td><b>{}</b></td><td>{}</td><td>{}</td><td>{}</td></tr>",
            port.value("caption", key),
            port.value("type", "Unknown"),
            port.value("data_type", "Unknown"),
            port.value("description", "No description"));
      }
    }
    html += "</table>";

    // parameters table
    html += "<h2>Parameters</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='5'>"
            "<tr><th>Name</th><th>Type</th><th>Description</th></tr>";

    if (this->documentation.contains("parameters") &&
        this->documentation["parameters"].is_object())
    {
      for (const auto &[key, param] : this->documentation["parameters"].items())
      {
        html += std::format("<tr><td><b>{}</b></td><td>{}</td><td>{}</td></tr>",
                            param.value("label", key),
                            param.value("type", "Unknown"),
                            param.value("description", "No description"));
      }
    }
    html += "</table>";
  }
  catch (const std::exception &e)
  {
    Logger::log()->error(
        "BaseNode::get_documentation_html: Error generating documentation HTML: {}",
        e.what());
    html = "<p>Error generating documentation</p>";
  }

  return html;
}

std::string BaseNode::get_documentation_short() const
{
  std::string str;
  size_t      width = 64;

  try
  {
    str += std::format("NODE TYPE: {}", this->get_label());
    str += "\n\n";

    // category
    if (this->documentation.contains("category"))
    {
      str += std::format("CATEGORIES: {}",
                         this->documentation["category"].get<std::string>());
      str += "\n\n";
    }

    // description
    std::string description = this->documentation.value("description",
                                                        "No description available");
    description = insert_char_every_nth(description, width, "\n");
    str += "DESCRIPTION:\n" + description;
    str += "\n\n";

    // ports
    if (this->documentation.contains("ports") && this->documentation["ports"].is_object())
    {
      str += "PORTS:\n";

      for (const auto &[key, port] : this->documentation["ports"].items())
      {
        std::string port_description = port.value("description", "No description");
        port_description = insert_char_every_nth(port_description, width, "\n");

        str += std::format("- {} / {} / {}\n{}\n",
                           port.value("caption", key),
                           port.value("type", "Unknown"),
                           port.value("data_type", "Unknown"),
                           port_description);
      }
    }
  }
  catch (const std::exception &e)
  {
    Logger::log()->error(
        "BaseNode::get_documentation_short: Error generating documentation HTML: {}",
        e.what());
    str = "Error generating documentation";
  }

  return str;
}

GraphEditor *BaseNode::get_p_graph_node() const
{
  if (!this->p_graph_node)
  {
    Logger::log()->warn("BaseNode::get_p_graph_node: p_graph_node is nullptr");
  }

  return this->p_graph_node;
}

void BaseNode::json_from(nlohmann::json const &json)
{
  try
  {
    this->set_id(json.value("id", this->get_id()));

    for (auto &[key, attr] : this->attr)
    {
      if (json.contains(key))
        attr->json_from(json[key]);
      else
        Logger::log()->warn("Missing JSON key for attribute: {}, using default", key);
    }
  }
  catch (const nlohmann::json::exception &e)
  {
    Logger::log()->error("BaseNode::json_from: JSON parsing error: {}", e.what());
  }
}

nlohmann::json BaseNode::json_to() const
{
  nlohmann::json json;

  try
  {
    for (const auto &[key, attr] : this->attr)
    {
      json[key] = attr->json_to();
    }
    json["id"] = this->get_id();
    json["label"] = this->get_label();
  }
  catch (const std::exception &e)
  {
    Logger::log()->error("BaseNode::json_to: Error serializing node to JSON: {}",
                         e.what());
  }
  return json;
}

nlohmann::json BaseNode::node_parameters_to_json() const
{
  nlohmann::json json;

  try
  {
    // Basic node info
    json["label"] = this->get_label();
    json["category"] = this->category;
    json["description"] = this->documentation.value("description",
                                                    "No description available");

    // Port information
    nlohmann::json ports_json;
    for (int k = 0; k < this->get_nports(); k++)
    {
      nlohmann::json    port_info;
      const std::string caption = this->get_port_caption(k);

      port_info["type"] = (get_port_type(k) == gngui::PortType::IN) ? "input" : "output";
      port_info["caption"] = caption;
      port_info["data_type"] = map_type_name(this->get_data_type(k));

      auto json_ptr = nlohmann::json::json_pointer("/ports/" + caption + "/description");
      port_info["description"] = this->documentation.value(json_ptr, "No description");

      ports_json[caption] = port_info;
    }
    json["ports"] = ports_json;

    // Attribute information
    nlohmann::json params_json;
    for (const auto &[key, attr] : this->attr)
    {
      nlohmann::json param_info;
      param_info["key"] = key;
      param_info["label"] = attr->get_label();
      param_info["type"] = attr::attribute_type_map.at(attr->get_type());

      auto json_ptr = nlohmann::json::json_pointer("/parameters/" + key + "/description");
      param_info["description"] = this->documentation.value(json_ptr, "No description");

      params_json[key] = param_info;
    }
    json["parameters"] = params_json;
  }
  catch (const std::exception &e)
  {
    Logger::log()->error(
        " BaseNode::node_parameters_to_json: Error generating node parameters JSON: {}",
        e.what());
  }

  return json;
}

void BaseNode::reseed()
{
  for (const auto &[key, attr] : this->attr)
    if (attr && attr->get_type() == attr::AttributeType::SEED)
      if (auto p_seed = attr->get_ref<attr::SeedAttribute>())
      {
        Logger::log()->trace("BaseNode::reseed: reseeding node {}_{}",
                             this->get_label(),
                             this->get_id());

        p_seed->set_value(p_seed->get_value() + 1);
      }
}

void BaseNode::set_attr_ordered_key(const std::vector<std::string> &new_attr_ordered_key)
{
  this->attr_ordered_key = new_attr_ordered_key;
}

void BaseNode::set_compute_fct(std::function<void(BaseNode *p_node)> new_compute_fct)
{
  this->compute_fct = std::move(new_compute_fct);
}

void BaseNode::set_qwidget_fct(std::function<QWidget *(BaseNode *p_node)> new_qwidget_fct)
{
  this->qwidget_fct = std::move(new_qwidget_fct);
}

void BaseNode::update_attributes_tool_tip()
{
  Logger::log()->trace("BaseNode::update_attributes_tool_tip");

  size_t width = 64;

  for (auto &[key, sp_attr] : this->attr)
    if (sp_attr)
    {
      std::string label = sp_attr->get_label();

      if (this->documentation.contains("parameters") &&
          this->documentation["parameters"].contains(key))
      {
        std::string description = label + ":\n";

        if (this->documentation["parameters"][key].contains("description"))
        {
          std::string base_desc = this->documentation["parameters"][key]["description"];
          base_desc = insert_char_every_nth(base_desc, width, "\n");
          description += base_desc;
        }

        sp_attr->set_description(description);
      }
    }
}

} // namespace hesiod

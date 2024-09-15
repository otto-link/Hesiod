/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file base_node.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <QObject>

#include "nlohmann/json.hpp"

#include "gnode/node.hpp"
#include "gnodegui/node_proxy.hpp"

#include "hesiod/model/attributes/attributes.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class BaseNode : public QObject, public gnode::Node, public gngui::NodeProxy
{
  Q_OBJECT

public:
  BaseNode() = default;

  BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config);

  template <typename AttributeType>
  auto get_attr(const std::string &key) -> decltype(auto)
  {
    return this->attr.at(key)->get_ref<AttributeType>()->get();
  }

  std::string get_category() const override { return this->category; }

  nlohmann::json get_documentation() const { return this->documentation; }

  std::string get_id() const override { return gnode::Node::get_id(); }

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

  void set_id(const std::string &new_id) override { gnode::Node::set_id(new_id); }

  //--------------------
  // NodeProxy wrapper - fetches the infos necessary to build up the node GUI
  //--------------------

  std::string get_caption() const override { return this->get_label(); }

  std::string get_data_type(int port_index) const override
  {
    return gnode::Node::get_data_type(port_index);
  }

  int get_nports() const override { return gnode::Node::get_nports(); }

  std::string get_port_caption(int port_index) const override
  {
    return gnode::Node::get_port_label(port_index);
  };

  gngui::PortType get_port_type(int port_index) const override;

Q_SIGNALS:
  void compute_started(std::string id);

  void compute_finished(std::string id);

  void node_deserialized(std::string id);

protected:
  std::map<std::string, std::unique_ptr<Attribute>> attr = {};

  /**
   * @brief Defines a preferred display order of the attributes.
   */
  std::vector<std::string> attr_ordered_key = {};

  std::string category;

private:
  std::shared_ptr<ModelConfig> config;

  nlohmann::json documentation;
};

} // namespace hesiod
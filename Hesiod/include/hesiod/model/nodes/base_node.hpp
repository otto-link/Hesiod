/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file base_node.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <functional>

#include <QObject>

#include "gnode/node.hpp"
#include "gnodegui/node_proxy.hpp"

#include "attributes/abstract_attribute.hpp"

#include "hesiod/gui/widgets/data_preview.hpp"
#include "hesiod/model/model_config.hpp"

// clang-format off
#define ADD_ATTR(aclass, key, ...) p_node->add_attr<aclass>(key, key, ## __VA_ARGS__)

#define GET(key, aclass) p_node->get_attr_ref()->at(key)->get_ref<attr::aclass>()->get_value()
#define GET_ATTR(key, aclass, what) p_node->get_attr_ref()->at(key)->get_ref<attr::aclass>()->get_ ## what()
#define GET_REF(key, aclass) p_node->get_attr_ref()->at(key)->get_ref<attr::aclass>()

#define CONFIG p_node->get_config_ref()->shape, p_node->get_config_ref()->tiling, p_node->get_config_ref()->overlap
// clang-format on

namespace hesiod
{

class GraphEditor; // forward

class BaseNode : public QWidget, public gnode::Node, public gngui::NodeProxy
{
  Q_OBJECT

public:
  BaseNode() = default;

  BaseNode(const std::string &label, std::shared_ptr<ModelConfig> config);

  template <typename T, typename... Args>
  void add_attr(const std::string &key, Args &&...args)
  {
    this->attr[key] = std::make_unique<T>(std::forward<Args>(args)...);
  }

  void compute() override { this->compute_fct(this); }

  template <typename T> auto get_attr(const std::string &key) const -> decltype(auto)
  {
    return this->attr.at(key)->get_ref<T>()->get_value();
  }

  nlohmann::json node_parameters_to_json() const;

  std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> *get_attr_ref()
  {
    return &this->attr;
  };

  std::vector<std::string> *get_attr_ordered_key_ref()
  {
    return &this->attr_ordered_key;
  };

  std::string get_category() const override { return this->category; }

  ModelConfig *get_config_ref() { return this->config.get(); }

  nlohmann::json get_documentation() const { return this->documentation; }

  std::string get_documentation_html() const;

  std::string get_id() const override { return gnode::Node::get_id(); }

  virtual void json_from(nlohmann::json const &json);

  virtual nlohmann::json json_to() const;

  void set_attr_ordered_key(const std::vector<std::string> &new_attr_ordered_key)
  {
    this->attr_ordered_key = new_attr_ordered_key;
  }

  void set_compute_fct(std::function<void(BaseNode *p_node)> new_compute_fct)
  {
    this->compute_fct = std::move(new_compute_fct);
  }

  void set_id(const std::string &new_id) override { gnode::Node::set_id(new_id); }

  //>>> NodeProxy wrapper - fetches the infos necessary to build up the node GUI

  std::string get_caption() const override { return this->get_label(); }

  void *get_data_ref(int port_index) override
  {
    return this->get_value_ref_void(port_index);
  }

  std::string get_data_type(int port_index) const override
  {
    return gnode::Node::get_data_type(port_index);
  }

  int get_nports() const override { return gnode::Node::get_nports(); }

  GraphEditor *get_p_graph_node() const { return this->p_graph_node; }

  std::string get_port_caption(int port_index) const override
  {
    return gnode::Node::get_port_label(port_index);
  };

  gngui::PortType get_port_type(int port_index) const override;

  virtual QWidget *get_qwidget_ref() override;

  //<<< NodeProxy wrapper end

Q_SIGNALS:
  void compute_finished(const std::string &id);

  void compute_started(const std::string &id);

private:
  std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> attr = {};

  /**
   * @brief Defines a preferred display order of the attributes.
   */
  std::vector<std::string> attr_ordered_key = {};

  std::string category;

  std::shared_ptr<ModelConfig> config;

  // keep track of the belonging graph node
  GraphEditor *p_graph_node;

  nlohmann::json documentation;

  std::function<void(BaseNode *p_node)> compute_fct = 0;

  // ownership of this pointer will be taken by the gngui::GraphicsNode instance
  DataPreview *data_preview = nullptr;
};

} // namespace hesiod
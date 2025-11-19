/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <chrono>
#include <functional>

#include <QObject>

#include "gnode/node.hpp"
#include "gnodegui/node_proxy.hpp"

#include "attributes/abstract_attribute.hpp"

#include "hesiod/gui/widgets/data_preview.hpp"
#include "hesiod/model/graph/graph_config.hpp"
#include "hesiod/model/nodes/node_runtime_info.hpp"

// clang-format off
#define CONFIG(obj) obj.get_config_ref()->shape, obj.get_config_ref()->tiling, obj.get_config_ref()->overlap
// clang-format on

namespace hesiod
{

class GraphNode; // forward

// helper
std::string map_type_name(const std::string &typeid_name);

// =====================================
// BaseNode
// =====================================
class BaseNode : public QWidget,
                 public gnode::Node,
                 public gngui::NodeProxy,
                 public std::enable_shared_from_this<BaseNode>
{
  Q_OBJECT

public:
  // --- Constructors ---
  BaseNode() = default;
  BaseNode(const std::string &label, std::weak_ptr<GraphConfig> config);
  ~BaseNode();

  // --- Configuration ---
  std::shared_ptr<const GraphConfig> get_config_ref() const;

  // --- Runtime info ---
  NodeRuntimeInfo get_runtime_info() const;
  float           get_memory_usage() const;
  void            update_runtime_info(NodeRuntimeStep step);

  // --- Identification ---
  std::string get_id() const override;
  void        set_id(const std::string &new_id) override;
  std::string get_category() const override;
  void        set_comment(const std::string &new_comment);
  std::string get_node_type() const;

  // --- Compute ---
  void compute() override { this->compute_fct(*this); }
  void set_compute_fct(std::function<void(BaseNode &node)> new_compute_fct);
  void set_qwidget_fct(std::function<QWidget *(BaseNode &node)> new_qwidget_fct);

  // --- Serialization ---
  virtual void           json_from(nlohmann::json const &json);
  virtual nlohmann::json json_to() const;
  nlohmann::json         node_parameters_to_json() const;

  // --- Documentation ---
  nlohmann::json get_documentation() const;
  std::string    get_documentation_html() const;
  std::string    get_documentation_short() const;
  void           update_attributes_tool_tip();

  // --- NodeProxy Interface (for GUI) ---
  std::string      get_caption() const override;
  std::string      get_comment() const override;
  DataPreview     *get_data_preview_ref();
  void            *get_data_ref(int port_index) override;
  std::string      get_data_type(int port_index) const override;
  int              get_nports() const override;
  std::string      get_port_caption(int port_index) const override;
  gngui::PortType  get_port_type(int port_index) const override;
  virtual QWidget *get_qwidget_ref() override;
  std::string      get_tool_tip_text() override;

  // --- Editor Connection ---
  // GraphNode *get_graph_node_ref() const;

  // --- Attribute Management ---
  template <typename T, typename... Args>
  void add_attr(const std::string &key, Args &&...args)
  {
    this->attr[key] = std::make_unique<T>(std::forward<Args>(args)...);
  }

  template <typename T> auto get_attr(const std::string &key) const -> decltype(auto)
  {
    return this->attr.at(key)->get_ref<T>()->get_value();
  }

  template <typename T> T *get_attr_ref(const std::string &key) const
  {
    return this->attr.at(key)->get_ref<T>();
  }

  std::vector<std::string> *get_attr_ordered_key_ref();
  std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> *get_attributes_ref();
  void set_attr_ordered_key(const std::vector<std::string> &new_attr_ordered_key);

  void reseed(bool backward);

  // --- Signals ---
signals:
  void compute_finished(const std::string &id);
  void compute_started(const std::string &id);

private:
  // --- Members ---
  std::map<std::string, std::unique_ptr<attr::AbstractAttribute>> attr = {};

  std::vector<std::string>                 attr_ordered_key = {};
  std::string                              category;
  std::string                              comment;
  std::weak_ptr<GraphConfig>               config; // owned by GraphNode
  nlohmann::json                           documentation;
  NodeRuntimeInfo                          runtime_info;
  std::function<void(BaseNode &node)>      compute_fct = nullptr;
  std::function<QWidget *(BaseNode &node)> qwidget_fct = nullptr;
  DataPreview *data_preview = nullptr; // owned by gngui::GraphicsNode
};

// =====================================
// Node-related enums
// =====================================
enum BlendingMethod : int
{
  ADD,
  EXCLUSION_BLEND,
  GRADIENTS,
  MAXIMUM,
  MAXIMUM_SMOOTH,
  MINIMUM,
  MINIMUM_SMOOTH,
  MULTIPLY,
  MULTIPLY_ADD,
  NEGATE,
  OVERLAY,
  REPLACE,
  SOFT,
  SUBSTRACT,
};

enum ExportFormat : int
{
  PNG8BIT,
  PNG16BIT,
  RAW16BIT,
};

enum MaskCombineMethod : int
{
  UNION,
  INTERSECTION,
  EXCLUSION,
};

} // namespace hesiod
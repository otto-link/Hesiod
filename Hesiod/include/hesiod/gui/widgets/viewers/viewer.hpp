/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QComboBox>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "hesiod/gui/widgets/viewers/viewer_node_param.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

enum ViewerType : int
{
  VIEWER3D,
};

static std::map<ViewerType, std::string> viewer_type_as_string = {
    {ViewerType::VIEWER3D, "Render 3D"}};

// =====================================
// Viewer (abstract)
// =====================================
class Viewer : public QWidget
{
  Q_OBJECT

public:
  Viewer() = delete;
  Viewer(QPointer<GraphNodeWidget> p_graph_node_widget_,
         ViewerType                viewer_type_ = ViewerType::VIEWER3D,
         const std::string        &label_ = "Viewer",
         QWidget                  *parent = nullptr);

  virtual void clear();
  void         set_current_node_id(const std::string &new_id);
  virtual void setup_connections();
  virtual void setup_layout();
  void         update_widgets();

  // --- Serialization ---
  virtual void           json_from(nlohmann::json const &json);
  virtual nlohmann::json json_to() const;

Q_SIGNALS:
  void clear_view_request();
  void current_node_id_changed(const std::string &new_id);
  void node_pinned(const std::string &node_id, bool state);
  void widget_close();

public Q_SLOTS:
  void on_node_deleted(const std::string &new_id);
  void on_node_deselected(const std::string &new_id);
  void on_node_pinned_changed();
  void on_node_selected(const std::string &new_id);

protected:
  void      closeEvent(QCloseEvent *event) override;
  BaseNode *safe_get_node();

  // --- Should be pure virtual but kept to avoid UB ---
  virtual ViewerNodeParam get_default_view_param() const;
  virtual void            update_renderer();

  // --- Members ---
  QPointer<GraphNodeWidget>              p_graph_node_widget;
  ViewerType                             viewer_type;
  std::string                            label;
  std::string                            current_node_id = "";
  ViewerNodeParam                        view_param;
  std::map<std::string, ViewerNodeParam> view_param_map; // storage per node id
  std::map<std::string, QComboBox *>     combo_map;
  bool                                   prevent_combo_connections = false;
  bool                                   is_node_pinned = false;
  QPushButton                           *button_pin_current_node;
};

// helper
void wild_guess_view_param(ViewerNodeParam  &view_param,
                           const ViewerType &viewer_type,
                           const BaseNode   &node);

} // namespace hesiod

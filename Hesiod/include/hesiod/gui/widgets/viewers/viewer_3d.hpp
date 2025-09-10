/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QComboBox>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "hesiod/gui/widgets/viewers/viewer_node_param.hpp"

#include "qtr/render_widget.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

// =====================================
// Viewer3D
// =====================================
class Viewer3D : public QWidget
{
  Q_OBJECT

public:
  Viewer3D() = delete;
  Viewer3D(GraphNodeWidget *p_graph_node_widget_, QWidget *parent = nullptr);

  void            clear();
  ViewerNodeParam get_default_view_param() const;
  void            setup_connections();
  void            setup_layout();
  void            update_combos();
  void            update_renderer();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

public Q_SLOTS:
  void on_current_node_id_changed(const std::string &new_id);

private:
  GraphNodeWidget                       *p_graph_node_widget;
  std::string                            current_node_id = "";
  ViewerNodeParam                        view_param;
  std::map<std::string, ViewerNodeParam> view_param_map; // storage per node id
  qtr::RenderWidget                     *terrain_renderer;
  std::map<std::string, QComboBox *>     combo_map;
  bool                                   prevent_renderer_update = false;
};

} // namespace hesiod
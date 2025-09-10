/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QComboBox>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "qtr/render_widget.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

// =====================================
// Viewer3DNodeParam
// =====================================
struct Viewer3DNodeParam
{
  // defines how the node port data are assigned for graphic representation with this
  // viewer
  std::map<std::string, std::string> port_ids = {{"elevation", ""},
                                                 {"color", ""},
                                                 {"normal_map", ""},
                                                 {"cloud", ""},
                                                 {"path", ""},
                                                 {"trees", ""},
                                                 {"rocks", ""}};

  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;
};

// =====================================
// Viewer3D
// =====================================
class Viewer3D : public QWidget
{
  Q_OBJECT

public:
  Viewer3D() = delete;
  Viewer3D(GraphNodeWidget *p_graph_node_widget_, QWidget *parent = nullptr);

  void clear();
  void setup_connections();
  void setup_layout();
  void update_combos();
  void update_renderer();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

public Q_SLOTS:
  void on_current_node_id_changed(const std::string &new_id);

private:
  GraphNodeWidget                         *p_graph_node_widget;
  std::string                              current_node_id = "";
  Viewer3DNodeParam                        view_param;
  std::map<std::string, Viewer3DNodeParam> view_param_map; // storage per node id
  qtr::RenderWidget                       *terrain_renderer;
  std::map<std::string, QComboBox *>       combo_map;
  bool                                     prevent_renderer_update = false;
};

} // namespace hesiod
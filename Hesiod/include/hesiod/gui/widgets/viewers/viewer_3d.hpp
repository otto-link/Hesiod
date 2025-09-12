/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QComboBox>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "qtr/render_widget.hpp"

#include "hesiod/gui/widgets/viewers/viewer.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

// =====================================
// Viewer3D
// =====================================
class Viewer3D : public Viewer
{
  Q_OBJECT

public:
  Viewer3D() = delete;
  Viewer3D(GraphNodeWidget   *p_graph_node_widget_,
           const std::string &label_ = "Viewer",
           QWidget           *parent = nullptr);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

  void clear() override;
  void setup_layout() override;

private:
  ViewerNodeParam get_default_view_param() const override;
  void            update_renderer() override;

  ViewerType         viewer_type;
  qtr::RenderWidget *p_renderer;
};

} // namespace hesiod
/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"
#include "highmap/range.hpp"

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"

namespace hesiod
{

Viewer3D::Viewer3D(GraphNodeWidget   *p_graph_node_widget_,
                   const std::string &label_,
                   QWidget           *parent)
    : Viewer(p_graph_node_widget_, ViewerType::VIEWER3D, "3D Renderer", parent)
{
  LOG->trace("Viewer3D::Viewer3D");

  this->view_param = this->get_default_view_param();
  this->setup_layout();
  this->update_widgets();
  this->setup_connections();
}

void Viewer3D::clear()
{
  Viewer::clear();
  // if (this->p_renderer)
  //   this->p_renderer->clear();
}

ViewerNodeParam Viewer3D::get_default_view_param() const
{
  ViewerNodeParam wp;

  wp.port_ids = {{"elevation", ""},
                 {"color", ""},
                 {"normal_map", ""},
                 {"points", ""},
                 {"path", ""},
                 {"trees", ""},
                 {"rocks", ""}};

  return wp;
}

void Viewer3D::setup_layout()
{
  LOG->trace("Viewer3D::setup_layout");

  Viewer::setup_layout();

  // retrieve layout and add specific widget
  QGridLayout *grid = dynamic_cast<QGridLayout *>(this->layout());

  int col_count = get_column_count(grid);
  int row_count = get_row_count(grid);

  // add viewer
  this->p_renderer = new qtr::RenderWidget("", this);
  grid->addWidget(dynamic_cast<QWidget *>(p_renderer), row_count, 0, 1, col_count);
}

void Viewer3D::update_renderer()
{
  LOG->trace("Viewer3D::update_renderer");

  // --- early exit cases

  if (!this->p_renderer)
  {
    LOG->error("Viewer3D::update_renderer: renderer reference is a dangling ptr");
    this->p_renderer->clear();
    return;
  }

  if (this->current_node_id == "")
  {
    this->p_renderer->clear();
    return;
  }

  BaseNode *p_node = this->safe_get_node();

  if (!p_node)
  {
    this->p_renderer->clear();
    return;
  }

  // --- route/send data to renderer

  // elevation
  {
    bool reset_heightmap = true;

    if (const auto &elevation_port_id = this->view_param.port_ids.at("elevation");
        !elevation_port_id.empty())
    {
      int pid = p_node->get_port_index(elevation_port_id);

      if (p_node->get_data_type(pid) == typeid(hmap::Heightmap).name())
        if (auto *p_h = p_node->get_value_ref<hmap::Heightmap>(pid))
        {
          hmap::Array array = p_h->to_array();

          this->p_renderer->set_heightmap_geometry(array.vector,
                                                   p_h->shape.x,
                                                   p_h->shape.y);
          reset_heightmap = false;
        }
    }

    if (reset_heightmap)
      this->p_renderer->reset_heightmap_geometry();
  }

  // points
  {
    bool reset_points = true;

    if (const auto &elevation_port_id = this->view_param.port_ids.at("points");
        !elevation_port_id.empty())
    {
      int pid = p_node->get_port_index(elevation_port_id);

      if (p_node->get_data_type(pid) == typeid(hmap::Cloud).name())
        if (auto *p_c = p_node->get_value_ref<hmap::Cloud>(pid))
        {
          this->p_renderer->set_points(p_c->get_x(), p_c->get_y(), p_c->get_values());
          reset_points = false;
        }
    }

    if (reset_points)
      this->p_renderer->reset_points();
  }
}

} // namespace hesiod

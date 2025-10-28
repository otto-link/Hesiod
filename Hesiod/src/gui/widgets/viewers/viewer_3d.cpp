/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/morphology.hpp"
#include "highmap/range.hpp"

#include "qtr/render_widget.hpp"

#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/viewers/render_helpers.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"

namespace hesiod
{

// 1 port
template <typename T, typename F>
bool helper_try_set_from_port(const BaseNode       &node,
                              const std::string    &port_name,
                              const std::type_info &type,
                              F                   &&fn)
{
  if (port_name.empty())
    return false;

  int pid = node.get_port_index(port_name);

  if (pid < 0)
    return false;
  if (node.get_data_type(pid) != type.name())
    return false;

  T *ptr = node.get_value_ref<T>(pid);

  if (!ptr)
    return false;

  fn(*ptr);
  return true;
}

// 2 ports
template <typename T, typename F>
bool helper_try_set_from_port(const BaseNode       &node,
                              const std::string    &port_name1,
                              const std::string    &port_name2,
                              const std::type_info &type,
                              F                   &&fn)
{
  if (port_name1.empty() || port_name2.empty())
    return false;

  int pid1 = node.get_port_index(port_name1);
  if (pid1 < 0)
    return false;
  if (node.get_data_type(pid1) != type.name())
    return false;

  int pid2 = node.get_port_index(port_name2);
  if (pid2 < 0)
    return false;
  if (node.get_data_type(pid2) != type.name())
    return false;

  T *ptr1 = node.get_value_ref<T>(pid1);
  T *ptr2 = node.get_value_ref<T>(pid2);

  if (!ptr1 || !ptr2)
    return false;

  fn(*ptr1, *ptr2);
  return true;
}

Viewer3D::Viewer3D(GraphNodeWidget *p_graph_node_widget_, QWidget *parent)
    : Viewer(p_graph_node_widget_, ViewerType::VIEWER3D, "3D Renderer", parent)
{
  Logger::log()->trace("Viewer3D::Viewer3D");

  this->view_param = this->get_default_view_param();
  this->setup_layout();
  this->update_widgets();
  this->setup_connections();
}

void Viewer3D::clear()
{
  Viewer::clear();
  if (this->p_renderer)
    this->p_renderer->clear();
}

ViewerNodeParam Viewer3D::get_default_view_param() const
{
  ViewerNodeParam wp;

  wp.port_ids = {
      {"elevation", ""},
      {"water_depth", ""},
      {"color", ""},
      {"normal_map", ""},
      {"points", ""},
      {"path", ""},
      // {"trees", ""},
      // {"rocks", ""},
  };

  return wp;
}

void Viewer3D::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("Viewer3D::json_from");

  Viewer::json_from(json);
  if (p_renderer)
    p_renderer->json_from(json["renderer"]);
}

nlohmann::json Viewer3D::json_to() const
{
  Logger::log()->trace("Viewer3D::json_to");

  nlohmann::json json = Viewer::json_to();
  if (p_renderer)
    json["renderer"] = p_renderer->json_to();

  return json;
}

void Viewer3D::setup_layout()
{
  Logger::log()->trace("Viewer3D::setup_layout");

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
  Logger::log()->trace("Viewer3D::update_renderer");

  // --- early exit cases

  if (!this->p_renderer)
  {
    Logger::log()->error(
        "Viewer3D::update_renderer: renderer reference is a dangling ptr");
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

  bool flip_y = false;

  // elevation
  if (!helper_try_set_from_port<hmap::Heightmap>(
          *p_node,
          this->view_param.port_ids.at("elevation"),
          typeid(hmap::Heightmap),
          [this](const hmap::Heightmap &h)
          {
            auto arr = h.to_array();
            if (this->p_renderer)
              this->p_renderer->set_heightmap_geometry(arr.vector, h.shape.x, h.shape.y);
          }))
  {
    this->p_renderer->reset_heightmap_geometry();
  }

  // water
  if (!helper_try_set_from_port<hmap::Heightmap>(
          *p_node,
          this->view_param.port_ids.at("elevation"),
          this->view_param.port_ids.at("water_depth"),
          typeid(hmap::Heightmap),
          [this](const hmap::Heightmap &h, const hmap::Heightmap &w)
          {
            // TODO do this somewhere else?
            auto ah = h.to_array(); // elevation
            auto aw = w.to_array(); // water depth

            ah += aw;

            // extend the water depth by one-cell to avoid truncated
            // cells at the interface water/ground
            float hmin = ah.min();

            for (int j = 0; j < h.shape.y; ++j)
              for (int i = 0; i < h.shape.x; ++i)
              {
                if (aw(i, j) <= 0.f)
                  ah(i, j) = 0.f;
                else
                  ah(i, j) += hmin;
              }

            ah = hmap::dilation_expand_border_only(ah, 1);

            // remove non-water cells
            float cut_value = -1e3f;

            for (int j = 0; j < h.shape.y; ++j)
              for (int i = 0; i < h.shape.x; ++i)
              {
                if (ah(i, j) <= 0.f)
                  ah(i, j) = cut_value;
                else
                  ah(i, j) -= hmin;
              }

            // send to renderer
            if (this->p_renderer)
              this->p_renderer->set_water_geometry(ah.vector,
                                                   h.shape.x,
                                                   h.shape.y,
                                                   cut_value);
          }))
  {
    this->p_renderer->reset_water_geometry();
  }

  // color
  if (!(helper_try_set_from_port<hmap::Heightmap>(
            *p_node,
            this->view_param.port_ids.at("color"),
            typeid(hmap::Heightmap),
            [this](const hmap::Heightmap &h)
            {
              auto arr = h.to_array();
              auto img = generate_selector_image(arr);

              if (this->p_renderer)
                this->p_renderer->set_texture(QTR_TEX_ALBEDO, img, h.shape.x);
            }) ||
        helper_try_set_from_port<hmap::HeightmapRGBA>(
            *p_node,
            this->view_param.port_ids.at("color"),
            typeid(hmap::HeightmapRGBA),
            [this, flip_y](const hmap::HeightmapRGBA &rgba)
            {
              auto img = rgba.to_img_8bit(rgba.shape, flip_y);

              if (this->p_renderer)
                this->p_renderer->set_texture(QTR_TEX_ALBEDO, img, rgba.shape.x);
            })))
  {
    this->p_renderer->reset_texture(QTR_TEX_ALBEDO);
  }

  // normal map
  if (!helper_try_set_from_port<hmap::HeightmapRGBA>(
          *p_node,
          this->view_param.port_ids.at("normal_map"),
          typeid(hmap::HeightmapRGBA),
          [this, flip_y](const hmap::HeightmapRGBA &rgba)
          {
            auto img = rgba.to_img_8bit(rgba.shape, flip_y);

            if (this->p_renderer)
              this->p_renderer->set_texture(QTR_TEX_NORMAL, img, rgba.shape.x);
          }))
  {
    this->p_renderer->reset_texture(QTR_TEX_NORMAL);
  }

  // points
  if (!helper_try_set_from_port<hmap::Cloud>(
          *p_node,
          this->view_param.port_ids.at("points"),
          typeid(hmap::Cloud),
          [this](const hmap::Cloud &c)
          {
            if (this->p_renderer)
              this->p_renderer->set_points(c.get_x(), c.get_y(), c.get_values());
          }))
  {
    this->p_renderer->reset_points();
  }

  // path
  if (!helper_try_set_from_port<hmap::Path>(
          *p_node,
          this->view_param.port_ids.at("path"),
          typeid(hmap::Path),
          [this](const hmap::Path &c)
          {
            if (this->p_renderer)
              this->p_renderer->set_path(c.get_x(), c.get_y(), c.get_values());
          }))
  {
    this->p_renderer->reset_path();
  }
}

} // namespace hesiod

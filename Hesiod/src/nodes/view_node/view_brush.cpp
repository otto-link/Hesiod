/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBrush::ViewBrush(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::Brush(id, shape, tiling, overlap)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewBrush::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  bool was_drawing = this->edit_state.is_drawing;

  if (!was_drawing)
    this->edit_state.last_mouse_pos = {std::numeric_limits<float>::max(),
                                       std::numeric_limits<float>::max()};

  if (this->edit_state.is_initted)
  {
    if (!this->edit_state.pending_changes.empty())
    {
      edit_state.apply_pending_changes();
      sync_drawing_texture();
    }
  }
  else
  {
    this->edit_state.pending_hm = hmap::HeightMap();
    this->edit_state.pending_hm.set_sto(this->value_out.shape,
                                        this->value_out.tiling,
                                        this->value_out.overlap);
    this->edit_state.is_initted = true;
  }

  if (ImGui::SliderFloat("max height",
                         &this->edit_state.max_height,
                         0.1f,
                         2.0f,
                         "%.1f"))
  {
    hmap::transform(this->edit_state.pending_hm,
                    [this](hmap::Array &m)
                    {
                      hmap::clamp_max_smooth(
                          m,
                          this->edit_state.max_height,
                          this->edit_state.k_clamp_smoothing);
                    });
    this->edit_state.brush_strength = std::min(this->edit_state.brush_strength,
                                               this->edit_state.max_height);
    sync_drawing_texture();
  }
  ImGui::SliderFloat("strength",
                     &this->edit_state.brush_strength,
                     0.01f,
                     1.f,
                     "%.2f");

  hesiod::gui::hmap_brush_editor(this->edit_state,
                                 (ImTextureID)this->edit_state.draw_texture);

  if (ImGui::Button("Clear"))
  {
    hmap::transform(this->edit_state.pending_hm,
                    [](hmap::Array &m) { m = 0.f; });
    sync_drawing_texture();
    sync_value();
  }
  ImGui::SameLine();
  if (ImGui::Button("Smooth"))
  {
    hmap::transform(this->edit_state.pending_hm,
                    [this](hmap::Array &m) {
                      hmap::smooth_gaussian(m, this->edit_state.blur_strength);
                    });
    this->edit_state.pending_hm.smooth_overlap_buffers();
    sync_drawing_texture();
    sync_value();
  }
  ImGui::SameLine();
  ImGui::SliderInt("ir", &this->edit_state.blur_strength, 1, 64);

  if (this->remap)
  {
    ImGui::Checkbox("inverse", &this->inverse);
    has_changed |= this->trigger_update_after_edit();

    if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
    {
      this->force_update();
      has_changed = true;
    }
  }

  if (was_drawing && !this->edit_state.is_drawing)
  {
    sync_value();
  }

  has_changed |= this->render_attribute_settings();
  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewBrush::sync_drawing_texture()
{
  hmap::Array array = this->edit_state.pending_hm.to_array(
      this->edit_state.draw_shape);

  std::vector<uint8_t> img = hmap::colorize(array,
					    0.f,
                                            this->edit_state.max_height,
                                            hmap::cmap::inferno,
                                            false);
  img_to_texture_rgb(img, array.shape, this->edit_state.draw_texture);
}

void ViewBrush::sync_value()
{
  this->value_out = this->edit_state.pending_hm;
  this->force_update();
}

} // namespace hesiod::vnode

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
  glGenTextures(1, &draw_texture);
}

bool ViewBrush::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  bool was_drawing = this->edit_state.is_drawing;
  auto now = std::chrono::steady_clock::now();

  if (!was_drawing)
    this->edit_state.last_mouse_pos = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };

  if (this->edit_state.is_initted)
  {
    if (!this->edit_state.pending_changes.empty()) {
      edit_state.apply_pending_changes();
      sync_drawing_texture();
    }
  }
  else {
    this->edit_state.pending_hm = hmap::HeightMap();
    this->edit_state.pending_hm.set_sto(this->value_out.shape, this->value_out.tiling, this->value_out.overlap);
    this->edit_state.is_initted = true;
  }

  if (ImGui::SliderFloat("max height", &this->edit_state.max_height, 0.1f, 2.0f, "%.1f"))
  {
    hmap::transform(this->edit_state.pending_hm, [this](hmap::Array& m) { hmap::clamp(m, 0.0f, this->edit_state.max_height); });
    this->edit_state.brush_strength = std::min(this->edit_state.brush_strength, this->edit_state.max_height);
    sync_drawing_texture();
  }
  ImGui::SliderFloat("strength", &this->edit_state.brush_strength, 0.01f, this->edit_state.max_height, "%.2f");

  hesiod::gui::hmap_brush_editor(this->edit_state, (ImTextureID)this->draw_texture);

  if (ImGui::Button("Clear"))
  {
    hmap::transform(this->edit_state.pending_hm, [](hmap::Array& m) { m = 0.0f; });
    sync_drawing_texture();
    sync_value();
  }
  ImGui::SameLine();
  if (ImGui::Button("Smooth"))
  {
    hmap::transform(this->edit_state.pending_hm, [this](hmap::Array& m) { hmap::smooth_gaussian(m, this->edit_state.blur_strength); });
    this->edit_state.pending_hm.smooth_overlap_buffers();
    sync_drawing_texture();
    sync_value();
  }
  ImGui::SameLine();
  ImGui::SliderInt("ir", &this->edit_state.blur_strength, 2, 32);

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

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewBrush::sync_drawing_texture()
{
  auto array = this->edit_state.pending_hm.to_array();
  std::vector<uint8_t> img(array.shape.x * array.shape.y);
  int k = 0;
  for (int j = array.shape.y - 1; j > -1; --j)
  {
    for (int i = 0; i < array.shape.x; ++i)
    {
      img[k++] = (uint8_t)(array(i, j) / this->edit_state.max_height * 255.0f);
    }
  }
  glBindTexture(GL_TEXTURE_2D, (GLuint)this->draw_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D,
    0,
    GL_RGBA,
    this->edit_state.pending_hm.shape.x,
    this->edit_state.pending_hm.shape.y,
    0,
    GL_LUMINANCE,
    GL_UNSIGNED_BYTE,
    img.data());
}

void ViewBrush::sync_value()
{
  this->value_out = this->edit_state.pending_hm;
  this->force_update();
}

} // namespace hesiod::vnode

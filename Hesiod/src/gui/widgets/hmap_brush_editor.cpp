/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdio>
#include <iostream>

#include "highmap.hpp"
#include "imgui_internal.h"
#include "macrologger.h"

#include "hesiod/hmap_brush_editor.hpp"

namespace hesiod::gui
{

void hmap_brush_editor(HmapBrushEditorState &edit_state,
                       ImTextureID         canvas_texture,
                       float               width)
{
  ImGui::PushID((void *)&edit_state);
  ImGui::BeginGroup();

  // --- canvas
  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

  ImVec2 canvas_size;
  if (width == 0.f)
  {
    canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 75.0f)
      canvas_size.x = 75.0f;
    canvas_size.y = canvas_size.x; // square canvas
  }
  else
  {
    canvas_size = {width, width};
  }
  edit_state.canvas_size = canvas_size;

  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x,
                            canvas_p0.y + canvas_size.y);

  // draw canvas and points
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddImage(canvas_texture, canvas_p0, canvas_p1);

  // mouse interactions
  ImGui::InvisibleButton("canvas",
                         canvas_size,
                         ImGuiButtonFlags_MouseButtonLeft |
                             ImGuiButtonFlags_MouseButtonRight);
  ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
  ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
  bool is_canvas_hovered = ImGui::IsItemHovered();

  ImGuiIO                &io = ImGui::GetIO();
  const hmap::Vec2<float> mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x,
                                              io.MousePos.y - canvas_p0.y);
  bool                    mouse_moved_enough = false;
  auto mouse_delta = mouse_pos_in_canvas - edit_state.last_mouse_pos;
  if (ImGui::IsAnyMouseDown() && std::sqrt(mouse_delta.x * mouse_delta.x +
                                           mouse_delta.y * mouse_delta.y) >=
                                     std::sqrt(edit_state.brush_radius))
  {
    edit_state.last_mouse_pos = mouse_pos_in_canvas;
    mouse_moved_enough = true;
  }

  if (is_canvas_hovered)
  {
    // draw brush size
    draw_list->AddCircle(io.MousePos,
                         edit_state.brush_radius,
                         IM_COL32(255, 255, 255, 255));

    // remap mouse coordinates to [0, 1] and reverse y-axis
    float x = (io.MousePos.x - canvas_p0.x) / canvas_size.x;
    float y = 1.f - (io.MousePos.y - canvas_p0.y) / canvas_size.y;

    auto strength = edit_state.brush_strength;
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
      edit_state.is_drawing = true;
      if (mouse_moved_enough)
      {
        edit_state.add_change({x, y}, strength);
      }
    }
    else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
      edit_state.is_drawing = true;
      if (mouse_moved_enough)
      {
        edit_state.add_change({x, y}, -strength);
      }
    }
    else
    {
      edit_state.is_drawing = false;
    }

    // brush size
    if (io.MouseWheel)
    {
      float step = edit_state.brush_radius * 0.1f;
      if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
        step *= 0.1f;
      edit_state.brush_radius = std::max(1.f,
                                         edit_state.brush_radius +
                                             io.MouseWheel * step);
    }
  }

  // display relative radius (grid size independent)
  ImGui::Text("Brush radius: %.2f", edit_state.brush_radius / canvas_size.x);

  ImGui::EndGroup();
  ImGui::PopID();
}

void HmapBrushEditorState::add_change(hmap::Vec2<float> pos, float weight)
{
  if (pending_changes.size() > 128)
    return;
  pending_changes.push_back({pos, weight});
}

void HmapBrushEditorState::apply_pending_changes()
{
  int ir = (int)(brush_radius / canvas_size.x * pending_hm.shape.x);

  std::vector<hmap::Array>       kernels;
  std::vector<hmap::Vec2<float>> positions;
  for (auto &[pos, weight] : pending_changes)
  {
    hmap::Array kernel = weight * hmap::cubic_pulse(
                                      hmap::Vec2<int>(2 * ir + 1, 2 * ir + 1));
    kernels.push_back(kernel);
    positions.push_back(pos);
  }
  apply_brushes(pending_hm, kernels, positions);
  hmap::transform(pending_hm,
                  [this](hmap::Array &m) { hmap::clamp(m, 0, max_height); });
  pending_hm.smooth_overlap_buffers();
  pending_changes.clear();
}

void HmapBrushEditorState::apply_brushes(hmap::HeightMap             &h,
                                         std::span<hmap::Array>       kernels,
                                         std::span<hmap::Vec2<float>> positions)
{
  hmap::transform(h,
                  [&kernels, &positions](hmap::Array &z, hmap::Vec4<float> bbox)
                  {
                    for (size_t i = 0; i < kernels.size(); ++i)
                    {
                      hmap::Vec2<float> pos = positions[i];
                      int ic = (int)((pos.x - bbox.a) / (bbox.b - bbox.a) *
                                     (z.shape.x - 1));
                      int jc = (int)((pos.y - bbox.c) / (bbox.d - bbox.c) *
                                     (z.shape.y - 1));
                      add_kernel(z, kernels[i], ic, jc);
                    }
                  });
}

} // namespace hesiod::gui

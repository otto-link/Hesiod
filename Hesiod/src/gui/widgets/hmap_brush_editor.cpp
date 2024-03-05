/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdio>
#include <iostream>

#include "highmap.hpp"
#include "imgui_internal.h"
#include "macrologger.h"

#include "hesiod/gui.hpp"

#define IMGUI_ID_BRUSH_RADIUS 0

namespace hesiod::gui
{


void hmap_brush_editor(hesiod::vnode::ViewBrush::HmBrushEditorState& edit_state, ImTextureID canvas_texture, float width)
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
    canvas_size = { width, width };
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

  // brush radius in pixels
  int ir = (int)(edit_state.brush_radius / canvas_size.x * edit_state.pending_hm.shape.x);

  ImGuiIO     &io = ImGui::GetIO();
  const hmap::Vec2<float> mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x,
                                   io.MousePos.y - canvas_p0.y);
  bool mouse_moved_enough = false;
  auto mouse_delta = mouse_pos_in_canvas - edit_state.last_mouse_pos;
  if (ImGui::IsAnyMouseDown() && std::sqrt(mouse_delta.x * mouse_delta.x + mouse_delta.y * mouse_delta.y) >= std::sqrt(edit_state.brush_radius))
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
        edit_state.add_change({ x, y }, strength);
      }
    }
    else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
      edit_state.is_drawing = true;
      if (mouse_moved_enough)
      {
        edit_state.add_change({ x, y }, -strength);
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
      edit_state.brush_radius = std::max(1.f, edit_state.brush_radius + io.MouseWheel * step);
    }
  }

  ImGui::Text("Brush radius: %d pixels",
              (int)(edit_state.brush_radius / canvas_size.x * edit_state.pending_hm.shape.x));

  ImGui::EndGroup();
  ImGui::PopID();
}

} // namespace hesiod::gui

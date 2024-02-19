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

// HELPER

void add_brush(hmap::HeightMap &h, hmap::Array &kernel, float x, float y)
{
  hmap::transform(h,
                  [&kernel, &x, &y](hmap::Array      &z,
                                    hmap::Vec2<float> shift,
                                    hmap::Vec2<float> scale)
                  {
                    int ic = (int)((x - shift.x) / scale.x * (z.shape.x - 1));
                    int jc = (int)((y - shift.y) / scale.y * (z.shape.y - 1));
                    add_kernel(z, kernel, ic, jc);
                  });
}

bool hmap_brush_editor(hmap::HeightMap &h, float width)
{
  ImGuiStorage *imgui_storage = ImGui::GetStateStorage();
  float brush_radius = imgui_storage->GetFloat(IMGUI_ID_BRUSH_RADIUS, 16.f);

  bool ret = false;

  ImGui::PushID((void *)&h);
  ImGui::BeginGroup();

  // --- canvas
  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

  ImVec2 canvas_size;
  if (width == 0.f)
  {
    canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 50.0f)
      canvas_size.x = 50.0f;
    canvas_size.y = canvas_size.x; // square canvas
  }
  else
    canvas_size = {width, width};

  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x,
                            canvas_p0.y + canvas_size.y);

  // draw canvas and points
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

  // mouse interactions
  ImGui::InvisibleButton("canvas",
                         canvas_size,
                         ImGuiButtonFlags_MouseButtonLeft |
                             ImGuiButtonFlags_MouseButtonRight);
  ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
  ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
  bool is_canvas_hovered = ImGui::IsItemHovered();

  ImGuiIO     &io = ImGui::GetIO();
  const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x,
                                   io.MousePos.y - canvas_p0.y);

  if (is_canvas_hovered)
  {
    // draw brush size
    draw_list->AddCircle(io.MousePos,
                         brush_radius,
                         IM_COL32(255, 255, 255, 255));

    // remap mouse coordinates to [0, 1] and reverse y-axis
    float x = (io.MousePos.x - canvas_p0.x) / canvas_size.x;
    float y = 1.f - (io.MousePos.y - canvas_p0.y) / canvas_size.y;

    // brush radius in pixels
    int ir = (int)(brush_radius / canvas_size.x * h.shape.x);

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) // move
    {
    }
    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
      hmap::Array kernel = hmap::cubic_pulse(
          hmap::Vec2<int>(2 * ir + 1, 2 * ir + 1));
      add_brush(h, kernel, x, y);
      ret = true;
    }
    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
      hmap::Array kernel = -hmap::cubic_pulse(
          hmap::Vec2<int>(2 * ir + 1, 2 * ir + 1));
      add_brush(h, kernel, x, y);
      ret = true;
    }

    // brush size
    if (io.MouseWheel)
    {
      float step = brush_radius * 0.1f;
      if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
        step *= 0.1f;
      brush_radius = std::max(1.f, brush_radius + io.MouseWheel * step);
      ret = true;
    }
  }

  ImGui::Text("Brush radius: %d pixels",
              (int)(brush_radius / canvas_size.x * h.shape.x));

  ImGui::EndGroup();
  imgui_storage->SetFloat(IMGUI_ID_BRUSH_RADIUS, brush_radius);

  return ret;
}

} // namespace hesiod::gui

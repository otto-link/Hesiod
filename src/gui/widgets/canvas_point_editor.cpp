/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdio>
#include <iostream>

#include "highmap.hpp"
#include "macrologger.h"

#include "hesiod/gui.hpp"

#define IMGUI_ID_HOVERED_POINT_INDEX 0
#define IMGUI_ID_VMAX 1
#define IMGUI_ID_RADIUS 2
#define IMGUI_ID_SEED 3

namespace hesiod::gui
{

bool canvas_point_editor(hmap::Cloud &cloud)
{
  ImGuiStorage *imgui_storage = ImGui::GetStateStorage();

  bool ret = false;
  ImGui::PushID((void *)&cloud);
  ImGui::BeginGroup();

  // --- button interactions
  int seed = imgui_storage->GetInt(IMGUI_ID_SEED, 1);

  if (ImGui::Button("Re-seed"))
  {
    seed = (int)time(NULL);
    cloud.randomize((uint)seed);
    ret = true;
  }

  if (ImGui::DragInt("seed", &seed))
  {
    cloud.randomize((uint)seed);
    ret = true;
  }

  float radius = imgui_storage->GetFloat(IMGUI_ID_RADIUS, 16.f);
  ImGui::SliderFloat("plotting radius", &radius, 1.f, 32.f, "%.1f");

  // --- canvas
  ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
  ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
  if (canvas_sz.x < 50.0f)
    canvas_sz.x = 50.0f;
  canvas_sz.y = canvas_sz.x; // square canvas

  ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x,
                            canvas_p0.y + canvas_sz.y);

  // draw canvas
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

  // mouse interactions
  ImGui::InvisibleButton("canvas",
                         canvas_sz,
                         ImGuiButtonFlags_MouseButtonLeft |
                             ImGuiButtonFlags_MouseButtonRight);
  bool is_canvas_hovered = ImGui::IsItemHovered();

  ImGuiIO     &io = ImGui::GetIO();
  const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x,
                                   io.MousePos.y - canvas_p0.y);

  // --- plot points
  int hovered_point_index = imgui_storage->GetInt(IMGUI_ID_HOVERED_POINT_INDEX,
                                                  -1);
  float vmax = imgui_storage->GetFloat(IMGUI_ID_VMAX, 1.f);

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    hovered_point_index = -1;

  for (auto &p : cloud.points)
  {
    float x_canvas = canvas_p0.x + p.x * canvas_sz.x;
    float y_canvas = canvas_p0.y + p.y * canvas_sz.y;

    draw_list->AddCircle(ImVec2(x_canvas, y_canvas), radius, IM_COL32_WHITE);

    int red = (int)(255.f * std::max(0.f, p.v) / vmax);
    int blue = (int)(-255.f * std::min(0.f, p.v) / vmax);

    draw_list->AddCircleFilled(ImVec2(x_canvas, y_canvas),
                               radius,
                               IM_COL32(red, 255 - red - blue, blue, 127));
    char buffer[10];
    std::snprintf(buffer, 10, "%.2f", p.v);
    draw_list->AddText(ImVec2(x_canvas, y_canvas), IM_COL32_WHITE, buffer);
  }

  // --- handle mouse events
  if (is_canvas_hovered)
  {
    // check hovering
    for (int k = 0; k < (int)cloud.get_npoints(); k++)
    {
      if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
      {
        float x_canvas = canvas_p0.x + cloud.points[k].x * canvas_sz.x;
        float y_canvas = canvas_p0.y + cloud.points[k].y * canvas_sz.y;

        float dist = (io.MousePos.x - x_canvas) * (io.MousePos.x - x_canvas) +
                     (io.MousePos.y - y_canvas) * (io.MousePos.y - y_canvas);
        if (dist < radius * radius)
        {
          hovered_point_index = k;
          break;
        }
      }
    }

    if (hovered_point_index > -1)
    {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) // move
      {
        float x = (io.MousePos.x - canvas_p0.x) /
                  canvas_sz.x; // remap to [0, 1]
        float y = (io.MousePos.y - canvas_p0.y) / canvas_sz.y;
        cloud.points[hovered_point_index].x = std::clamp(x, 0.f, 1.f);
        cloud.points[hovered_point_index].y = std::clamp(y, 0.f, 1.f);
      }
      else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
      {
        hovered_point_index = -1;
        ret = true;
      }

      if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) // remove
      {
        cloud.remove_point(hovered_point_index);
        hovered_point_index = -1;
        ret = true;
      }

      if (io.MouseWheel) // change value
      {
        float step = 0.05f;
        if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
          step *= 0.1f;
        cloud.points[hovered_point_index].v += io.MouseWheel * step;
        ret = true;
      }
    }
    else
    {
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) // add
      {
        float x = (io.MousePos.x - canvas_p0.x) / canvas_sz.x;
        float y = (io.MousePos.y - canvas_p0.y) / canvas_sz.y;
        cloud.add_point(hmap::Point(x, y));
        ret = true;
      }
    }
  }

  // update colorscale amplitude if there has been some interations
  if (ret && cloud.get_npoints() > 0)
    vmax = std::max(std::abs(cloud.get_values_min()),
                    std::abs(cloud.get_values_max()));

  ImGui::EndGroup();
  ImGui::PopID();
  imgui_storage->SetInt(IMGUI_ID_HOVERED_POINT_INDEX, hovered_point_index);
  imgui_storage->SetFloat(IMGUI_ID_RADIUS, radius);
  imgui_storage->SetFloat(IMGUI_ID_VMAX, vmax);
  imgui_storage->SetInt(IMGUI_ID_SEED, seed);

  return ret;
}

} // namespace hesiod::gui

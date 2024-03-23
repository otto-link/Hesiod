/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdio>
#include <iostream>

#include "imgui_internal.h"
#include "macrologger.h"

#include "hesiod/widgets.hpp"

#define IMGUI_ID_HOVERED_POINT_INDEX 0
#define IMGUI_ID_RADIUS 1
#define IMGUI_ID_SEED 2

namespace hesiod::gui
{

// --- HELPERS

void draw_polyline(std::vector<hmap::Point> &points,
                   ImVec2                    canvas_p0,
                   ImVec2                    canvas_size,
                   bool                      closed)
{
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  if (points.size())
  {
    uint kmax = points.size() - 1;
    if (closed)
      kmax++;

    for (size_t k = 0; k < kmax; k++)
    {
      float x1 = canvas_p0.x + points[k].x * canvas_size.x;
      float y1 = canvas_p0.y + (1.f - points[k].y) * canvas_size.y;

      int   kpp = (k + 1) % points.size();
      float x2 = canvas_p0.x + points[kpp].x * canvas_size.x;
      float y2 = canvas_p0.y + (1.f - points[kpp].y) * canvas_size.y;

      draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32_WHITE);
    }
  }
}

void draw_points(hmap::Cloud &cloud,
                 float        radius,
                 ImVec2       canvas_p0,
                 ImVec2       canvas_size)
{
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  float vmax = 1.f;
  if (cloud.get_npoints() > 0)
    vmax = std::max(std::abs(cloud.get_values_min()),
                    std::abs(cloud.get_values_max()));

  for (auto &p : cloud.points)
  {
    float x_canvas = canvas_p0.x + p.x * canvas_size.x;
    float y_canvas = canvas_p0.y + (1.f - p.y) * canvas_size.y;

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
}

// --- WIDGETS

void hmap_cloud(hmap::Cloud &cloud, float width, float radius)
{
  ImGui::PushID((void *)&cloud);
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

  ImGui::InvisibleButton("canvas", canvas_size);

  // draw canvas and points
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_points(cloud, radius, canvas_p0, canvas_size);

  ImGui::EndGroup();
  ImGui::PopID();
}

bool hmap_cloud_editor(hmap::Cloud &cloud,
                       float        width,
                       ImVec2      *p_canvas_p0,
                       ImVec2      *p_canvas_size)
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

  // draw canvas
  {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2      canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x,
                              canvas_p0.y + canvas_size.y);
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
  }

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

  // --- draw points
  draw_points(cloud, radius, canvas_p0, canvas_size);

  // --- handle mouse events

  if (is_canvas_hovered)
  {
    int hovered_point_index = imgui_storage->GetInt(
        IMGUI_ID_HOVERED_POINT_INDEX,
        -1);

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
      hovered_point_index = -1;

    // check hovering
    for (int k = 0; k < (int)cloud.get_npoints(); k++)
    {
      if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
      {
        float x_canvas = canvas_p0.x + cloud.points[k].x * canvas_size.x;
        float y_canvas = canvas_p0.y +
                         (1.f - cloud.points[k].y) * canvas_size.y;

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
        // remap to [0, 1]
        float x = (io.MousePos.x - canvas_p0.x) / canvas_size.x;
        float y = (io.MousePos.y - canvas_p0.y) / canvas_size.y;
        cloud.points[hovered_point_index].x = std::clamp(x, 0.f, 1.f);
        cloud.points[hovered_point_index].y = 1.f - std::clamp(y, 0.f, 1.f);
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
        float x = (io.MousePos.x - canvas_p0.x) / canvas_size.x;
        float y = 1.f - (io.MousePos.y - canvas_p0.y) / canvas_size.y;
        cloud.add_point(hmap::Point(x, y, 1.f));
        ret = true;
      }
    }

    imgui_storage->SetInt(IMGUI_ID_HOVERED_POINT_INDEX, hovered_point_index);
  }

  if (cloud.get_npoints())
    if (ImGui::Button("Clear"))
      cloud.clear();

  ImGui::EndGroup();
  ImGui::PopID();
  imgui_storage->SetFloat(IMGUI_ID_RADIUS, radius);
  imgui_storage->SetInt(IMGUI_ID_SEED, seed);

  if (p_canvas_p0)
    *p_canvas_p0 = canvas_p0;
  if (p_canvas_size)
    *p_canvas_size = canvas_size;

  return ret;
}

void hmap_path(hmap::Path &path, float width)
{
  ImGui::PushID((void *)&path);
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

  ImGui::InvisibleButton("canvas", canvas_size);

  // draw canvas and points
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
  draw_polyline(path.points, canvas_p0, canvas_size, path.closed);

  ImGui::EndGroup();
  ImGui::PopID();
}

bool hmap_path_editor(hmap::Path &path, float width)
{
  bool ret = false;

  ImVec2 canvas_p0;
  ImVec2 canvas_size;

  ret |=
      hmap_cloud_editor((hmap::Cloud &)path, width, &canvas_p0, &canvas_size);

  draw_polyline(path.points, canvas_p0, canvas_size, path.closed);

  if (path.get_npoints())
  {
    if (ImGui::Button("Reorder"))
      path.reorder_nns();

    ImGui::SameLine();

    if (ImGui::Button("Divide"))
      path.divide();

    ImGui::SameLine();

    if (ImGui::Button("Resample"))
      path.resample_uniform();
  }

  return ret;
}

} // namespace hesiod::gui

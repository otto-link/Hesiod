/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <map>
#include <string>
#include <vector>

#include "highmap.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/view_tree.hpp"

namespace hesiod::gui
{

enum icon_type : int
{
  circle,
  square
};

void glfw_error_callback(int error, const char *description);

GLFWwindow *init_gui(int width, int height, std::string window_title);

void save_screenshot(std::string fname);

// main GUI
void main_dock(hesiod::vnode::ViewTree &view_tree);

// custom ImGui widgets
void canvas_cloud(hmap::Cloud &cloud, float width = 0.f, float radius = 4.f);

bool canvas_cloud_editor(hmap::Cloud &cloud,
                         float        width = 0.f,
                         ImVec2      *p_canvas_p0 = nullptr,
                         ImVec2      *p_canvas_size = nullptr);

void canvas_path(hmap::Path &path, float width = 0.f);

bool canvas_path_editor(hmap::Path &path, float width = 0.f);

bool drag_float_matrix(std::vector<std::vector<float>> &matrix);

bool drag_float_vec2(hmap::Vec2<float> &kw, bool &link_xy);

bool drag_float_vector(std::vector<float> &vector,
                       bool                monotonic = false,
                       bool                variable_size = true,
                       float               vmin = 0.f,
                       float               vmax = 1.f,
                       bool                horizontal = false,
                       float               height = 160.f);

void draw_icon(int icon_type, ImVec2 size, ImU32 color, bool filled);

void help_marker(std::string text);

bool hmap_brush_editor(hmap::HeightMap &h, float width = 0.f);

bool listbox_map_enum(std::map<std::string, int> &map,
                      int                        &selected,
                      float                       width);

bool select_shape(std::string      label,
                  hmap::Vec2<int> &shape,
                  hmap::Vec2<int>  shape_max);

bool slider_vmin_vmax(float &vmin, float &vmax);

} // namespace hesiod::gui

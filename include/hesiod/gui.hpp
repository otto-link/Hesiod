/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <map>
#include <string>
#include <vector>

#include "highmap.hpp"

#include "imnodes.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace hesiod::gui
{

void glfw_error_callback(int error, const char *description);

GLFWwindow *init_gui(int width, int height, std::string window_title);

// custom ImGui widgets

bool drag_float_matrix(std::vector<std::vector<float>> &matrix);

bool drag_float_vec2(hmap::Vec2<float> &kw, bool &link_xy);

bool drag_float_vector(std::vector<float> &vector,
                       bool                monotonic = false,
                       bool                variable_size = true,
                       float               vmin = 0.f,
                       float               vmax = 1.f,
                       bool horizontal = true,
                       float               height = 160.f);

bool listbox_map_enum(std::map<std::string, int> &map, int &selected);

bool slider_vmin_vmax(float &vmin, float &vmax);

} // namespace hesiod::gui
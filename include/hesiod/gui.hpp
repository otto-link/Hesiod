/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>
#include <vector>

#include "imnodes.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace hesiod::gui
{

static void glfw_error_callback(int error, const char *description);

GLFWwindow *init_gui(int width, int height, std::string window_title);

// custom ImGui widgets

bool drag_float_matrix(std::vector<std::vector<float>> &matrix);

bool slider_vmin_vmax(float &vmin, float &vmax);

} // namespace hesiod::gui
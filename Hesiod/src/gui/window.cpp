/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/shortcuts.hpp"

namespace hesiod::gui
{

Window::Window() : title(), flags()
{
}

bool Window::render()
{
  bool res = true;
  if (ImGui::Begin(this->title.c_str(), nullptr, this->flags))
  {
    if (ImGui::IsWindowFocused())
    {
      this->p_parent_window_manager->get_shortcuts_manager_ref()
          ->set_focused_group_id(this->get_element_shortcut_group_id());
    }

    res &= this->render_window_content();
  }
  ImGui::End();
  return res;
}

bool Window::render_window_content()
{
  ImGui::Text("render_window_content not implemented.");
  return true;
}

bool Window::add_shortcuts()
{
  for (auto &s : this->shortcuts)
  {
    this->p_parent_window_manager->get_shortcuts_manager_ref()->add_shortcut(
        std::move(s));
  }
  return true;
}

bool Window::remove_shortcuts()
{
  for (auto &s : shortcuts)
  {
    this->p_parent_window_manager->get_shortcuts_manager_ref()->remove_shortcut(
        s->get_id());
  }
  this->shortcuts.clear();
  return true;
}

} // namespace hesiod::gui
